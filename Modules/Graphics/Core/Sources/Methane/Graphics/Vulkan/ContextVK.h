/******************************************************************************

Copyright 2019 Evgeny Gorodetskiy

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

FILE: Methane/Graphics/Vulkan/ContextVK.h
Vulkan implementation of the context interface.

******************************************************************************/

#pragma once

#include <Methane/Graphics/ContextBase.h>

namespace Methane::Graphics
{

struct CommandQueue;
class RenderPassVK;
class DeviceVK;

class ContextVK : public ContextBase
{
public:
    ContextVK(const Platform::AppEnvironment& env, DeviceBase& device, const Settings& settings);
    ~ContextVK() override;

    // Context interface
    bool ReadyToRender() const override;
    void OnCommandQueueCompleted(CommandQueue& cmd_queue, uint32_t frame_index) override;
    void WaitForGpu(WaitFor wait_for) override;
    void Resize(const FrameSize& frame_size) override;
    void Present() override;
    bool SetVSyncEnabled(bool vsync_enabled) override;
    bool SetFrameBuffersCount(uint32_t frame_buffers_count) override;
    Platform::AppView GetAppView() const override { return { nullptr }; }
    float GetBackingScaleFactor() const override { return 1.f; }

    DeviceVK& GetDeviceVK();

protected:
    // ContextBase overrides
    void Release() override;
    void Initialize(Device& device, bool deferred_heap_allocation) override;
};

} // namespace Methane::Graphics
