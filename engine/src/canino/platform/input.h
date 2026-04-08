#pragma once
#include <canino/core/core.h>
#include <cstdint>

namespace canino {
    // Teclas traduzidas do SO (Baseado livremente asciis primitivos para bater quase 1:1 com Win32)
    enum KeyCode : uint16_t {
        CANINO_KEY_SPACE = 32,
        CANINO_KEY_ESCAPE = 256,
        CANINO_KEY_W = 87,
        CANINO_KEY_A = 65,
        CANINO_KEY_S = 83,
        CANINO_KEY_D = 68
    };

    enum MouseButton : uint8_t {
        CANINO_MOUSE_LEFT = 0,
        CANINO_MOUSE_RIGHT = 1,
        CANINO_MOUSE_MIDDLE = 2
    };

    // Objeto primitivo puro de bytes (O(1) Memory Layout)
    struct InputState {
        bool CurrentKeys[512];
        bool PreviousKeys[512];
        bool CurrentMouse[3];
        bool PreviousMouse[3];
        int MouseX, MouseY;
        int MouseDeltaX, MouseDeltaY;
    };

    // Pipeline Methods
    // Check bruto O(1) de bitwise da malha física state
    CANINO_API bool IsKeyDown(const InputState* state, KeyCode key);
    
    // Transição 0 pra 1 no ultimo frame
    CANINO_API bool IsKeyPressed(const InputState* state, KeyCode key);
    
    // Transição 1 pra 0
    CANINO_API bool IsKeyReleased(const InputState* state, KeyCode key);

    CANINO_API bool IsMouseButtonDown(const InputState* state, MouseButton button);
    CANINO_API bool IsMouseButtonPressed(const InputState* state, MouseButton button);
}
