#pragma once

#include <vector>
#include <string>
#include "Core/Logging.h"

#include "App/InputEvents.h"
#include "App/Window.h"
#include "Render/Renderer.h"

namespace RE {
class FPlatform {
public:
    virtual ~FPlatform() = default;

    virtual auto GetArguments() const -> const std::vector<std::string> & {
        return Arguments;
    }

    virtual void CreateMainWindow(const FWindow::FProperties &Properties) = 0;

    virtual void Resize(uint32_t Width, uint32_t Height);

    virtual void SetFocus(bool bFocused);

    virtual void InputEvent(const FInputEvent &InputEvent);

    void EngineLoop();

protected:
    std::vector<std::string> Arguments;

    std::unique_ptr<FWindow> Window{nullptr};

    std::unique_ptr<FRenderer> Renderer{nullptr};

    /* Source of truth for window state */
    FWindow::FProperties Properties;
    /* Delta time should be fixed with a fabricated value */
    bool bFixedSimulationFps{false};
    /* A fabricated delta time */
    float SimulationFrameTime = 0.016f;
    /* App should continue processing input events */
    bool bProcessInputEvents{true};
    bool bFocused{true};
    bool bCloseRequested{false};

    FPlatform() = default;
};
}