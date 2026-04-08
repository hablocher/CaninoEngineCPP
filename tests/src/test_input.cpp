#include "test_core.h"
#include <canino/platform/input.h>

static bool Test_InputStateLogics() {
    canino::InputState state = {};
    
    // Simulate raw memory injection
    state.CurrentKeys[canino::CANINO_KEY_W] = true;
    state.PreviousKeys[canino::CANINO_KEY_W] = false;
    
    // Test helper deductions
    CANINO_EXPECT(canino::IsKeyDown(&state, canino::CANINO_KEY_W) == true);
    CANINO_EXPECT(canino::IsKeyPressed(&state, canino::CANINO_KEY_W) == true);
    CANINO_EXPECT(canino::IsKeyReleased(&state, canino::CANINO_KEY_W) == false);

    // Simulate next frame where key is held down
    state.PreviousKeys[canino::CANINO_KEY_W] = true;
    
    CANINO_EXPECT(canino::IsKeyDown(&state, canino::CANINO_KEY_W) == true);
    CANINO_EXPECT(canino::IsKeyPressed(&state, canino::CANINO_KEY_W) == false); // Not instantly pressed anymore

    return true;
}

bool RunInputTests() {
    CANINO_RUN_TEST(Test_InputStateLogics);
    return true;
}
