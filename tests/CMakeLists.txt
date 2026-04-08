add_executable(CaninoTests 
    src/test_main.cpp 
    src/test_memory.cpp 
    src/test_platform.cpp
    src/test_input.cpp
)

# Precisamos do pacote base da engine p/ testar e das pastas de includes publicos 
target_link_libraries(CaninoTests PRIVATE CaninoEngine)
target_include_directories(CaninoTests PRIVATE src)

# Hookup com o CTest test-runner
add_test(NAME CaninoCoreTests COMMAND CaninoTests)
