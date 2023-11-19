#pragma once

#include "App/Platform.h"

struct GLFWwindow;

namespace RE {

class FGltfWindow final: public FWindow {
  public:
    FGltfWindow(FPlatform *Platform, const FProperties &Properties);
    ~FGltfWindow() override;

    bool ShouldClose() override;
    void ProcessEvents() override;
    void Close() override;
    float GetDpiFactor() const override;
    float GetContentScaleFactor() const override;
    auto GetRequiredSurfaceExtensions() const -> std::vector<const char *> override;
    void *GetNativeWindow() override;

private:
    GLFWwindow *Handle = nullptr;
};
}