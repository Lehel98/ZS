cmake_minimum_required(VERSION 3.21)

project(
    ZombieSurvival
    VERSION 0.1
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# GLFW options
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
add_executable(ZombieSurvival ${SOURCE_FILES})

target_include_directories(
    ZombieSurvival
    PRIVATE
        external/glfw/include
        external/glad/include
        external/glm
)

target_link_libraries(
    ZombieSurvival
    PRIVATE
        glfw
        opengl32
)

target_compile_definitions(ZombieSurvival PRIVATE
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
)