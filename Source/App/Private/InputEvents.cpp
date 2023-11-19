#include "App/InputEvents.h"

namespace RE {
FInputEvent::FInputEvent(EEventSource Source): Source{Source} {}

EEventSource FInputEvent::GetSource() const { return Source; }

FKeyInputEvent::FKeyInputEvent(EKeyCode Code, EKeyAction Action)
    : FInputEvent{EEventSource::Keyboard}, Code{Code}, Action{Action} {}

EKeyCode FKeyInputEvent::GetCode() const { return Code; }

EKeyAction FKeyInputEvent::GetAction() const { return Action; }

FMouseButtonInputEvent::FMouseButtonInputEvent(
    EMouseButton Button, EMouseAction Action, float PosX, float PosY)
    : FInputEvent{EEventSource::Mouse},
      Button{Button},
      Action{Action},
      PosX{PosX},
      PosY{PosY} {}

EMouseButton FMouseButtonInputEvent::GetButton() const { return Button; }

EMouseAction FMouseButtonInputEvent::GetAction() const { return Action; }

float FMouseButtonInputEvent::GetPosX() const { return PosX; }

float FMouseButtonInputEvent::GetPosY() const { return PosY; }

FTouchInputEvent::FTouchInputEvent(
    int32_t PointerId, std::size_t TouchPoints, ETouchAction Action, float PosX,
    float PosY)
    : FInputEvent{EEventSource::Touchscreen},
      Action{Action},
      PointerId{PointerId},
      TouchPoints{TouchPoints},
      PosX{PosX},
      PosY{PosY} {}

ETouchAction FTouchInputEvent::GetAction() const { return Action; }

int32_t FTouchInputEvent::GetPointerId() const { return PointerId; }

std::size_t FTouchInputEvent::GetTouchPoints() const { return TouchPoints; }

float FTouchInputEvent::GetPosX() const { return PosX; }

float FTouchInputEvent::GetPosY() const { return PosY; }
}
