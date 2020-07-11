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

FILE: Methane/UserInterface/Badge.cpp
Badge rendering primitive displaying fixed texture in specific corner of the screen.

******************************************************************************/

#include <Methane/UserInterface/Badge.h>

#include <Methane/Graphics/ImageLoader.h>
#include <Methane/Data/AppResourceProviders.h>
#include <Methane/Instrumentation.h>

#include <cmath>

namespace Methane::UserInterface
{

static Badge::Settings ScaleBadgeSize(Badge::Settings settings, float scale_factor)
{
    settings.size.width  = static_cast<uint32_t>(std::round(scale_factor * settings.size.width));
    settings.size.height = static_cast<uint32_t>(std::round(scale_factor * settings.size.height));
    settings.margins     = settings.margins * scale_factor;
    return settings;
}

Badge::Badge(gfx::RenderContext& context, Data::Provider& data_provider, const std::string& image_path, Settings settings)
    : Badge(context,
            gfx::ImageLoader(data_provider).LoadImageToTexture2D(context, image_path),
            ScaleBadgeSize(settings, context.GetContentScalingFactor()))
{
    META_FUNCTION_TASK();
}

Badge::Badge(gfx::RenderContext& context, Ptr<gfx::Texture> sp_texture, Settings settings)
    : ScreenQuad(context, std::move(sp_texture),
        ScreenQuad::Settings
        {
            settings.name,
            GetBadgeRectInFrame(context.GetSettings().frame_size, settings),
            true,
            settings.blend_color,
            settings.texture_mode
        }
    )
    , m_settings(std::move(settings))
    , m_context(context)
{
    META_FUNCTION_TASK();
}

void Badge::FrameResize(const gfx::FrameSize& frame_size, std::optional<gfx::FrameSize> badge_size, std::optional<gfx::Point2i> margins)
{
    META_FUNCTION_TASK();
    if (badge_size)
    {
        m_settings.size = *badge_size;
    }
    if (margins)
    {
        m_settings.margins = *margins;
    }
    SetScreenRect(GetBadgeRectInFrame(frame_size));
}

void Badge::SetCorner(FrameCorner frame_corner)
{
    META_FUNCTION_TASK();
    m_settings.corner = frame_corner;
    SetScreenRect(GetBadgeRectInFrame(m_context.GetSettings().frame_size));
}

void Badge::SetMargins(gfx::Point2i& margins)
{
    META_FUNCTION_TASK();
    m_settings.margins = margins;
    SetScreenRect(GetBadgeRectInFrame(m_context.GetSettings().frame_size));
}

void Badge::SetSize(const gfx::FrameSize& size)
{
    META_FUNCTION_TASK();
    if (m_settings.size == size)
        return;

    m_settings.size = size;
    SetScreenRect(GetBadgeRectInFrame(m_context.GetSettings().frame_size));
}

gfx::FrameRect Badge::GetBadgeRectInFrame(const gfx::FrameSize& frame_size)
{
    return GetBadgeRectInFrame(frame_size, m_settings);
}

gfx::FrameRect Badge::GetBadgeRectInFrame(const gfx::FrameSize& frame_size, const Badge::Settings& settings)
{
    META_FUNCTION_TASK();

    switch(settings.corner)
    {
    case FrameCorner::TopLeft:
        return gfx::FrameRect{ settings.margins, settings.size };

    case FrameCorner::TopRight:
        return gfx::FrameRect{ gfx::FrameRect::Point(frame_size.width - settings.size.width - settings.margins.GetX(), settings.margins.GetY()), settings.size };

    case FrameCorner::BottomLeft:
        return gfx::FrameRect{ gfx::FrameRect::Point(settings.margins.GetX(), frame_size.height - settings.size.height - settings.margins.GetY()), settings.size };

    case FrameCorner::BottomRight:
        return gfx::FrameRect{
            gfx::FrameRect::Point(frame_size.width  - settings.size.width, frame_size.height - settings.size.height) - settings.margins,
            settings.size
        };
    }

    return gfx::FrameRect();
}

} // namespace Methane::UserInterface
