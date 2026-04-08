#include "input.h"

namespace canino {

bool IsKeyDown(const InputState* state, KeyCode key) {
    if (!state || key >= 512) return false;
    return state->CurrentKeys[key];
}

bool IsKeyPressed(const InputState* state, KeyCode key) {
    if (!state || key >= 512) return false;
    return state->CurrentKeys[key] && !state->PreviousKeys[key];
}

bool IsKeyReleased(const InputState* state, KeyCode key) {
    if (!state || key >= 512) return false;
    return !state->CurrentKeys[key] && state->PreviousKeys[key];
}

bool IsMouseButtonDown(const InputState* state, MouseButton button) {
    if (!state || button >= 3) return false;
    return state->CurrentMouse[button];
}

bool IsMouseButtonPressed(const InputState* state, MouseButton button) {
    if (!state || button >= 3) return false;
    return state->CurrentMouse[button] && !state->PreviousMouse[button];
}

}
