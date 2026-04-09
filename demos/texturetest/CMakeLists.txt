project(TextureTest)
add_executable(TextureTest src/main.cpp)
target_link_libraries(TextureTest PRIVATE CaninoEngine)
target_compile_definitions(TextureTest PRIVATE CANINO_BUILD_CLIENT)
