#include "App/Win32/WindowsPlatform.h"
#include <shellapi.h>
#include "App/Win32/GltfWindow.h"

namespace RE {
std::string WstrToStr(const std::wstring &Wstr) {
    if(Wstr.empty()) { return {}; }

    auto WstrLen = static_cast<int>(Wstr.size());
    auto StrLen = WideCharToMultiByte(CP_UTF8, 0, &Wstr[0], WstrLen, NULL, 0, NULL, NULL);

    std::string Str(StrLen, 0);
    WideCharToMultiByte(CP_UTF8, 0, &Wstr[0], WstrLen, &Str[0], StrLen, NULL, NULL);

    return Str;
}

std::vector<std::string> GetArgs() {
    LPWSTR *Argv;
    int Argc;

    Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);

    // Ignore the first argument containing the application full path
    std::vector<std::wstring> ArgStrings(Argv + 1, Argv + Argc);
    std::vector<std::string> Args;

    Args.reserve(ArgStrings.size());
    for(auto &arg: ArgStrings) {
        Args.push_back(WstrToStr(arg));
    }

    return Args;
}

FWindowsPlatform::FWindowsPlatform(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    Arguments = GetArgs();
    // Attempt to attach to the parent process console if it exists
    if(!AttachConsole(ATTACH_PARENT_PROCESS)) {
        // No parent console, allocate a new one for this process
        if(!AllocConsole()) { throw std::runtime_error{"AllocConsole error"}; }
    }

    FILE *fp;
    freopen_s(&fp, "conin$", "r", stdin);
    freopen_s(&fp, "conout$", "w", stdout);
    freopen_s(&fp, "conout$", "w", stderr);
}
void FWindowsPlatform::CreateMainWindow(const FWindow::FProperties &Properties) {
    Window = std::make_unique<FGltfWindow>(this, Properties);
}
}
