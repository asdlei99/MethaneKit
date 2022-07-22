/******************************************************************************

Copyright 2019-2022 Evgeny Gorodetskiy

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

FILE: Methane/Graphics/Vulkan/SamplerVK.h
Vulkan implementation of the sampler interface.

******************************************************************************/

#pragma once

#include "ResourceVK.hpp"

#include <Methane/Graphics/SamplerBase.h>

#include <vulkan/vulkan.hpp>

namespace Methane::Graphics
{

struct IContextVK;

class SamplerVK final // NOSONAR - inheritance hierarchy is greater than 5
    : public ResourceVK<SamplerBase, vk::Sampler, false>
{
public:
    SamplerVK(const ContextBase& context, const Settings& settings);

    const vk::Sampler& GetNativeSampler() const noexcept { return m_vk_unique_sampler.get(); }

protected:
    // ResourceVK override
    Ptr<ResourceViewVK::ViewDescriptorVariant> CreateNativeViewDescriptor(const View::Id& view_id) override;
    
private:
    vk::UniqueSampler m_vk_unique_sampler;
};

} // namespace Methane::Graphics
