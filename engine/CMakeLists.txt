add_library(CaninoEngine SHARED)

target_sources(CaninoEngine PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/canino.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/core.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/memory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/memory_arena.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/memory_arena.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/memory_pool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/memory_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/platform/window.h
)

# Adicionamos dinamicamente as classes de Platform dependendo do host/target do compiler
if(WIN32)
    target_sources(CaninoEngine PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/platform/windows/window_win32.cpp
    )
    # Win32 GDI Libs
    target_link_libraries(CaninoEngine PRIVATE user32.lib gdi32.lib)
endif()

# Diretório público base de includes (ex: para engolir "canino/core/core.h")
target_include_directories(CaninoEngine PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
    "$<INSTALL_INTERFACE:include>"
)

# Garante a macro de exportação DLL ativada neste target (export para engine, e import para clients)
target_compile_definitions(CaninoEngine PRIVATE CANINO_BUILD_DLL)
