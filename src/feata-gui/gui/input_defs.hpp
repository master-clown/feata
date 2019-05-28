#pragma once

#include "geom/point.hpp"

#define MOUSE_SINGLE_CLICK_MSTIME 1000   // assume a click as single if the button has been held for < 1000 ms


/**
 * Values of enums are compatible with analogous enums from Qt
 */
namespace gui
{
    enum MouseButtonBit
    {
        MOUSE_BUTTON_BIT_NONE = 0,
        MOUSE_BUTTON_BIT_L = 0b001,
        MOUSE_BUTTON_BIT_R = 0b010,
        MOUSE_BUTTON_BIT_M = 0b100
    };

    enum KeyboardModif
    {
        KEYBOARD_MODS_NONE,
        KEYBOARD_MODS_SHIFT = 0x02000000,
        KEYBOARD_MODS_CTRL  = 0x04000000,
        KEYBOARD_MODS_ALT   = 0x08000000,
        KEYBOARD_MODS_META  = 0x10000000
    };

    struct MouseInput
    {
        MouseButtonBit Button;
        KeyboardModif KbMods;
        geom::Point2i Pos;

        MouseInput(int btn, int kb_mods, int x, int y)
            : Button(MouseButtonBit(btn))
            , KbMods(KeyboardModif(kb_mods))
            , Pos({x, y})
        {}
    };
}
