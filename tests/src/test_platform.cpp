#include "test_core.h"
#include <canino/platform/window.h>

static bool Test_WindowCreationAndDestruction() {
    // Configuraçao bruta sem invocar polimorfismos class abstractions da std
    canino::WindowDesc desc = {"Canino Internal Headless Test", 100, 100};
    
    // O Win32 WinProc vai processar e vomitar o Handler alocado
    canino::Window* win = canino::PlatformCreateWindow(desc);
    
    CANINO_EXPECT(win != nullptr);
    
    // Dá um pump cego p/ Windows desopilar filas Message Queue atrasadas do S.O
    canino::PlatformPumpMessages(win);
    
    // Assert no Handle que nós proprios escrevemos
    CANINO_EXPECT(canino::PlatformWindowShouldClose(win) == false);
    
    // Frieza absoluta pra matar e desalocar a GDI Context Memory associada e evitar Leak
    canino::PlatformDestroyWindow(win);
    return true;
}

bool RunPlatformTests() {
    // Esse teste provara que nosso Isolamento funciona
    CANINO_RUN_TEST(Test_WindowCreationAndDestruction);
    return true;
}
