#include "App/Win32/GltfWindow.h"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace RE {
namespace {
void ErrorCallback(int Error, const char *Description) {
    RE_LOGE("GLFW Error code :", Error, Description);
}

void WindowCloseCallback(GLFWwindow *Window) {
    glfwSetWindowShouldClose(Window, GLFW_TRUE);
}

void WindowSizeCallback(GLFWwindow *Window, int Width, int Height) {
    if(auto Platform = static_cast<FPlatform *>(glfwGetWindowUserPointer(Window))) {
        Platform->Resize(Width, Height);
    }
}

void WindowFocusCallback(GLFWwindow *Window, int bFocused) {
    if(auto Platform = static_cast<FPlatform *>(glfwGetWindowUserPointer(Window))) {
        Platform->SetFocus(bFocused);
    }
}

inline EKeyCode TranslateKeyCode(int Key) {
    static const std::unordered_map<int, EKeyCode> KeyLookup = {
        {GLFW_KEY_SPACE, EKeyCode::Space},
        {GLFW_KEY_APOSTROPHE, EKeyCode::Apostrophe},
        {GLFW_KEY_COMMA, EKeyCode::Comma},
        {GLFW_KEY_MINUS, EKeyCode::Minus},
        {GLFW_KEY_PERIOD, EKeyCode::Period},
        {GLFW_KEY_SLASH, EKeyCode::Slash},
        {GLFW_KEY_0, EKeyCode::_0},
        {GLFW_KEY_1, EKeyCode::_1},
        {GLFW_KEY_2, EKeyCode::_2},
        {GLFW_KEY_3, EKeyCode::_3},
        {GLFW_KEY_4, EKeyCode::_4},
        {GLFW_KEY_5, EKeyCode::_5},
        {GLFW_KEY_6, EKeyCode::_6},
        {GLFW_KEY_7, EKeyCode::_7},
        {GLFW_KEY_8, EKeyCode::_8},
        {GLFW_KEY_9, EKeyCode::_9},
        {GLFW_KEY_SEMICOLON, EKeyCode::Semicolon},
        {GLFW_KEY_EQUAL, EKeyCode::Equal},
        {GLFW_KEY_A, EKeyCode::A},
        {GLFW_KEY_B, EKeyCode::B},
        {GLFW_KEY_C, EKeyCode::C},
        {GLFW_KEY_D, EKeyCode::D},
        {GLFW_KEY_E, EKeyCode::E},
        {GLFW_KEY_F, EKeyCode::F},
        {GLFW_KEY_G, EKeyCode::G},
        {GLFW_KEY_H, EKeyCode::H},
        {GLFW_KEY_I, EKeyCode::I},
        {GLFW_KEY_J, EKeyCode::J},
        {GLFW_KEY_K, EKeyCode::K},
        {GLFW_KEY_L, EKeyCode::L},
        {GLFW_KEY_M, EKeyCode::M},
        {GLFW_KEY_N, EKeyCode::N},
        {GLFW_KEY_O, EKeyCode::O},
        {GLFW_KEY_P, EKeyCode::P},
        {GLFW_KEY_Q, EKeyCode::Q},
        {GLFW_KEY_R, EKeyCode::R},
        {GLFW_KEY_S, EKeyCode::S},
        {GLFW_KEY_T, EKeyCode::T},
        {GLFW_KEY_U, EKeyCode::U},
        {GLFW_KEY_V, EKeyCode::V},
        {GLFW_KEY_W, EKeyCode::W},
        {GLFW_KEY_X, EKeyCode::X},
        {GLFW_KEY_Y, EKeyCode::Y},
        {GLFW_KEY_Z, EKeyCode::Z},
        {GLFW_KEY_LEFT_BRACKET, EKeyCode::LeftBracket},
        {GLFW_KEY_BACKSLASH, EKeyCode::Backslash},
        {GLFW_KEY_RIGHT_BRACKET, EKeyCode::RightBracket},
        {GLFW_KEY_GRAVE_ACCENT, EKeyCode::GraveAccent},
        {GLFW_KEY_ESCAPE, EKeyCode::Escape},
        {GLFW_KEY_ENTER, EKeyCode::Enter},
        {GLFW_KEY_TAB, EKeyCode::Tab},
        {GLFW_KEY_BACKSPACE, EKeyCode::Backspace},
        {GLFW_KEY_INSERT, EKeyCode::Insert},
        {GLFW_KEY_DELETE, EKeyCode::DelKey},
        {GLFW_KEY_RIGHT, EKeyCode::Right},
        {GLFW_KEY_LEFT, EKeyCode::Left},
        {GLFW_KEY_DOWN, EKeyCode::Down},
        {GLFW_KEY_UP, EKeyCode::Up},
        {GLFW_KEY_PAGE_UP, EKeyCode::PageUp},
        {GLFW_KEY_PAGE_DOWN, EKeyCode::PageDown},
        {GLFW_KEY_HOME, EKeyCode::Home},
        {GLFW_KEY_END, EKeyCode::End},
        {GLFW_KEY_CAPS_LOCK, EKeyCode::CapsLock},
        {GLFW_KEY_SCROLL_LOCK, EKeyCode::ScrollLock},
        {GLFW_KEY_NUM_LOCK, EKeyCode::NumLock},
        {GLFW_KEY_PRINT_SCREEN, EKeyCode::PrintScreen},
        {GLFW_KEY_PAUSE, EKeyCode::Pause},
        {GLFW_KEY_F1, EKeyCode::F1},
        {GLFW_KEY_F2, EKeyCode::F2},
        {GLFW_KEY_F3, EKeyCode::F3},
        {GLFW_KEY_F4, EKeyCode::F4},
        {GLFW_KEY_F5, EKeyCode::F5},
        {GLFW_KEY_F6, EKeyCode::F6},
        {GLFW_KEY_F7, EKeyCode::F7},
        {GLFW_KEY_F8, EKeyCode::F8},
        {GLFW_KEY_F9, EKeyCode::F9},
        {GLFW_KEY_F10, EKeyCode::F10},
        {GLFW_KEY_F11, EKeyCode::F11},
        {GLFW_KEY_F12, EKeyCode::F12},
        {GLFW_KEY_KP_0, EKeyCode::KP_0},
        {GLFW_KEY_KP_1, EKeyCode::KP_1},
        {GLFW_KEY_KP_2, EKeyCode::KP_2},
        {GLFW_KEY_KP_3, EKeyCode::KP_3},
        {GLFW_KEY_KP_4, EKeyCode::KP_4},
        {GLFW_KEY_KP_5, EKeyCode::KP_5},
        {GLFW_KEY_KP_6, EKeyCode::KP_6},
        {GLFW_KEY_KP_7, EKeyCode::KP_7},
        {GLFW_KEY_KP_8, EKeyCode::KP_8},
        {GLFW_KEY_KP_9, EKeyCode::KP_9},
        {GLFW_KEY_KP_DECIMAL, EKeyCode::KP_Decimal},
        {GLFW_KEY_KP_DIVIDE, EKeyCode::KP_Divide},
        {GLFW_KEY_KP_MULTIPLY, EKeyCode::KP_Multiply},
        {GLFW_KEY_KP_SUBTRACT, EKeyCode::KP_Subtract},
        {GLFW_KEY_KP_ADD, EKeyCode::KP_Add},
        {GLFW_KEY_KP_ENTER, EKeyCode::KP_Enter},
        {GLFW_KEY_KP_EQUAL, EKeyCode::KP_Equal},
        {GLFW_KEY_LEFT_SHIFT, EKeyCode::LeftShift},
        {GLFW_KEY_LEFT_CONTROL, EKeyCode::LeftControl},
        {GLFW_KEY_LEFT_ALT, EKeyCode::LeftAlt},
        {GLFW_KEY_RIGHT_SHIFT, EKeyCode::RightShift},
        {GLFW_KEY_RIGHT_CONTROL, EKeyCode::RightControl},
        {GLFW_KEY_RIGHT_ALT, EKeyCode::RightAlt},
    };

    auto KeyIt = KeyLookup.find(Key);

    if(KeyIt == KeyLookup.end()) { return EKeyCode::Unknown; }

    return KeyIt->second;
}

inline EKeyAction TranslateKeyAction(int Action) {
    if(Action == GLFW_PRESS) { return EKeyAction::Down; }
    if(Action == GLFW_RELEASE) { return EKeyAction::Up; }
    if(Action == GLFW_REPEAT) { return EKeyAction::Repeat; }

    return EKeyAction::Unknown;
}

void KeyCallback(
    GLFWwindow *Window, int Key, int /*scancode*/, int Action, int /*mods*/) {
    EKeyCode KeyCode = TranslateKeyCode(Key);
    EKeyAction KeyAction = TranslateKeyAction(Action);

    if(auto Platform = static_cast<FPlatform *>(glfwGetWindowUserPointer(Window))) {
        Platform->InputEvent(FKeyInputEvent{KeyCode, KeyAction});
    }
}

inline EMouseButton TranslateMouseButton(int Button) {
    if(Button < GLFW_MOUSE_BUTTON_6) { return static_cast<EMouseButton>(Button); }

    return EMouseButton::Unknown;
}

inline EMouseAction TranslateMouseAction(int Action) {
    if(Action == GLFW_PRESS) { return EMouseAction::Down; }
    if(Action == GLFW_RELEASE) { return EMouseAction::Up; }

    return EMouseAction::Unknown;
}

void CursorPositionCallback(GLFWwindow *Window, double XPos, double YPos) {
    if(auto *Platform = static_cast<FPlatform *>(glfwGetWindowUserPointer(Window))) {
        Platform->InputEvent(FMouseButtonInputEvent{
            EMouseButton::Unknown, EMouseAction::Move, static_cast<float>(XPos),
            static_cast<float>(YPos)});
    }
}

void MouseButtonCallback(GLFWwindow *Window, int Button, int Action, int /*mods*/) {
    EMouseAction MouseAction = TranslateMouseAction(Action);

    if(auto *Platform = static_cast<FPlatform *>(glfwGetWindowUserPointer(Window))) {
        double XPos, YPos;
        glfwGetCursorPos(Window, &XPos, &YPos);

        Platform->InputEvent(FMouseButtonInputEvent{
            TranslateMouseButton(Button), MouseAction, static_cast<float>(XPos),
            static_cast<float>(YPos)});
    }
}
}

FGltfWindow::FGltfWindow(FPlatform *Platform, const FWindow::FProperties &Properties)
    : FWindow{Properties} {
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, false);
#endif
    checkf(glfwInit(), "Failed to initialize GLFW");
    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    switch(Properties.Mode) {
        case EMode::Fullscreen: {
            auto *Monitor = glfwGetPrimaryMonitor();
            const auto *Mode = glfwGetVideoMode(Monitor);
            Handle = glfwCreateWindow(
                Mode->width, Mode->height, Properties.Title.c_str(), Monitor, nullptr);
            break;
        }

        case EMode::FullscreenBorderless: {
            auto *Monitor = glfwGetPrimaryMonitor();
            const auto *Mode = glfwGetVideoMode(Monitor);
            glfwWindowHint(GLFW_RED_BITS, Mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, Mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, Mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, Mode->refreshRate);
            Handle = glfwCreateWindow(
                Mode->width, Mode->height, Properties.Title.c_str(), Monitor, nullptr);
            break;
        }

        case EMode::FullscreenStretch: {
            RE_LOGE("Cannot support stretch mode on this platform.");
            break;
        }

        default:
            Handle = glfwCreateWindow(
                int(Properties.Extent.Width), int(Properties.Extent.Height),
                Properties.Title.c_str(), nullptr, nullptr);
            break;
    }
    checkf(Handle, "Failed to create GLFW window");

    Resize({Properties.Extent.Width, Properties.Extent.Height});

    glfwSetWindowUserPointer(Handle, Platform);

    glfwSetWindowCloseCallback(Handle, WindowCloseCallback);
    glfwSetWindowSizeCallback(Handle, WindowSizeCallback);
    glfwSetWindowFocusCallback(Handle, WindowFocusCallback);
    glfwSetKeyCallback(Handle, KeyCallback);
    glfwSetCursorPosCallback(Handle, CursorPositionCallback);
    glfwSetMouseButtonCallback(Handle, MouseButtonCallback);

    glfwSetInputMode(Handle, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(Handle, GLFW_STICKY_MOUSE_BUTTONS, 1);
}
FGltfWindow::~FGltfWindow() { glfwTerminate(); }
bool FGltfWindow::ShouldClose() { return glfwWindowShouldClose(Handle); }
void FGltfWindow::ProcessEvents() { glfwPollEvents(); }
void FGltfWindow::Close() { glfwSetWindowShouldClose(Handle, GLFW_TRUE); }
float FGltfWindow::GetDpiFactor() const {
    // It calculates the dpi factor using the density from GLFW physical size
    auto PrimaryMonitor = glfwGetPrimaryMonitor();
    auto VideoMode = glfwGetVideoMode(PrimaryMonitor);

    int WidthMM, HeightMM;
    glfwGetMonitorPhysicalSize(PrimaryMonitor, &WidthMM, &HeightMM);

    // As suggested by the GLFW monitor guide
    static const float InnchToMM = 25.0f;
    static const float WinBaseDensity = 96.0f;

    auto Dpi = static_cast<uint32_t>(VideoMode->width / (WidthMM / InnchToMM));
    auto DpiFactor = Dpi / WinBaseDensity;
    return DpiFactor;
}
float FGltfWindow::GetContentScaleFactor() const {
    int FbWidth, FbHeight;
    glfwGetFramebufferSize(Handle, &FbWidth, &FbHeight);
    int WinWidth, WinHheight;
    glfwGetWindowSize(Handle, &WinWidth, &WinHheight);

    // We could return a 2D result here instead of a scalar,
    // but non-uniform scaling is very unlikely, and would
    // require significantly more changes in the IMGUI integration
    return static_cast<float>(FbWidth) / WinWidth;
}
auto FGltfWindow::GetRequiredSurfaceExtensions() const -> std::vector<const char *> {
    uint32_t GlfwExtensionCount{0};
    const char **Names = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);
    return {Names, Names + GlfwExtensionCount};
}
void *FGltfWindow::GetNativeWindow() { return glfwGetWin32Window(Handle); }
}