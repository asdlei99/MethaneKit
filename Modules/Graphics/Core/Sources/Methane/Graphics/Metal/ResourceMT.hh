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

FILE: Methane/Graphics/Metal/ResourceMT.hh
Metal implementation of the resource interface.

******************************************************************************/
#pragma once

#include <Methane/Graphics/ResourceBase.h>

namespace Methane::Graphics
{

struct IContextMT;

class ResourceMT : public ResourceBase
{
public:
    class BarriersMT : public Barriers
    {
    public:
        BarriersMT(const Set& barriers) : Barriers(barriers) {}
    };

    ResourceMT(Type type, Usage::Mask usage_mask, ContextBase& context, const DescriptorByUsage& descriptor_by_usage);

protected:
    IContextMT& GetContextMT() noexcept;
};

} // namespace Methane::Graphics