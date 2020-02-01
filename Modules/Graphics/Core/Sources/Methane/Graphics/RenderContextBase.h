/******************************************************************************

Copyright 2019-2020 Evgeny Gorodetskiy

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************

FILE: Methane/Graphics/RenderContextBase.h
Base implementation of the render context interface.

******************************************************************************/

#pragma once

#include "ContextBase.h"

#include <Methane/Graphics/RenderContext.h>
#include <Methane/Graphics/FpsCounter.h>

#include <atomic>

namespace Methane::Graphics
{

class RenderContextBase
    : public ContextBase
    , public RenderContext
{
public:
    RenderContextBase(DeviceBase& device, const Settings& settings);

    // Context interface
    void WaitForGpu(WaitFor wait_for) override;

    // RenderContext interface
    void              Resize(const FrameSize& frame_size) override;
    void              Present() override;
    CommandQueue&     GetRenderCommandQueue() override;
    const Settings&   GetSettings() const override          { return m_settings; }
    uint32_t          GetFrameBufferIndex() const override  { return m_frame_buffer_index;  }
    const FpsCounter& GetFpsCounter() const override        { return m_fps_counter; }
    bool              SetVSyncEnabled(bool vsync_enabled) override;
    bool              SetFrameBuffersCount(uint32_t frame_buffers_count) override;
    bool              SetFullScreen(bool is_full_screen) override;

    // ContextBase overrides
    void Release() override;

protected:
    void ResetWithSettings(const Settings& settings);
    void OnCpuPresentComplete();

    // ContextBase overrides
    void OnGpuWaitComplete(WaitFor wait_for) override;

    Settings              m_settings;
    Ptr<CommandQueue>     m_sp_render_cmd_queue;
    std::atomic<uint32_t> m_frame_buffer_index;
    FpsCounter            m_fps_counter;
};

} // namespace Methane::Graphics
