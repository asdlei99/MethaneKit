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

FILE: Methane/Graphics/Metal/Device.mm
Metal implementation of the device interface.

******************************************************************************/

#include <Methane/Graphics/Metal/Device.hh>

#include <Methane/Platform/Apple/Types.hh>
#include <Methane/Instrumentation.h>
#include <Methane/Checks.hpp>

#include <algorithm>
#include <magic_enum.hpp>

namespace Methane::Graphics::Rhi
{

ISystem& ISystem::Get()
{
    META_FUNCTION_TASK();
    static const auto s_system_ptr = std::make_shared<Metal::System>();
    return *s_system_ptr;
}

} // namespace Methane::Graphics::Rhi

namespace Methane::Graphics::Metal
{

DeviceFeatures Device::GetSupportedFeatures(const id<MTLDevice>&)
{
    META_FUNCTION_TASK();
    DeviceFeatures supported_features = DeviceFeatures::BasicRendering;
    return supported_features;
}

Device::Device(const id<MTLDevice>& mtl_device, const Capabilities& capabilities)
    : Base::Device(MacOS::ConvertFromNsType<NSString, std::string>(mtl_device.name), false, capabilities)
    , m_mtl_device(mtl_device)
{
    META_FUNCTION_TASK();
}

System::~System()
{
    META_FUNCTION_TASK();
#ifdef APPLE_MACOS
    if (m_device_observer != nil)
        MTLRemoveDeviceObserver(m_device_observer);
#endif
}

const Ptrs<IDevice>& System::UpdateGpuDevices(const Platform::AppEnvironment&, const DeviceCaps& required_device_caps)
{
    META_FUNCTION_TASK();
    return UpdateGpuDevices(required_device_caps);
}

const Ptrs<IDevice>& System::UpdateGpuDevices(const DeviceCaps& required_device_caps)
{
    META_FUNCTION_TASK();
#ifdef APPLE_MACOS
    if (m_device_observer != nil)
        MTLRemoveDeviceObserver(m_device_observer);
#endif

    SetDeviceCapabilities(required_device_caps);
    ClearDevices();

#ifdef APPLE_MACOS
    NSArray<id<MTLDevice>>* mtl_devices = MTLCopyAllDevicesWithObserver(&m_device_observer,
        ^(id<MTLDevice> device, MTLDeviceNotificationName device_notification)
        {
            OnDeviceNotification(device, device_notification);
        });
    for(id<MTLDevice> mtl_device in mtl_devices)
    {
        AddDevice(mtl_device);
    }
#else
    const id<MTLDevice> mtl_device = MTLCreateSystemDefaultDevice();
    AddDevice(mtl_device);
#endif
    
    return GetGpuDevices();
}

#ifdef APPLE_MACOS
void System::OnDeviceNotification(id<MTLDevice> mtl_device, MTLDeviceNotificationName device_notification)
{
    META_FUNCTION_TASK();
    if (device_notification == MTLDeviceWasAddedNotification)
    {
        AddDevice(mtl_device);
    }
    else
    {
        const Ptr<IDevice>& device_ptr = FindMetalDevice(mtl_device);
        META_CHECK_ARG_NOT_NULL_DESCR(device_ptr, "no device object found");

        if (device_notification == MTLDeviceRemovalRequestedNotification)
            RequestRemoveDevice(*device_ptr);
        else if (device_notification == MTLDeviceWasRemovedNotification)
            RemoveDevice(*device_ptr);
    }
}
#endif

void System::AddDevice(const id<MTLDevice>& mtl_device)
{
    META_FUNCTION_TASK();
    using namespace magic_enum::bitwise_operators;

    DeviceFeatures device_supported_features = Device::GetSupportedFeatures(mtl_device);
    if (!static_cast<bool>(device_supported_features & GetDeviceCapabilities().features))
        return;

    Base::System::AddDevice(std::make_shared<Device>(mtl_device, GetDeviceCapabilities()));
}

const Ptr<IDevice>& System::FindMetalDevice(const id<MTLDevice>& mtl_device) const
{
    META_FUNCTION_TASK();
    const Ptrs<IDevice>& devices = GetGpuDevices();
    const auto device_it = std::find_if(devices.begin(), devices.end(),
                                        [&mtl_device](const Ptr<IDevice>& device_ptr)
                                        {
                                            META_CHECK_ARG_NOT_NULL(device_ptr);
                                            Device& metal_device = static_cast<Device&>(*device_ptr);
                                            return metal_device.GetNativeDevice() == mtl_device;
                                        });
    
    static const Ptr<IDevice> s_empty_device_ptr;
    return device_it != devices.end() ? *device_it : s_empty_device_ptr;
}

} // namespace Methane::Graphics::Metal
