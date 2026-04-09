set(DEMO2_SOURCES
    src/main.cpp
)

add_executable(Demo2 ${DEMO2_SOURCES})

# O Demo depende da engine pura
target_link_libraries(Demo2 PRIVATE CaninoEngine)
