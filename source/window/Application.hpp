/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "ImGuiHelper.hpp"
#include "Settings.hpp"
#include "demos/DemosFactory.hpp"
#include "util/FPS.hpp"
#include "util/Timer.hpp"
#include <filesystem>
#include <gsl/gsl>
#include <vector>

struct SDL_Window;
typedef void* SDL_GLContext;

namespace environment::input
{
    enum class MouseButtons;
    enum class KeyboardButtons;
}

namespace window
{
    class [[nodiscard]] Application
    {
    public:
        Application(gsl::czstring title = "OpenGL App", demos::Demos starting_demo = demos::Demos::HelloQuad);
        ~Application();

        Application(const Application&)                = delete;
        Application& operator=(const Application&)     = delete;
        Application(Application&&) noexcept            = delete;
        Application& operator=(Application&&) noexcept = delete;

        void Update();
        bool IsDone() const noexcept;

        void ForceResize(int desired_width, int desired_height) const;

    private:
        void imguiDraw();
        void getAndSetWritableDirectory(gsl::czstring title);
        void setupSDLWindow(gsl::czstring title);
        void setStartingWindowPlacement();
        void setupOpenGL();
        void getOpenGLSettings();
        void setupWindowSizeAndDPI() const;
        void setupImGui();
        void updateEnvironment();
        void updateWindowEvents();
        void updateDisplayViewport();
        void imguiSelectDemo();

    private:
        using MouseButton    = environment::input::MouseButtons;
        using KeyboardButton = environment::input::KeyboardButtons;
        util::FPS                   fps{};
        util::Timer                 timer{};
        gsl::owner<demos::IDemo*>   ptr_program = nullptr;
        gsl::owner<SDL_Window*>     ptr_window  = nullptr;
        gsl::owner<SDL_GLContext>   gl_context  = nullptr;
        bool                        is_done     = false;
        std::filesystem::path       writableDirectory;
        ImGuiHelper::Viewport       lastViewport{ 0, 0, 0, 0 };
        ImGuiHelper::Viewport       currentViewport;
        std::vector<MouseButton>    pressedMouseButtons;
        std::vector<MouseButton>    releasedMouseButtons;
        std::vector<MouseButton>    lastReleasedMouseButtons;
        std::vector<KeyboardButton> pressedKeyboardButtons;
        std::vector<KeyboardButton> releasedKeyboardButtons;
        std::vector<KeyboardButton> lastReleasedKeyboardButtons;
        double                      lastMouseVelocityX = 0;
        double                      lastMouseVelocityY = 0;
        double                      lastMouseWheel     = 0;
        window::Settings            settings;

        struct
        {
            std::string Vendor;
            std::string Renderer;
            std::string Version;
            std::string GLSLVersion;
        } openglStrings;
    };
}
