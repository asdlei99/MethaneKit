/******************************************************************************

Copyright 2019-2020 Evgeny Gorodetskiy

Licensed under the Apache License, Version 2.0 (the "License"),
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************

FILE: Methane/Graphics/App.hpp
Base template class of the Methane graphics application with multiple frame buffers.
Base frame class provides frame buffer management with resize handling.

******************************************************************************/

#pragma once

#include "AppBase.h"

#include <Methane/Data/AppResourceProviders.h>
#include <Methane/Graphics/AppController.h>
#include <Methane/Graphics/RHI/ITexture.h>
#include <Methane/Graphics/RHI/IRenderPass.h>
#include <Methane/Instrumentation.h>
#include <Methane/Checks.hpp>

#include <vector>

namespace Methane::Graphics
{

struct AppFrame
{
    const uint32_t        index = 0;
    Ptr<Rhi::ITexture>    screen_texture_ptr;
    Ptr<Rhi::IRenderPass> screen_pass_ptr;

    explicit AppFrame(uint32_t frame_index) : index(frame_index) { META_FUNCTION_TASK(); }
    virtual ~AppFrame() = default;

    // AppFrame interface
    virtual void ReleaseScreenPassAttachmentTextures()
    {
        screen_pass_ptr->ReleaseAttachmentTextures();
        screen_texture_ptr.reset();
    }
};

template<typename FrameT, typename IAppType = IApp, typename = std::enable_if_t<std::is_base_of_v<AppFrame, FrameT>>>
class App
    : public Graphics::AppBase
    , public IAppType
{
    static_assert(std::is_base_of_v<AppFrame, FrameT>, "Application Frame type must be derived from AppFrame.");

public:
    explicit App(const AppSettings& settings)
        : AppBase(settings, Data::TextureProvider::Get())
    { }

    App(const AppSettings& settings, const std::string& help_description)
        : App(settings)
    {
        META_FUNCTION_TASK();
        AddInputControllers({ std::make_shared<AppController>(*this, help_description) });
    }

    // WARNING: Don't forget to wait for GPU rendering completion in the derived class destructor to release resources properly
    // m_context_ptr->WaitForGpu(IRenderContext::WaitFor::RenderComplete)

    // Platform::App interface

    void Init() override
    {
        META_FUNCTION_TASK();
        AppBase::Init();

        // Create frame resources
        Rhi::IRenderContext& render_context = GetRenderContext();
        const Rhi::RenderContextSettings& context_settings = render_context.GetSettings();
        for (uint32_t frame_index = 0; frame_index < context_settings.frame_buffers_count; ++frame_index)
        {
            FrameT frame(frame_index);

            // Create color texture for frame buffer
            frame.screen_texture_ptr = Rhi::ITexture::CreateFrameBuffer(render_context, frame.index);
            frame.screen_texture_ptr->SetName(IndexedName("Frame Buffer", frame.index));

            // Configure render pass: color, depth, stencil attachments and shader access
            frame.screen_pass_ptr = CreateScreenRenderPass(*frame.screen_texture_ptr);

            m_frames.emplace_back(std::move(frame));
        }
    }

    bool Resize(const FrameSize& frame_size, bool is_minimized) override
    {
        META_FUNCTION_TASK();
        if (!AppBase::Resize(frame_size, is_minimized))
            return false;

        // Save frame and depth textures restore information and delete obsolete resources
        std::vector<ResourceRestoreInfo> frame_restore_infos;
        frame_restore_infos.reserve(m_frames.size());
        for (FrameT& frame : m_frames)
        {
            META_CHECK_ARG_NOT_NULL(frame.screen_texture_ptr);
            frame_restore_infos.emplace_back(*frame.screen_texture_ptr);
            frame.ReleaseScreenPassAttachmentTextures();
        }
        const Opt<ResourceRestoreInfo> depth_restore_info_opt = ReleaseDepthTexture();

        // Resize render context
        GetRenderContext().Resize(frame_size);

        // Restore frame and depth buffers with new size and update textures in render pass settings
        RestoreDepthTexture(depth_restore_info_opt);
        for (FrameT& frame : m_frames)
        {
            ResourceRestoreInfo& frame_restore_info = frame_restore_infos[frame.index];
            frame.screen_texture_ptr = Rhi::ITexture::CreateFrameBuffer(GetRenderContext(), frame.index);
            frame.screen_texture_ptr->RestoreDescriptorViews(frame_restore_info.descriptor_by_view_id);
            frame.screen_texture_ptr->SetName(frame_restore_info.name);
            frame.screen_pass_ptr->Update({
                GetScreenPassAttachments(*frame.screen_texture_ptr),
                frame_size
            });
        }

        return true;
    }

    // Graphics::IApp interface

    const Graphics::IApp::Settings& GetGraphicsAppSettings() const noexcept override { return AppBase::GetBaseGraphicsAppSettings(); }
    bool  SetAnimationsEnabled(bool animations_enabled) override                     { return SetBaseAnimationsEnabled(animations_enabled); }

protected:
    void OnContextReleased(Rhi::IContext& context) override
    {
        META_FUNCTION_TASK();
        AppBase::OnContextReleased(context);
        m_frames.clear();
    }

    FrameT& GetCurrentFrame()
    {
        META_FUNCTION_TASK();
        return m_frames.at(GetRenderContext().GetFrameBufferIndex());
    }

    const FrameT& GetCurrentFrame() const
    {
        META_FUNCTION_TASK();
        return m_frames.at(GetRenderContext().GetFrameBufferIndex());
    }

    std::vector<FrameT>& GetFrames() noexcept { return m_frames; }

private:
    std::vector<FrameT> m_frames;
};

} // namespace Methane::Graphics
