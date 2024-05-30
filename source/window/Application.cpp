/**
 * \file
 * \author Rudy Castan
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "Application.hpp"

#include "ImGuiHelper.hpp"
#include "Logo.hpp"
#include "assets/Path.hpp"
#include "environment/Environment.hpp"
#include "environment/Input.hpp"
#include "environment/OpenGL.hpp"
#include "opengl/GL.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <sstream>

namespace
{
    void hint_gl(SDL_GLattr attr, int value)
    {
        // https://wiki.libsdl.org/SDL_GL_SetAttribute
        if (const auto success = SDL_GL_SetAttribute(attr, value); success != 0)
        {
            std::cerr << "Failed to Set GL Attribute: " << SDL_GetError() << '\n';
        }
    }

    template <typename... Messages>
    [[noreturn]] void throw_error_message(Messages&&... more_messages)
    {
        std::ostringstream sout;
        (sout << ... << more_messages);
        std::cerr << sout.str() << '\n';
        throw std::runtime_error{ sout.str() };
    }

    std::string format_folder_name(const std::string& input);

    environment::input::KeyboardButtons sdl_scancode_to_button(SDL_Scancode scancode) noexcept;
}

namespace window
{

    Application::Application(gsl::czstring title, demos::Demos starting_demo)
    {
        if (title == nullptr || title[0] == '\0')
            throw_error_message("App title shouldn't be empty");
        getAndSetWritableDirectory(title);
        setupSDLWindow(title);
        setupOpenGL();
        setupWindowSizeAndDPI();
        setupImGui();
        if (starting_demo != demos::Demos::None)
        {
            settings.CurrentDemo = starting_demo;
        }
        ptr_program     = demos::create_demo(settings.CurrentDemo);
        currentViewport = { 0, 0, environment::DisplayWidth, environment::DisplayHeight };
        timer.ResetTimeStamp();
    }

    Application::~Application()
    {
        delete ptr_program;
        ImGuiHelper::Shutdown();
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(ptr_window);
        SDL_Quit();
    }

    void Application::Update()
    {
        updateEnvironment();
        updateWindowEvents();
        updateDisplayViewport();
        ptr_program->Update();
        ptr_program->Draw();
        currentViewport = ImGuiHelper::Begin();
        imguiDraw();
        ImGuiHelper::End(ptr_window, gl_context);
        SDL_GL_SwapWindow(ptr_window);
    }

    bool Application::IsDone() const noexcept
    {
        return is_done;
    }

    void Application::ForceResize(int desired_width, int desired_height) const
    {
        SDL_SetWindowSize(ptr_window, desired_width, desired_height);
    }

    void Application::imguiDraw()
    {
        if (ImGui::BeginMainMenuBar())
        {
            imguiSelectDemo();
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Demo Settings", "", &settings.ShowDemoSettings);
                ImGui::MenuItem("FPS", "", &settings.ShowFPS);
                ImGui::MenuItem("Window Information", "", &settings.ShowWindowInformation);
                ImGui::MenuItem("Mouse Information", "", &settings.ShowMouseInformation);
                ImGui::MenuItem("Keyboard Information", "", &settings.ShowKeyboardInformation);
                ImGui::MenuItem("OpenGL Information", "", &settings.ShowOpenGLInformation);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (settings.ShowFPS)
        {
            ImGui::Begin("FPS", &settings.ShowFPS);
            ImGui::Text("%d", environment::FPS);
            ImGui::End();
        }

        if (settings.ShowWindowInformation)
        {
            ImGui::Begin("Window Information", &settings.ShowWindowInformation);
            ImGui::Text("Window Size (%d, %d)", environment::WindowWidth, environment::WindowHeight);
            ImGui::Text("Display Size (%d, %d)", environment::DisplayWidth, environment::DisplayHeight);
            ImGui::Text("DPI Scale (%g, %g)", environment::HorizontalDPIScale, environment::VerticalDPIScale);
            ImGui::Text("GL Viewport x: %d y: %d", currentViewport.x, currentViewport.y);
            ImGui::Text("GL Viewport w: %d h: %d", currentViewport.width, currentViewport.height);
            ImGui::End();
        }

        if (settings.ShowMouseInformation)
        {
            using namespace std::string_literals;
            ImGui::Begin("Mouse Information", &settings.ShowMouseInformation);
            ImGui::Text("Mouse Window Position (%g, %g)", environment::input::MouseWindowX, environment::input::MouseWindowY);
            ImGui::Text("Mouse Display Position (%g, %g)", environment::input::MouseDisplayX, environment::input::MouseDisplayY);
            ImGui::Text("Last Mouse Velocity (%g, %g)", lastMouseVelocityX, lastMouseVelocityY);
            ImGui::Text("Last Mouse Wheel %g", lastMouseWheel);
            std::string collection = "{";
            for (const auto button : environment::input::PressedMouseButtons)
            {
                collection += environment::input::to_string(button) + ", "s;
            }
            collection += "}";
            ImGui::Text("Pressed %s", collection.c_str());
            collection = "{";
            for (const auto button : lastReleasedMouseButtons)
            {
                collection += environment::input::to_string(button) + ", "s;
            }
            collection += "}";
            ImGui::Text("Last Released %s", collection.c_str());
            ImGui::End();
        }

        if (settings.ShowKeyboardInformation)
        {
            using namespace std::string_literals;
            ImGui::Begin("Keyboard Information", &settings.ShowKeyboardInformation);
            std::string collection = "{";
            for (const auto button : environment::input::PressedKeyboardButtons)
            {
                collection += environment::input::to_string(button) + ", "s;
            }
            collection += "}";
            ImGui::Text("Pressed %s", collection.c_str());
            collection = "{";
            for (const auto button : lastReleasedKeyboardButtons)
            {
                collection += environment::input::to_string(button) + ", "s;
            }
            collection += "}";
            ImGui::Text("Last Released %s", collection.c_str());
            ImGui::End();
        }

        if (settings.ShowOpenGLInformation)
        {
            ImGui::Begin("OpenGL Information", &settings.ShowOpenGLInformation);
            ImGui::Text("Vendor %s", openglStrings.Vendor.c_str());
            ImGui::Text("Renderer %s", openglStrings.Renderer.c_str());
            ImGui::Text("Version %s", openglStrings.Version.c_str());
            ImGui::Text("GLSLVersion %s", openglStrings.GLSLVersion.c_str());
            ImGui::Text("Is OpenGL ES %s", environment::opengl::IsOpenGL_ES ? "true" : "false");
            ImGui::Text("MaxElementVertices %d", environment::opengl::MaxElementVertices);
            ImGui::Text("MaxElementIndices %d", environment::opengl::MaxElementIndices);
            ImGui::Text("MaxTextureImageUnits %d", environment::opengl::MaxTextureImageUnits);
            ImGui::Text("MaxTextureSize %d", environment::opengl::MaxTextureSize);
            ImGui::End();
        }

        if (settings.ShowDemoSettings)
        {
            ImGui::Begin("Demo Settings", &settings.ShowDemoSettings);
            ptr_program->ImGuiDraw();
            ImGui::End();
        }
    }

    void Application::getAndSetWritableDirectory(gsl::czstring title)
    {
        const auto sdl_path = SDL_GetPrefPath("digipen.student.edu", format_folder_name(title).c_str());
        if (sdl_path)
        {
            writableDirectory = sdl_path;
            SDL_free(sdl_path);
        }
#if defined(DEVELOPER_VERSION)
        std::cout << "Writable Directory : " << writableDirectory << '\n';
#endif
    }

    void Application::setupSDLWindow(gsl::czstring title)
    {
        // https://wiki.libsdl.org/SDL_Init
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw_error_message("Failed to init SDK error: ", SDL_GetError());
        }

        settings = window::load_settings(writableDirectory / window::Settings::FileName);
        setStartingWindowPlacement();


#if defined(OPENGL_ES3_ONLY)
        hint_gl(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        hint_gl(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        hint_gl(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        environment::opengl::IsOpenGL_ES = true;
// else Desktop will pick the highest OpenGL version by default
#else
        hint_gl(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        environment::opengl::IsOpenGL_ES = false;
#endif
        hint_gl(SDL_GL_DOUBLEBUFFER, true);
        hint_gl(SDL_GL_STENCIL_SIZE, 8);
        hint_gl(SDL_GL_DEPTH_SIZE, 24);
        hint_gl(SDL_GL_RED_SIZE, 8);
        hint_gl(SDL_GL_GREEN_SIZE, 8);
        hint_gl(SDL_GL_BLUE_SIZE, 8);
        hint_gl(SDL_GL_ALPHA_SIZE, 8);
        hint_gl(SDL_GL_MULTISAMPLEBUFFERS, 1);
        hint_gl(SDL_GL_MULTISAMPLESAMPLES, 4);


        // https://wiki.libsdl.org/SDL_CreateWindow
        ptr_window = SDL_CreateWindow(
            title, settings.Startup.PositionX, settings.Startup.PositionY, settings.Startup.Width, settings.Startup.Height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        if (ptr_window == nullptr)
        {
            throw_error_message("Failed to create window: ", SDL_GetError());
        }

        set_sdl_icon(ptr_window);
    }

    void Application::setStartingWindowPlacement()
    {
        SDL_Rect  display_bounds;
        const int num_displays = SDL_GetNumVideoDisplays();
        for (int i = 0; i < num_displays; ++i)
        {
            SDL_GetDisplayBounds(i, &display_bounds);
            if (settings.Startup.PositionX >= display_bounds.x && settings.Startup.PositionX + settings.Startup.Width <= display_bounds.x + display_bounds.w &&
                settings.Startup.PositionY >= display_bounds.y && settings.Startup.PositionY + settings.Startup.Height <= display_bounds.y + display_bounds.h)
            {
                // saved settings are still valid
                return;
            }
        }

        // reset to main display
        SDL_GetDisplayBounds(0, &display_bounds);
        settings.Startup.PositionX = SDL_WINDOWPOS_UNDEFINED;
        settings.Startup.PositionY = SDL_WINDOWPOS_UNDEFINED;
        if (settings.Startup.Width > display_bounds.w || settings.Startup.Height > display_bounds.h)
        {
            settings.Startup.Width         = window::Settings::DEFAULT_WIDTH;
            settings.Startup.Height        = window::Settings::DEFAULT_HEIGHT;
            settings.Startup.ResetImGuiIni = true;
        }
    }

    void Application::setupOpenGL()
    {
        // https://wiki.libsdl.org/SDL_GL_CreateContext
        if (gl_context = SDL_GL_CreateContext(ptr_window); gl_context == nullptr)
        {
            throw_error_message("Failed to create opengl context: ", SDL_GetError());
        }

        // https://wiki.libsdl.org/SDL_GL_MakeCurrent
        SDL_GL_MakeCurrent(ptr_window, gl_context);

        // http://glew.sourceforge.net/basic.html
        if (const auto result = glewInit(); GLEW_OK != result)
        {
            throw_error_message("Unable to initialize GLEW - error: ", glewGetErrorString(result));
        }

        // https://wiki.libsdl.org/SDL_GL_SetSwapInterval
        constexpr int ADAPTIVE_VSYNC = -1;
        constexpr int VSYNC          = 1;
        if (const auto result = SDL_GL_SetSwapInterval(ADAPTIVE_VSYNC); result != 0)
        {
            SDL_GL_SetSwapInterval(VSYNC);
        }

        getOpenGLSettings();
    }

    void Application::getOpenGLSettings()
    {
        openglStrings.Vendor      = reinterpret_cast<const char*>(GL::GetString(GL_VENDOR));
        openglStrings.Renderer    = reinterpret_cast<const char*>(GL::GetString(GL_RENDERER));
        openglStrings.Version     = reinterpret_cast<const char*>(GL::GetString(GL_VERSION));
        openglStrings.GLSLVersion = reinterpret_cast<const char*>(GL::GetString(GL_SHADING_LANGUAGE_VERSION));
        GL::GetIntegerv(GL_MAJOR_VERSION, &environment::opengl::MajorVersion);
        GL::GetIntegerv(GL_MINOR_VERSION, &environment::opengl::MinorVersion);
        GL::GetIntegerv(GL_MAX_ELEMENTS_VERTICES, &environment::opengl::MaxElementVertices);
        GL::GetIntegerv(GL_MAX_ELEMENTS_INDICES, &environment::opengl::MaxElementIndices);
        GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &environment::opengl::MaxTextureImageUnits);
        GL::GetIntegerv(GL_MAX_TEXTURE_SIZE, &environment::opengl::MaxTextureSize);
    }

    void Application::setupWindowSizeAndDPI() const
    {
        SDL_GetWindowSize(ptr_window, &environment::WindowWidth, &environment::WindowHeight);
        SDL_GL_GetDrawableSize(ptr_window, &environment::DisplayWidth, &environment::DisplayHeight);
        // handle DPI scaling
        if (environment::WindowWidth != environment::DisplayWidth || environment::WindowHeight != environment::DisplayHeight)
        {
            environment::HorizontalDPIScale = static_cast<double>(environment::DisplayWidth) / static_cast<double>(environment::WindowWidth);
            environment::VerticalDPIScale   = static_cast<double>(environment::DisplayHeight) / static_cast<double>(environment::WindowHeight);
        }
        else
        {
            environment::HorizontalDPIScale = 1.0;
            environment::VerticalDPIScale   = 1.0;
        }
    }

    void Application::setupImGui()
    {
        const auto imgui_ini_location = writableDirectory / "imgui.ini";
        if (settings.Startup.ResetImGuiIni || !std::filesystem::exists(imgui_ini_location))
        {
            std::filesystem::copy_file(assets::get_base_path() / "imgui_starter.ini", imgui_ini_location, std::filesystem::copy_options::overwrite_existing);
            settings.Startup.ResetImGuiIni = false;
        }
        ImGuiHelper::Initialize(ptr_window, gl_context, imgui_ini_location);
    }

    void Application::updateEnvironment()
    {
        environment::DeltaTime = timer.GetElapsedSeconds();
        timer.ResetTimeStamp();
        environment::ElapsedTime += environment::DeltaTime;
        ++environment::FrameCount;
        fps.Update(environment::DeltaTime);
        environment::FPS = fps;
    }

    void Application::updateWindowEvents()
    {
        if (environment::input::MouseWheel != 0)
        {
            lastMouseWheel = environment::input::MouseWheel;
        }
        if (environment::input::MouseVelocityX != 0)
        {
            lastMouseVelocityX = environment::input::MouseVelocityX;
        }
        if (environment::input::MouseVelocityY != 0)
        {
            lastMouseVelocityY = environment::input::MouseVelocityY;
        }

        if (!releasedMouseButtons.empty())
        {
            lastReleasedMouseButtons = releasedMouseButtons;
        }

        if (!releasedKeyboardButtons.empty())
        {
            lastReleasedKeyboardButtons = releasedKeyboardButtons;
        }

        environment::input::MouseWheel     = 0;
        environment::input::MouseVelocityX = 0;
        environment::input::MouseVelocityY = 0;

        releasedMouseButtons.clear();
        releasedKeyboardButtons.clear();

        SDL_Event event{ 0 };
        while (SDL_PollEvent(&event) != 0)
        {
            ImGuiHelper::FeedEvent(event);
            switch (event.type)
            {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    {
                        const SDL_Scancode scan_code = event.key.keysym.scancode;
                        const auto         button    = sdl_scancode_to_button(scan_code);
                        if (event.key.state == SDL_PRESSED)
                        {
                            if (const auto iter = std::find(std::begin(pressedKeyboardButtons), std::end(pressedKeyboardButtons), button); iter == std::end(pressedKeyboardButtons))
                            {
                                pressedKeyboardButtons.push_back(button);
                            }
                        }
                        else
                        {
                            releasedKeyboardButtons.push_back(button);
                            pressedKeyboardButtons.erase(std::remove(std::begin(pressedKeyboardButtons), std::end(pressedKeyboardButtons), button), std::end(pressedKeyboardButtons));
                        }
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    environment::input::MouseWheel = static_cast<double>(event.wheel.preciseY);
                    break;
                case SDL_MOUSEMOTION:
                    if (event.motion.windowID == SDL_GetWindowID(ptr_window))
                    {
                        environment::input::MouseWindowX   = static_cast<double>(event.motion.x);
                        environment::input::MouseWindowY   = static_cast<double>(event.motion.y);
                        environment::input::MouseVelocityX = static_cast<double>(event.motion.xrel);
                        environment::input::MouseVelocityY = static_cast<double>(event.motion.yrel);
                        environment::input::MouseDisplayX  = environment::input::MouseWindowX * environment::HorizontalDPIScale - static_cast<double>(currentViewport.x);
                        environment::input::MouseDisplayY  = (environment::WindowHeight - environment::input::MouseWindowY) * environment::VerticalDPIScale - static_cast<double>(currentViewport.y);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    {
                        environment::input::MouseButtons button = environment::input::MouseButtons::None;
                        switch (event.button.button)
                        {
                            case SDL_BUTTON_LEFT: button = environment::input::MouseButtons::Left; break;
                            case SDL_BUTTON_MIDDLE: button = environment::input::MouseButtons::Middle; break;
                            case SDL_BUTTON_RIGHT: button = environment::input::MouseButtons::Right; break;
                        }

                        if (event.button.state == SDL_PRESSED)
                        {
                            if (const auto iter = std::find(std::begin(pressedMouseButtons), std::end(pressedMouseButtons), button); iter == std::end(pressedMouseButtons))
                            {
                                pressedMouseButtons.push_back(button);
                            }
                        }
                        else // SDL_RELEASED
                        {
                            releasedMouseButtons.push_back(button);
                            pressedMouseButtons.erase(std::remove(std::begin(pressedMouseButtons), std::end(pressedMouseButtons), button), std::end(pressedMouseButtons));
                        }
                    }
                    break;
                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                            {
                                is_done = true;
                            }
                            break;
                        case SDL_WINDOWEVENT_RESIZED:
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            {
                                environment::WindowWidth   = event.window.data1;
                                environment::WindowHeight  = event.window.data2;
                                environment::DisplayWidth  = static_cast<int>(static_cast<double>(environment::WindowWidth) * environment::HorizontalDPIScale);
                                environment::DisplayHeight = static_cast<int>(static_cast<double>(environment::WindowHeight) * environment::VerticalDPIScale);
                            }
                            break;
                    }
                    break;
                case SDL_QUIT:
                    [[unlikely]] is_done = true;
                    SDL_GetWindowPosition(ptr_window, &settings.Startup.PositionX, &settings.Startup.PositionY);
                    settings.Startup.Width  = environment::WindowWidth;
                    settings.Startup.Height = environment::WindowHeight;
                    window::save_settings(settings, writableDirectory / window::Settings::FileName);
                    break;
            }
        }

        environment::input::PressedMouseButtons     = std::span{ pressedMouseButtons };
        environment::input::ReleasedMouseButtons    = std::span{ releasedMouseButtons };
        environment::input::PressedKeyboardButtons  = std::span{ pressedKeyboardButtons };
        environment::input::ReleasedKeyboardButtons = std::span{ releasedKeyboardButtons };
    }

    void Application::updateDisplayViewport()
    {
        if (currentViewport != lastViewport)
        {
            lastViewport = currentViewport;
            GL::Viewport(currentViewport.x, currentViewport.y, currentViewport.width, currentViewport.height);
            environment::DisplayWidth  = currentViewport.width;
            environment::DisplayHeight = currentViewport.height;
            ptr_program->SetDisplaySize(currentViewport.width, currentViewport.height);
        }
    }

    void Application::imguiSelectDemo()
    {
        using namespace demos;
        Demos selected_demo = settings.CurrentDemo;
        if (ImGui::BeginMenu("Select Demo"))
        {
            struct NameType
            {
                const char* name{ nullptr };
                Demos       demo{ demos::Demos::HelloQuad };
            };

            std::vector supported_demos = 
            {
                NameType{       "Hello Quad",        Demos::HelloQuad},
                NameType{"Procedural Meshes",        Demos::ProceduralMeshes},
                NameType{              "Fog",        Demos::Fog},
                NameType{     "Toon Shading",        Demos::ToonShading},
                NameType{   "Shadow Mapping",        Demos::ShadowMapping},
                NameType{      "Value Noise",        Demos::ValueNoise},
                NameType{   "Gradient Noise",        Demos::GradientNoise},
                NameType{  "Curves N Splines",       Demos::CurvesNSplines}
            };

            if (!(environment::opengl::IsOpenGL_ES))
            {
                IF_CAN_DO_OPENGL(3, 2)
                {
                    supported_demos.push_back(NameType{ "Geometry Shaders", Demos::GeometryShaders });
                    supported_demos.push_back(NameType{ "Tessellation Shaders", Demos::TessellationShaders });
                    supported_demos.push_back(NameType{ "Compute Shaders", Demos::ComputeShaders });
                }
            }

            for (const auto& [name, demo] : supported_demos)
            {
                if (ImGui::MenuItem(name, "", settings.CurrentDemo == demo))
                {
                    selected_demo = demo;
                }
            }

            ImGui::EndMenu();
        }

        if (selected_demo != settings.CurrentDemo)
        {
            settings.CurrentDemo = selected_demo;
            delete ptr_program;
            ptr_program = create_demo(selected_demo);
        }
    }
}

namespace
{
    std::string format_folder_name(const std::string& input)
    {
        // Remove punctuation, symbols, and special characters
        std::string formatted = "";
        for (char c : input)
        {
            if (std::isalnum(c))
            {
                formatted += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            else if (c == ' ')
            {
                // Replace spaces with hyphens
                formatted += '-';
            }
        }

        // Remove consecutive hyphens
        formatted.erase(std::unique(formatted.begin(), formatted.end(), [](char a, char b)
                                    { return a == '-' && b == '-'; }),
                        formatted.end());

        return formatted;
    }

    environment::input::KeyboardButtons sdl_scancode_to_button(SDL_Scancode scancode) noexcept
    {
        switch (scancode)
        {
            case SDL_SCANCODE_A: return environment::input::KeyboardButtons::A;
            case SDL_SCANCODE_B: return environment::input::KeyboardButtons::B;
            case SDL_SCANCODE_C: return environment::input::KeyboardButtons::C;
            case SDL_SCANCODE_D: return environment::input::KeyboardButtons::D;
            case SDL_SCANCODE_E: return environment::input::KeyboardButtons::E;
            case SDL_SCANCODE_F: return environment::input::KeyboardButtons::F;
            case SDL_SCANCODE_G: return environment::input::KeyboardButtons::G;
            case SDL_SCANCODE_H: return environment::input::KeyboardButtons::H;
            case SDL_SCANCODE_I: return environment::input::KeyboardButtons::I;
            case SDL_SCANCODE_J: return environment::input::KeyboardButtons::J;
            case SDL_SCANCODE_K: return environment::input::KeyboardButtons::K;
            case SDL_SCANCODE_L: return environment::input::KeyboardButtons::L;
            case SDL_SCANCODE_M: return environment::input::KeyboardButtons::M;
            case SDL_SCANCODE_N: return environment::input::KeyboardButtons::N;
            case SDL_SCANCODE_O: return environment::input::KeyboardButtons::O;
            case SDL_SCANCODE_P: return environment::input::KeyboardButtons::P;
            case SDL_SCANCODE_Q: return environment::input::KeyboardButtons::Q;
            case SDL_SCANCODE_R: return environment::input::KeyboardButtons::R;
            case SDL_SCANCODE_S: return environment::input::KeyboardButtons::S;
            case SDL_SCANCODE_T: return environment::input::KeyboardButtons::T;
            case SDL_SCANCODE_U: return environment::input::KeyboardButtons::U;
            case SDL_SCANCODE_V: return environment::input::KeyboardButtons::V;
            case SDL_SCANCODE_W: return environment::input::KeyboardButtons::W;
            case SDL_SCANCODE_X: return environment::input::KeyboardButtons::X;
            case SDL_SCANCODE_Y: return environment::input::KeyboardButtons::Y;
            case SDL_SCANCODE_Z: return environment::input::KeyboardButtons::Z;
            case SDL_SCANCODE_1: return environment::input::KeyboardButtons::_1;
            case SDL_SCANCODE_2: return environment::input::KeyboardButtons::_2;
            case SDL_SCANCODE_3: return environment::input::KeyboardButtons::_3;
            case SDL_SCANCODE_4: return environment::input::KeyboardButtons::_4;
            case SDL_SCANCODE_5: return environment::input::KeyboardButtons::_5;
            case SDL_SCANCODE_6: return environment::input::KeyboardButtons::_6;
            case SDL_SCANCODE_7: return environment::input::KeyboardButtons::_7;
            case SDL_SCANCODE_8: return environment::input::KeyboardButtons::_8;
            case SDL_SCANCODE_9: return environment::input::KeyboardButtons::_9;
            case SDL_SCANCODE_0: return environment::input::KeyboardButtons::_0;
            case SDL_SCANCODE_ESCAPE: return environment::input::KeyboardButtons::Escape;
            case SDL_SCANCODE_LCTRL:
            case SDL_SCANCODE_RCTRL: return environment::input::KeyboardButtons::Control;
            case SDL_SCANCODE_LGUI: return environment::input::KeyboardButtons::LeftSystem;
            case SDL_SCANCODE_RGUI: return environment::input::KeyboardButtons::RightSystem;
            case SDL_SCANCODE_MENU: return environment::input::KeyboardButtons::Menu;
            case SDL_SCANCODE_LEFTBRACKET: return environment::input::KeyboardButtons::LeftBracket;
            case SDL_SCANCODE_RIGHTBRACKET: return environment::input::KeyboardButtons::RightBracket;
            case SDL_SCANCODE_SEMICOLON: return environment::input::KeyboardButtons::Semicolon;
            case SDL_SCANCODE_COMMA: return environment::input::KeyboardButtons::Comma;
            case SDL_SCANCODE_PERIOD: return environment::input::KeyboardButtons::Period;
            case SDL_SCANCODE_APOSTROPHE: return environment::input::KeyboardButtons::Quote;
            case SDL_SCANCODE_SLASH: return environment::input::KeyboardButtons::Slash;
            case SDL_SCANCODE_BACKSLASH: return environment::input::KeyboardButtons::Backslash;
            case SDL_SCANCODE_GRAVE: return environment::input::KeyboardButtons::Tilde;
            case SDL_SCANCODE_EQUALS: return environment::input::KeyboardButtons::Equal;
            case SDL_SCANCODE_MINUS: return environment::input::KeyboardButtons::Hyphen;
            case SDL_SCANCODE_SPACE: return environment::input::KeyboardButtons::Space;
            case SDL_SCANCODE_RETURN: return environment::input::KeyboardButtons::Enter;
            case SDL_SCANCODE_BACKSPACE: return environment::input::KeyboardButtons::Backspace;
            case SDL_SCANCODE_TAB: return environment::input::KeyboardButtons::Tab;
            case SDL_SCANCODE_PAGEUP: return environment::input::KeyboardButtons::PageUp;
            case SDL_SCANCODE_PAGEDOWN: return environment::input::KeyboardButtons::PageDown;
            case SDL_SCANCODE_END: return environment::input::KeyboardButtons::End;
            case SDL_SCANCODE_HOME: return environment::input::KeyboardButtons::Home;
            case SDL_SCANCODE_PRINTSCREEN: return environment::input::KeyboardButtons::PrintScreen;
            case SDL_SCANCODE_INSERT: return environment::input::KeyboardButtons::Insert;
            case SDL_SCANCODE_DELETE: return environment::input::KeyboardButtons::Delete;
            case SDL_SCANCODE_KP_PLUS: return environment::input::KeyboardButtons::Add;
            case SDL_SCANCODE_KP_MINUS: return environment::input::KeyboardButtons::Subtract;
            case SDL_SCANCODE_KP_MULTIPLY: return environment::input::KeyboardButtons::Multiply;
            case SDL_SCANCODE_KP_DIVIDE: return environment::input::KeyboardButtons::Divide;
            case SDL_SCANCODE_RIGHT: return environment::input::KeyboardButtons::Right;
            case SDL_SCANCODE_LEFT: return environment::input::KeyboardButtons::Left;
            case SDL_SCANCODE_DOWN: return environment::input::KeyboardButtons::Down;
            case SDL_SCANCODE_UP: return environment::input::KeyboardButtons::Up;
            case SDL_SCANCODE_F1: return environment::input::KeyboardButtons::F1;
            case SDL_SCANCODE_F2: return environment::input::KeyboardButtons::F2;
            case SDL_SCANCODE_F3: return environment::input::KeyboardButtons::F3;
            case SDL_SCANCODE_F4: return environment::input::KeyboardButtons::F4;
            case SDL_SCANCODE_F5: return environment::input::KeyboardButtons::F5;
            case SDL_SCANCODE_F6: return environment::input::KeyboardButtons::F6;
            case SDL_SCANCODE_F7: return environment::input::KeyboardButtons::F7;
            case SDL_SCANCODE_F8: return environment::input::KeyboardButtons::F8;
            case SDL_SCANCODE_F9: return environment::input::KeyboardButtons::F9;
            case SDL_SCANCODE_KP_1: return environment::input::KeyboardButtons::NumPad_1;
            case SDL_SCANCODE_KP_2: return environment::input::KeyboardButtons::NumPad_2;
            case SDL_SCANCODE_KP_3: return environment::input::KeyboardButtons::NumPad_3;
            case SDL_SCANCODE_KP_4: return environment::input::KeyboardButtons::NumPad_4;
            case SDL_SCANCODE_KP_5: return environment::input::KeyboardButtons::NumPad_5;
            case SDL_SCANCODE_KP_6: return environment::input::KeyboardButtons::NumPad_6;
            case SDL_SCANCODE_KP_7: return environment::input::KeyboardButtons::NumPad_7;
            case SDL_SCANCODE_KP_8: return environment::input::KeyboardButtons::NumPad_8;
            case SDL_SCANCODE_KP_9: return environment::input::KeyboardButtons::NumPad_9;
            case SDL_SCANCODE_KP_0: return environment::input::KeyboardButtons::NumPad_0;
            case SDL_SCANCODE_PAUSE: return environment::input::KeyboardButtons::Pause;
            default: return environment::input::KeyboardButtons::None;
        }
    }
}
