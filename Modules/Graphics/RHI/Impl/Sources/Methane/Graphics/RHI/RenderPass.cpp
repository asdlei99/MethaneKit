/******************************************************************************

Copyright 2022 Evgeny Gorodetskiy

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

FILE: Methane/Graphics/RHI/RenderPass.cpp
Methane RenderPass PIMPL wrappers for direct calls to final implementation.

******************************************************************************/

#include <Methane/Graphics/RHI/RenderPass.h>
#include <Methane/Graphics/RHI/RenderContext.h>

#if defined METHANE_GFX_DIRECTX

#include <Methane/Graphics/DirectX/RenderPass.h>
using RenderPatternImpl = Methane::Graphics::Base::RenderPattern;
using RenderPassImpl = Methane::Graphics::DirectX::RenderPass;

#elif defined METHANE_GFX_VULKAN

#include <Methane/Graphics/Vulkan/RenderPass.h>
using RenderPatternImpl = Methane::Graphics::Vulkan::RenderPattern;
using RenderPassImpl = Methane::Graphics::Vulkan::RenderPass;

#elif defined METHANE_GFX_METAL

#include <Methane/Graphics/Metal/RenderPass.hh>
using RenderPatternImpl = Methane::Graphics::Base::RenderPattern;
using RenderPassImpl = Methane::Graphics::Metal::RenderPass;

#else // METHAN_GFX_[API] is undefined

static_assert(false, "Static graphics API macro-definition is missing.");

#endif

#include "ImplWrapper.hpp"

#include <Methane/Instrumentation.h>

namespace Methane::Graphics::Rhi
{

class RenderPattern::Impl : public ImplWrapper<IRenderPattern, RenderPatternImpl>
{
public:
    using ImplWrapper::ImplWrapper;
};

RenderPattern::RenderPattern(const Ptr<IRenderPattern>& interface_ptr)
    : m_impl_ptr(std::make_unique<Impl>(interface_ptr))
{
}

RenderPattern::RenderPattern(const RenderContext& render_context, const Settings& settings)
    : RenderPattern(IRenderPattern::Create(render_context.GetInterface(), settings))
{
}

void RenderPattern::Init(const RenderContext& render_context, const Settings& settings)
{
    m_impl_ptr = std::make_unique<Impl>(IRenderPattern::Create(render_context.GetInterface(), settings));
}

void RenderPattern::Release()
{
    m_impl_ptr.release();
}

bool RenderPattern::IsInitialized() const noexcept
{
    return static_cast<bool>(m_impl_ptr);
}

IRenderPattern& RenderPattern::GetInterface() const noexcept
{
    return m_impl_ptr->GetInterface();
}

bool RenderPattern::SetName(const std::string& name) const
{
    return m_impl_ptr->Get().SetName(name);
}

const std::string& RenderPattern::GetName() const noexcept
{
    return m_impl_ptr->Get().GetName();
}

class RenderPass::Impl : public ImplWrapper<IRenderPass, RenderPassImpl>
{
public:
    using ImplWrapper::ImplWrapper;
};

RenderPass::RenderPass(const Ptr<IRenderPass>& interface_ptr)
    : m_impl_ptr(std::make_unique<Impl>(interface_ptr))
{
}

RenderPass::RenderPass(const Pattern& render_pattern, const Settings& settings)
    : RenderPass(IRenderPass::Create(render_pattern.GetInterface(), settings))
{
}

void RenderPass::Init(const Pattern& render_pattern, const Settings& settings)
{
    m_impl_ptr = std::make_unique<Impl>(IRenderPass::Create(render_pattern.GetInterface(), settings));
}

void RenderPass::Release()
{
    m_impl_ptr.release();
}

bool RenderPass::IsInitialized() const noexcept
{
    return static_cast<bool>(m_impl_ptr);
}

IRenderPass& RenderPass::GetInterface() const noexcept
{
    return m_impl_ptr->GetInterface();
}

bool RenderPass::SetName(const std::string& name) const
{
    return m_impl_ptr->Get().SetName(name);
}

const std::string& RenderPass::GetName() const noexcept
{
    return m_impl_ptr->Get().GetName();
}

} // namespace Methane::Graphics::Rhi
