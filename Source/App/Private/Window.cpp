#include "App/Window.h"

namespace RE {
FWindow::FWindow(const FProperties &Properties): Properties{Properties} {}
FWindow::FExtent FWindow::Resize(const FExtent &InExtent) {
    if(Properties.Resizable) {
        Properties.Extent.Width = InExtent.Width;
        Properties.Extent.Height = InExtent.Height;
    }

    return Properties.Extent;
}

}