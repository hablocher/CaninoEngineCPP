#ifdef _WIN32
#include <canino/platform/window.h>
#include <canino/core/core.h>

// As macros WIN32_LEAN_AND_MEAN e NOMINMAX já são injetadas pelo CMake via command line
#include <windows.h>
#include <cstdlib>
#include <cstring>

namespace canino {

// PIMPL State Native OS
struct OSWindowState {
    HWND Hwnd;
    HINSTANCE HInstance;
};

// Opaque struct revelada internamente pro framework C++
struct Window {
    OSWindowState OSState;
    bool ShouldClose;
    unsigned int Width;
    unsigned int Height;
    bool IsCursorLocked;
    InputState State; // Nosso pacote contíguo de Inputs OS
};

// Callback oficial de loop do Sistema Win32 interceptado pra dentro da nossa Opaque
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Window* window = nullptr;

    // Magia Negra para amarrar a struct Instanciada (this) no HWND (Janela) Nativo
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        window = (Window*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
    } else {
        window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (window) {
        switch (uMsg) {
            case WM_CLOSE: // Clique no botão X
            case WM_DESTROY: { // Força bruta Quit (Alt-F4)
                window->ShouldClose = true;
                return 0; // Previne liberação bruta pra encerrar nosso GameLoop elegante
            }
            // Capture Keyboard
            case WM_KEYDOWN:
            case WM_KEYUP: {
                bool isDown = (uMsg == WM_KEYDOWN);
                int vkCode = (int)wParam;
                // Tratamento especial unificando a Virtual key do windows na nossa Opaque struct
                if (vkCode == VK_ESCAPE) vkCode = CANINO_KEY_ESCAPE; // Overwrite
                if (vkCode < 512) {
                    window->State.CurrentKeys[vkCode] = isDown;
                }
                return 0;
            }
            // Capture Mouse
            case WM_MOUSEMOVE: {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                window->State.MouseDeltaX += (x - window->State.MouseX);
                window->State.MouseDeltaY += (y - window->State.MouseY);
                window->State.MouseX = x;
                window->State.MouseY = y;
                return 0;
            }
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP: {
                bool isDown = (uMsg == WM_LBUTTONDOWN);
                window->State.CurrentMouse[CANINO_MOUSE_LEFT] = isDown;
                return 0;
            }
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP: {
                bool isDown = (uMsg == WM_RBUTTONDOWN);
                window->State.CurrentMouse[CANINO_MOUSE_RIGHT] = isDown;
                return 0;
            }
        }
    }

    // Default system behaviour
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Window* PlatformCreateWindow(const WindowDesc& config) {
    // Aloca a Opaque na Heap pra persistir enquanto vive (usaremos nossa Arena futuramente se precisarmos)
    Window* window = (Window*)std::malloc(sizeof(Window));
    // Zero init do array gigante pra n crasharmos hardware ram leaks lixosos
    std::memset(window, 0, sizeof(Window)); 
    
    window->ShouldClose = false;
    window->Width = config.Width;
    window->Height = config.Height;
    window->OSState.HInstance = GetModuleHandle(nullptr); // Pega Thread Base de injecao MSVC

    const char* className = "CaninoEngineWindowClass";
    
    // Estrutural basica do buffer de display e callbacks de input
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    // Removemos flickers forçando repintura dupla 
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = window->OSState.HInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassExA(&wc)) {
        // Ignoramos erro silencioso pra previnir crash com hotloads repetidos da classe
    }

    // Ajusta a resolução pra ser client-side interna não contando as bordas grossas do Windows 11
    RECT wr = { 0, 0, (LONG)config.Width, (LONG)config.Height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    // Instancia Hardware Context de GDI na Placa Mãe
    window->OSState.Hwnd = CreateWindowExA(
        0, className, config.Title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, window->OSState.HInstance,
        window // Ponteiro magico que é engolido lá no WM_NCCREATE do nosso handler
    );

    if (!window->OSState.Hwnd) {
        CANINO_ASSERT(false && "[Platform] Fudeu. Criação fisica falhou a nível de Kernel.");
        std::free(window);
        return nullptr;
    }

    // Force Expose
    ShowWindow(window->OSState.Hwnd, SW_SHOWDEFAULT);
    
    return window;
}

void PlatformDestroyWindow(Window* window) {
    if (window) {
        if (window->OSState.Hwnd) {
            DestroyWindow(window->OSState.Hwnd); // Desaloca GDI Memory HW
        }
        std::free(window); // Remove cache
    }
}

void PlatformPumpMessages(Window* window) {
    if (!window) return;

    MSG msg = {};
    // Puxa toda a fila sem blockar Thread
    while (PeekMessage(&msg, window->OSState.Hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg); // Parsers brutos pra keys compostas
        DispatchMessage(&msg);  // Envia e desvia de volta pro DefWindowProc / Nosso Handler
    }
}

bool PlatformWindowShouldClose(Window* window) {
    if (!window) return true;
    return window->ShouldClose;
}

const InputState* PlatformGetInputState(Window* window) {
    if (!window) return nullptr;
    return &window->State;
}

void PlatformUpdateInputState(Window* window) {
    if (!window) return;
    
    // Transferencia massiva hiper veloz dos conteudos bit a bit pros Previous (Sync End Frame)
    for (int i = 0; i < 512; ++i) window->State.PreviousKeys[i] = window->State.CurrentKeys[i];
    for (int i = 0; i < 3; ++i) window->State.PreviousMouse[i] = window->State.CurrentMouse[i];
    window->State.MouseDeltaX = 0;
    window->State.MouseDeltaY = 0;

    // Se estiver locked, chutar o cursor violentamente pro meio da area Cliente
    if (window->IsCursorLocked && window->OSState.Hwnd) {
        RECT rect;
        GetClientRect(window->OSState.Hwnd, &rect);
        POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
        ClientToScreen(window->OSState.Hwnd, &center); // Win32 set position precisa da coordenata total do monitor
        SetCursorPos(center.x, center.y);
        
        // Mutação silenciosa pra evitar Delta recursivo do Windows detectando "falso mouse-move"
        window->State.MouseX = (rect.right - rect.left) / 2;
        window->State.MouseY = (rect.bottom - rect.top) / 2;
    }
}

void PlatformLockCursor(Window* window, bool lock) {
    if (!window) return;
    window->IsCursorLocked = lock;
    ShowCursor(!lock);
}

void* PlatformGetNativeWindowHandle(Window* window) {
    if (!window) return nullptr;
    return (void*)window->OSState.Hwnd;
}

}
#endif
