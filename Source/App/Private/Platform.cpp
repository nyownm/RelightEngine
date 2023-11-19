#include "App/Platform.h"
namespace RE {
void FPlatform::Resize(uint32_t Width, uint32_t Height) {}
void FPlatform::SetFocus(bool bFocused) {}
void FPlatform::InputEvent(const FInputEvent &InputEvent) {}
void FPlatform::EngineLoop() {
    Renderer = std::make_unique<FRenderer>(Window->GetNativeWindow());
    while(!Window->ShouldClose() && !bCloseRequested) {
        Window->ProcessEvents();
        Renderer->Tick();
    }

    Renderer.reset();
}
}