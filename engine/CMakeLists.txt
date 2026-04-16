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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/platform/input.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/platform/input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/render/rhi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/math/math_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/math/math_ops.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/job_system.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/job_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/ecs/ecs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/ecs/component_array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/ecs/registry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/ecs/registry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/config_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/core/config_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/render/rhi_backend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/render/rhi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/render/backends/opengl_rhi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/render/backends/dx11_rhi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/render/backends/dx12_rhi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/render/backends/vulkan_rhi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/render/mesh_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/physics/physics.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/physics/physics.cpp
)

# Adicionamos dinamicamente as classes de Platform dependendo do host/target do compiler
if(WIN32)
    target_sources(CaninoEngine PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/canino/platform/windows/window_win32.cpp
    )
    # Win32 GDI Libs e Contextos Gráficos
    target_link_libraries(CaninoEngine PRIVATE user32.lib gdi32.lib opengl32.lib d3d11.lib dxgi.lib d3dcompiler.lib)
endif()

# Diretório público base de includes (ex: para engolir "canino/core/core.h")
target_include_directories(CaninoEngine PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
    "$<INSTALL_INTERFACE:include>"
)

# Garante a macro de exportação DLL ativada neste target (export para engine, e import para clients)
target_compile_definitions(CaninoEngine PRIVATE CANINO_BUILD_DLL)
