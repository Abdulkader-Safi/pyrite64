cmake_minimum_required(VERSION 3.28)

# Runtime engine library
add_library(pyrite64-runtime STATIC
    # Platform
    src/platform/window.cpp
    src/platform/input.cpp
    src/platform/audio.cpp
    src/platform/filesystem.cpp

    # Engine lib
    src/engine/lib/math.cpp

    # Engine collision
    src/engine/collision/shapes.cpp
    src/engine/collision/bvh.cpp
    src/engine/collision/mesh.cpp
    src/engine/collision/meshLoader.cpp
    src/engine/collision/resolver.cpp
    src/engine/collision/scene.cpp
    src/engine/collision/attach.cpp

    # Engine scene
    src/engine/scene/object.cpp
    src/engine/scene/scene.cpp
    src/engine/scene/sceneManager.cpp
    src/engine/scene/componentTable.cpp
)

target_include_directories(pyrite64-runtime PUBLIC
    src
    ${CMAKE_SOURCE_DIR}/vendored
    ${CMAKE_SOURCE_DIR}/vendored/glm
)

target_compile_definitions(pyrite64-runtime PUBLIC
    PLATFORM_PC
    GLM_FORCE_QUAT_DATA_XYZW
    GLM_ENABLE_EXPERIMENTAL
)

target_link_libraries(pyrite64-runtime PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    glm::glm
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(pyrite64-runtime PUBLIC HAS_SHADER_CROSS=1)
    target_link_libraries(pyrite64-runtime PUBLIC
        SDL3_shadercross::SDL3_shadercross-static
    )
    target_include_directories(pyrite64-runtime PUBLIC
        ${CMAKE_SOURCE_DIR}/vendored/SDL_shadercross/include
    )
endif()

# Game executable
add_executable(pyrite64-game
    src/main.cpp
)

target_link_libraries(pyrite64-game PRIVATE
    pyrite64-runtime
)

set_target_properties(pyrite64-game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/runtime"
)

if(MINGW)
    target_link_options(pyrite64-game PRIVATE
        -static-libgcc
        -static-libstdc++
        --static
    )
endif()
