// dear imgui: standalone example application for GLFW + OpenGL 3, using
// programmable pipeline If you are new to dear imgui, see examples/README.txt
// and documentation at the top of imgui.cpp. (GLFW is a cross-platform general
// purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics
// context creation, etc.)

#define IMGUI_IMPL_OPENGL_LOADER_GLAD 1

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// #include "vecadd_tests_cuda.cuh"
#include "graphics.h"

#include <stdio.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load
//  OpenGL function pointers. Helper libraries are often used for this purpose!
//  Here we are supporting a few common ones (gl3w, glew, glad). You may use
//  another loader/header of your choice (glext, glLoadGen, etc.), or chose to
//  manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>  // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>  // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>  // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE  // GLFW including OpenGL headers causes ambiguity or
                           // multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE  // GLFW including OpenGL headers causes ambiguity or
                           // multiple definition errors.
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>  // Initialize with glbinding::initialize()
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to
// maximize ease of testing and compatibility with old VS compilers. To link
// with VS2010-era libraries, VS2015+ requires linking with
// legacy_stdio_definitions.lib, which we do using this pragma. Your own project
// should not be affected, as you are likely to link with a newer binary of GLFW
// that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
  !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

constexpr int BMASK_LEFT = 0x1 << 0;    // binary 001
constexpr int BMASK_MIDDLE = 0x1 << 1;  // binary 010
constexpr int BMASK_RIGHT = 0x1 << 2;   // binary 100

struct Application_state {
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4{0.45f, 0.55f, 0.60f, 1.00f};

    // Make scene
    std::unique_ptr<opengl_cuda_test::Scene> scene;

    int counter = 0;
    float f = 0.0f;

    int button_mask = 0;
    double last_x = 0.0;
    double last_y = 0.0;
    double mouse_x = 0.0;
    double mouse_y = 0.0;
    int key_mods = 0;
};

static Application_state* g_state = nullptr;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void mouse_button_callback(GLFWwindow* window,
                                  int button,
                                  int action,
                                  int mods) {
    auto const& io = ImGui::GetIO();
    if (!g_state || io.WantCaptureMouse) { return; }

    g_state->last_x = g_state->mouse_x;
    g_state->last_y = g_state->mouse_y;
    glfwGetCursorPos(window, &(g_state->mouse_x), &(g_state->mouse_y));

    // Shift
    if (mods & GLFW_MOD_SHIFT) {
        g_state->key_mods |= GLFW_MOD_SHIFT;
    } else {
        g_state->key_mods &= ~GLFW_MOD_SHIFT;
    }

    // Control
    if (mods & GLFW_MOD_CONTROL) {
        g_state->key_mods |= GLFW_MOD_CONTROL;
    } else {
        g_state->key_mods &= ~GLFW_MOD_CONTROL;
    }

    // Alt
    if (mods & GLFW_MOD_ALT) {
        g_state->key_mods |= GLFW_MOD_ALT;
    } else {
        g_state->key_mods &= ~GLFW_MOD_ALT;
    }

    // Super
    if (mods & GLFW_MOD_SUPER) {
        g_state->key_mods |= GLFW_MOD_SUPER;
    } else {
        g_state->key_mods &= ~GLFW_MOD_SUPER;
    }

    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            g_state->button_mask = g_state->button_mask | BMASK_LEFT;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            g_state->button_mask = g_state->button_mask | BMASK_MIDDLE;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            g_state->button_mask = g_state->button_mask | BMASK_RIGHT;
            break;
        }
    } else {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            g_state->button_mask = g_state->button_mask & ~BMASK_LEFT;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            g_state->button_mask = g_state->button_mask & ~BMASK_MIDDLE;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            g_state->button_mask = g_state->button_mask & ~BMASK_RIGHT;
            break;
        }
    }
}

static void cursor_position_callback(GLFWwindow* window,
                                     double xpos,
                                     double ypos) {
    auto const& io = ImGui::GetIO();
    if (!g_state || io.WantCaptureMouse) { return; }

    g_state->last_x = g_state->mouse_x;
    g_state->last_y = g_state->mouse_y;
    g_state->mouse_x = xpos;
    g_state->mouse_y = ypos;
    auto const dx = g_state->mouse_x - g_state->last_x;
    auto const dy = g_state->mouse_y - g_state->last_y;

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    glm::dvec2 const delta_mouse_pos{dx, dy};
    glm::dvec2 const view_size{static_cast<double>(display_w),
                               static_cast<double>(display_h)};

    if (g_state->key_mods & GLFW_MOD_ALT) {
        if (((g_state->button_mask & BMASK_LEFT) &&
             (g_state->button_mask & BMASK_MIDDLE)) ||
            (g_state->button_mask & BMASK_RIGHT)) {
            g_state->scene->camera =
              dolly(g_state->scene->camera, delta_mouse_pos, view_size);
        } else if (g_state->button_mask & BMASK_LEFT) {
            g_state->scene->camera =
              rotate(g_state->scene->camera, delta_mouse_pos, view_size);
        } else if (g_state->button_mask & BMASK_MIDDLE) {
            g_state->scene->camera =
              track(g_state->scene->camera, delta_mouse_pos, view_size);
        }
    }
    // Some scroll-wheel mice refuse to give middle button signals,
    // so we allow for 'ctrl+left' for tracking.
    else if (g_state->key_mods & GLFW_MOD_CONTROL) {
        if (g_state->button_mask & BMASK_LEFT) {
            g_state->scene->camera =
              track(g_state->scene->camera, delta_mouse_pos, view_size);
        }
    }
    // Just to make mac usage easier... we'll use SHIFT for dolly also.
    else if (g_state->key_mods & GLFW_MOD_SHIFT) {
        if (g_state->button_mask & BMASK_LEFT) {
            // m_camera.dolly( V2d( dx, dy ) );
            // m_camera.autoSetClippingPlanes( m_sim->getBounds() );
            g_state->scene->camera =
              dolly(g_state->scene->camera, delta_mouse_pos, view_size);
        }
    } else {
        // sim mouse drag.
    }
}

static void key_callback(
  GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto const& io = ImGui::GetIO();
    if (!g_state || io.WantCaptureKeyboard) { return; }

    // Shift
    if (mods & GLFW_MOD_SHIFT) {
        g_state->key_mods |= GLFW_MOD_SHIFT;
    } else {
        g_state->key_mods &= ~GLFW_MOD_SHIFT;
    }

    // Control
    if (mods & GLFW_MOD_CONTROL) {
        g_state->key_mods |= GLFW_MOD_CONTROL;
    } else {
        g_state->key_mods &= ~GLFW_MOD_CONTROL;
    }

    // Alt
    if (mods & GLFW_MOD_ALT) {
        g_state->key_mods |= GLFW_MOD_ALT;
    } else {
        g_state->key_mods &= ~GLFW_MOD_ALT;
    }

    // Super
    if (mods & GLFW_MOD_SUPER) {
        g_state->key_mods |= GLFW_MOD_SUPER;
    } else {
        g_state->key_mods &= ~GLFW_MOD_SUPER;
    }

    // std::cout << "Key hit: " << ( int )i_key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }
}

int main(int, char**) {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

        // Decide GL+GLSL versions
#ifdef __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 410";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(
      1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) ==
               0;  // glad2 recommend using the windowing library loader instead
                   // of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) {
        return (glbinding::ProcAddress)glfwGetProcAddress(name);
    });
#else
    bool err =
      false;  // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is
              // likely to requires some form of initialization.
#endif
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
    // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
    // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can
    // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
    // them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
    // need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please
    // handle those errors in your application (e.g. use an assertion, or
    // display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and
    // stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
    // below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string
    // literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font =
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
    // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

    // Our state
    /*bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);*/

    // Do the CUDA thing
    // printf("About to do CUDA\n");
    // do_vecadd_test();

    // Make scene
    auto state = std::make_unique<Application_state>();
    g_state = state.get();
    state->scene =
      std::make_unique<opengl_cuda_test::Scene>(opengl_cuda_test::make_scene());

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
        // tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data
        // to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
        // data to your main application. Generally you may always pass all
        // inputs to dear imgui, and hide them from your application based on
        // those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in
        // ImGui::ShowDemoWindow()! You can browse its code to learn more about
        // Dear ImGui!).
        if (state->show_demo_window) {
            ImGui::ShowDemoWindow(&(state->show_demo_window));
        }

        // 2. Show a simple window that we create ourselves. We use a Begin/End
        // pair to created a named window.
        {
            // static float f = 0.0f;
            // static int counter = 0;

            ImGui::Begin("Hello, world!");  // Create a window called "Hello,
                                            // world!" and append into it.

            ImGui::Text(
              "This is some useful text.");  // Display some text (you can use a
                                             // format strings too)
            ImGui::Checkbox(
              "Demo Window",
              &(state->show_demo_window));  // Edit bools storing our window
                                            // open/close state
            ImGui::Checkbox("Another Window", &(state->show_another_window));

            ImGui::SliderFloat(
              "float",
              &(state->f),
              0.0f,
              1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3(
              "clear color",
              (float*)&(
                state->clear_color));  // Edit 3 floats representing a color

            if (ImGui::Button(
                  "Button")) {  // Buttons return true when clicked (most
                                // widgets return true when edited/activated)
                (state->counter)++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", state->counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (state->show_another_window) {
            ImGui::Begin(
              "Another Window",
              &(state->show_another_window));  // Pass a pointer to our bool
                                               // variable (the window will have
                                               // a closing button that will
                                               // clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me")) state->show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(state->clear_color.x,
                     state->clear_color.y,
                     state->clear_color.z,
                     state->clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        opengl_cuda_test::draw(*(state->scene),
                               glm::dvec2{static_cast<double>(display_w),
                                          static_cast<double>(display_h)});

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    g_state = nullptr;
    state.reset();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
