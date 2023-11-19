/* Copyright (c) 2019-2021, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace RE {
class FPlatform;

enum class EEventSource { Keyboard, Mouse, Touchscreen };

class FInputEvent {
  public:
    FInputEvent(EEventSource Source);

    EEventSource GetSource() const;

  private:
    EEventSource Source;
};

enum class EKeyCode {
    Unknown,
    Space,
    Apostrophe, /* ' */
    Comma,      /* , */
    Minus,      /* - */
    Period,     /* . */
    Slash,      /* / */
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    Semicolon, /* ; */
    Equal,     /* = */
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LeftBracket,  /* [ */
    Backslash,    /* \ */
    RightBracket, /* ] */
    GraveAccent,  /* ` */
    Escape,
    Enter,
    Tab,
    Backspace,
    Insert,
    DelKey,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,
    Back,
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    KP_0,
    KP_1,
    KP_2,
    KP_3,
    KP_4,
    KP_5,
    KP_6,
    KP_7,
    KP_8,
    KP_9,
    KP_Decimal,
    KP_Divide,
    KP_Multiply,
    KP_Subtract,
    KP_Add,
    KP_Enter,
    KP_Equal,
    LeftShift,
    LeftControl,
    LeftAlt,
    RightShift,
    RightControl,
    RightAlt
};

enum class EKeyAction { Down, Up, Repeat, Unknown };

class FKeyInputEvent: public FInputEvent {
  public:
    FKeyInputEvent(EKeyCode Code, EKeyAction Action);

    EKeyCode GetCode() const;

    EKeyAction GetAction() const;

  private:
    EKeyCode Code;

    EKeyAction Action;
};

enum class EMouseButton { Left, Right, Middle, Back, Forward, Unknown };

enum class EMouseAction { Down, Up, Move, Unknown };

class FMouseButtonInputEvent: public FInputEvent {
  public:
    FMouseButtonInputEvent(
        EMouseButton Button, EMouseAction Action, float PosX, float PosY);

    EMouseButton GetButton() const;

    EMouseAction GetAction() const;

    float GetPosX() const;

    float GetPosY() const;

  private:
    EMouseButton Button;

    EMouseAction Action;

    float PosX;

    float PosY;
};

enum class ETouchAction { Down, Up, Move, Cancel, PointerDown, PointerUp, Unknown };

class FTouchInputEvent: public FInputEvent {
  public:
    FTouchInputEvent(
        int32_t PointerId, size_t PointerCount, ETouchAction Action, float PosX,
        float PosY);

    ETouchAction GetAction() const;

    int32_t GetPointerId() const;

    size_t GetTouchPoints() const;

    float GetPosX() const;

    float GetPosY() const;

  private:
    ETouchAction Action;

    int32_t PointerId;

    size_t TouchPoints;

    float PosX;

    float PosY;
};
}
