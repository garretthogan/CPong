# CMakeLists.txt : Cross-platform 3D Pong - Windows, Mac, Linux
cmake_minimum_required(VERSION 3.16)
project(CPong VERSION 1.0.0 LANGUAGES CXX)

# Enable Hot Reload for MSVC compilers if supported
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  if(MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
  endif()
endif()

set(CMAKE_CXX_STANDARD 17)

# Use dynamic CRT (matches GLFW) to avoid LNK4098 conflicts
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Platform-specific OpenGL library
if(WIN32)
  set(OPENGL_LIBRARIES opengl32)
elseif(APPLE)
  find_library(OPENGL_LIBRARY OpenGL REQUIRED)
  set(OPENGL_LIBRARIES ${OPENGL_LIBRARY})
else()
  find_package(OpenGL REQUIRED)
  set(OPENGL_LIBRARIES OpenGL::GL)
endif()

# FetchContent for dependencies
include(FetchContent)

# GLFW - Cross-platform window and input
FetchContent_Declare(glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
  GIT_SHALLOW TRUE
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM - Math library
FetchContent_Declare(glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# GLAD - OpenGL loader (pre-generated GL 3.3 Core from DawidLokiec/glad)
FetchContent_Declare(glad
  GIT_REPOSITORY https://github.com/DawidLokiec/glad.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
  FetchContent_Populate(glad)
endif()

set(GLAD_DIR "${glad_SOURCE_DIR}")
if(EXISTS "${GLAD_DIR}/src/glad.c")
  add_library(glad STATIC "${GLAD_DIR}/src/glad.c")
  target_include_directories(glad PUBLIC "${GLAD_DIR}/include")
  if(APPLE)
    target_compile_definitions(glad PRIVATE GLAD_GLAPI_EXPORT=)
  endif()
else()
  message(FATAL_ERROR
    "GLAD not found. Generate from https://gen.glad.sh/ (GL 3.3 Core, C, Loader)\n"
    "Extract to third_party/glad/ with include/ and src/ directories.")
endif()

# Main executable
add_executable(CPong
  CPong.cpp
  CPong.h
  src/Game.cpp
  src/Game.h
  src/Shader.cpp
  src/Shader.h
  src/Renderer.cpp
  src/Renderer.h
)

target_include_directories(CPong PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(CPong PRIVATE
  glfw
  glad
  ${OPENGL_LIBRARIES}
)

# GLM is header-only
target_include_directories(CPong PRIVATE ${glm_SOURCE_DIR})

# macOS: Add frameworks for GLFW
if(APPLE)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(IOKIT_LIBRARY IOKit REQUIRED)
  find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
  target_link_libraries(CPong PRIVATE ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()

# Copy shaders to build directory
add_custom_command(TARGET CPong POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/shaders
  $<TARGET_FILE_DIR:CPong>/shaders
)
