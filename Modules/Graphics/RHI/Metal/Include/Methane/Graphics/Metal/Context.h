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

FILE: Methane/Graphics/Metal/Context.h
Metal context accessor interface for template class Context<ContextBaseT>

******************************************************************************/

#pragma once

#include <Methane/Graphics/RHI/ICommandList.h>
#include <Methane/Memory.hpp>

#include <string>

namespace Methane::Graphics::Metal
{

class Device;
class CommandQueue;
class ProgramLibrary;

struct IContext
{
    virtual const Device& GetMetalDevice() const noexcept = 0;
    virtual CommandQueue& GetMetalDefaultCommandQueue(CommandListType type) = 0;
    virtual const Ptr<ProgramLibrary>& GetMetalLibrary(const std::string& library_name = "") const = 0;

    virtual ~IContext() = default;
};

} // namespace Methane::Graphics::Metal
