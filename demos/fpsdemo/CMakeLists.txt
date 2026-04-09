project(FPSDemo)

# Executável do Demo 3 isolado C++20
add_executable(FPSDemo src/main.cpp)

# Output no mesmo padrão das pipelines binárias
set_target_properties(FPSDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
)

# Acopla a Morte à Engine Root!
target_link_libraries(FPSDemo PRIVATE CaninoEngine)
