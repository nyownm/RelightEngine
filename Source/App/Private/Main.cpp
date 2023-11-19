#include <string>
#include "App/Win32/WindowsPlatform.h"

#include "core/Logging.h"

CUSTOM_MAIN(RE::FPlatform &Platform) {

    RE::FWindow::FProperties Properties{
        .Title = "RelightEngine",
    };
    Platform.CreateMainWindow(Properties);

    Platform.EngineLoop();

    return 0;
}