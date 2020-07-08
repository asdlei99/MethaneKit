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

FILE: HelloTriangleAppSimple.cpp
Simplified tutorial demonstrating triangle rendering with Methane graphics API

******************************************************************************/

#include <Methane/Kit.h>

using namespace Methane;
using namespace Methane::Graphics;

struct HelloTriangleFrame final : AppFrame
{
    Ptr<RenderCommandList> sp_render_cmd_list;
    Ptr<CommandListSet>    sp_render_cmd_lists;
    using AppFrame::AppFrame;
};

using GraphicsApp = App<HelloTriangleFrame>;
class HelloTriangleApp final : public GraphicsApp
{
private:
    Ptr<RenderState> m_sp_state;
    Ptr<BufferSet>   m_sp_vertex_buffers;

public:
    HelloTriangleApp() : GraphicsApp(
        {                                               // Application settings:
            {                                           // platform_app:
                "Methane Hello Triangle",               // - name
                0.8, 0.8,                               // - width, height
            },                                          //
            {                                           // graphics_app:
                RenderPass::Access::None,               // - screen_pass_access
                false,                                  // - animations_enabled
                false,                                  // - show_logo_badge
            },                                          //
            {                                           // render_context:
                FrameSize(),                            // - frame_size placeholder: set in InitContext
                PixelFormat::BGRA8Unorm,                // - color_format
                PixelFormat::Unknown,                   // - depth_stencil_format
                Color4f(0.0f, 0.2f, 0.4f, 1.0f),        // - clear_color
            }
        })
    { }

    ~HelloTriangleApp() override
    {
        GetRenderContext().WaitForGpu(Context::WaitFor::RenderComplete);
    }

    void Init() override
    {
        GraphicsApp::Init();

        struct Vertex { Vector3f position; Vector3f color; };
        const std::array<Vertex, 3> triangle_vertices{ {
            { { 0.0f,   0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5f,  -0.5f,  0.0f }, { 0.0f, 1.0f, 0.0f } },
            { { -0.5f, -0.5f,  0.0f }, { 0.0f, 0.0f, 1.0f } },
        } };

        const Data::Size vertex_buffer_size = static_cast<Data::Size>(sizeof(triangle_vertices));
        Ptr<Buffer> sp_vertex_buffer = Buffer::CreateVertexBuffer(GetRenderContext(), vertex_buffer_size, static_cast<Data::Size>(sizeof(Vertex)));
        sp_vertex_buffer->SetData(
            Resource::SubResources
            {
                Resource::SubResource { reinterpret_cast<Data::ConstRawPtr>(triangle_vertices.data()), vertex_buffer_size }
            }
        );
        m_sp_vertex_buffers = BufferSet::CreateVertexBuffers({ *sp_vertex_buffer });

        m_sp_state = RenderState::Create(GetRenderContext(),
            RenderState::Settings
            {
                Program::Create(GetRenderContext(),
                    Program::Settings
                    {
                        Program::Shaders
                        {
                            Shader::CreateVertex(GetRenderContext(), { Data::ShaderProvider::Get(), { "Triangle", "TriangleVS" } }),
                            Shader::CreatePixel(GetRenderContext(),  { Data::ShaderProvider::Get(), { "Triangle", "TrianglePS" } }),
                        },
                        Program::InputBufferLayouts
                        {
                            Program::InputBufferLayout
                            {
                                Program::InputBufferLayout::ArgumentSemantics { "POSITION", "COLOR" },
                            }
                        },
                        Program::ArgumentDescriptions { },
                        PixelFormats { GetRenderContext().GetSettings().color_format }
                    }
                ),
                Viewports    { GetFrameViewport(GetRenderContext().GetSettings().frame_size)    },
                ScissorRects { GetFrameScissorRect(GetRenderContext().GetSettings().frame_size) },
            }
        );

        for (HelloTriangleFrame& frame : GetFrames())
        {
            frame.sp_render_cmd_list  = RenderCommandList::Create(GetRenderContext().GetRenderCommandQueue(), *frame.sp_screen_pass);
            frame.sp_render_cmd_lists = CommandListSet::Create({ *frame.sp_render_cmd_list });
        }

        GraphicsApp::CompleteInitialization();
    }

    bool Resize(const FrameSize& frame_size, bool is_minimized) override
    {
        if (!GraphicsApp::Resize(frame_size, is_minimized))
            return false;

        m_sp_state->SetViewports(    { GetFrameViewport(frame_size)    } );
        m_sp_state->SetScissorRects( { GetFrameScissorRect(frame_size) } );
        return true;
    }

    bool Render() override
    {
        if (!GetRenderContext().ReadyToRender() || !GraphicsApp::Render())
            return false;

        HelloTriangleFrame& frame = GetCurrentFrame();
        frame.sp_render_cmd_list->Reset(m_sp_state);
        frame.sp_render_cmd_list->SetVertexBuffers(*m_sp_vertex_buffers);
        frame.sp_render_cmd_list->Draw(RenderCommandList::Primitive::Triangle, 3);
        frame.sp_render_cmd_list->Commit();

        GetRenderContext().GetRenderCommandQueue().Execute(*frame.sp_render_cmd_lists);
        GetRenderContext().Present();

        return true;
    }

    void OnContextReleased(Context& context) override
    {
        m_sp_vertex_buffers.reset();
        m_sp_state.reset();

        GraphicsApp::OnContextReleased(context);
    }
};

int main(int argc, const char* argv[])
{
    return HelloTriangleApp().Run({ argc, argv });
}
