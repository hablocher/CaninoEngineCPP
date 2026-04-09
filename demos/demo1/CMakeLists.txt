set(DEMO1_SOURCES
    src/main.cpp
)

add_executable(Demo1 ${DEMO1_SOURCES})

# O Demo depende da engine. Garantimos que ele fassa link.
target_link_libraries(Demo1 PRIVATE CaninoEngine)
