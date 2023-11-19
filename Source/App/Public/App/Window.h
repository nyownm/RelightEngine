#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace RE {
class FWindow {
public:
    struct FExtent {
        uint32_t Width;
        uint32_t Height;
    };
    enum class EMode {
        Headless,
        Fullscreen,
        FullscreenBorderless,
        FullscreenStretch,
        Default
    };
    enum class EVsync { OFF, ON, Default };

    struct FProperties {
        std::string Title;
        EMode Mode{EMode::Default};
        bool Resizable{true};
        EVsync Vsync{EVsync::Default};
        FExtent Extent{1280, 720};
    };

    explicit FWindow(const FProperties &Properties);
    virtual ~FWindow() = default;

    virtual bool ShouldClose() = 0;

    virtual void ProcessEvents() {}

    virtual void Close() = 0;

    virtual float GetDpiFactor() const = 0;

    virtual float GetContentScaleFactor() const { return 1.0f; }

    FExtent Resize(const FExtent &InExtent);

    virtual auto GetRequiredSurfaceExtensions() const -> std::vector<const char *> = 0;

    const FExtent &GetExtent() const;

    EMode GetWindowMode() const { return Properties.Mode; }

    auto GetProperties() const -> const FProperties & { return Properties; }
    virtual void *GetNativeWindow() = 0;

protected:
    FProperties Properties;
};
}