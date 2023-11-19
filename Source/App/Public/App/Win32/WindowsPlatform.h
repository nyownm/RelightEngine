#pragma once
#include "App/Platform.h"

#include <windows.h>

#define CUSTOM_MAIN                                                                   \
    int platform_main(RE::FPlatform &);                                               \
    int __stdcall WinMain(                                                            \
        HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) { \
        RE::FWindowsPlatform platform{hInstance, hPrevInstance, lpCmdLine, nCmdShow}; \
        return platform_main(platform);                                               \
    }                                                                                 \
    int platform_main

namespace RE {
class FWindowsPlatform final: public FPlatform {
  public:
    FWindowsPlatform(
        HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow);
    ~FWindowsPlatform() override = default;
    void CreateMainWindow(const FWindow::FProperties &Properties) override;
};
}
