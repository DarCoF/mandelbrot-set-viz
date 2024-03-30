cmake_minimum_required(VERSION 3.17)
project(fractal_shader)

set(CMAKE_CXX_STANDARD 20)

# Define the executable
add_executable(
    fractal_shader
    src/main_opengl.cpp
    headers/event_manager.hpp
    headers/utils_shader.hpp
    headers/utils.hpp)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "C:/Users/dario/Desktop/Video-18/vcpkg/packages/glew_x64-windows/share/glew/vcpkg-cmake-wrapper.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

set(SFML_ROOT c:/SFML)

# Find required packages
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics network audio)
find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)
# Manually set the GLEW include directory and libraries
set(GLEW_INCLUDE_DIRS C:/Users/dario/Desktop/Video-18/vcpkg/installed/x64-windows/include)
set(GLEW_LIBRARIES C:/Users/dario/Desktop/Video-18/vcpkg/installed/x64-windows/lib/glew32.lib)

# Link libraries to the executable
if (SFML_FOUND)
    target_include_directories(fractal_shader PRIVATE ${GLEW_INCLUDE_DIRS} ${SFML_INCLUDE_DIR})
    target_link_libraries(fractal_shader PRIVATE ${GLEW_LIBRARIES} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} OpenGL::GL)
endif()

# Copy assets to the binary directory after build
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Copy dlls to build
if(WIN32)
    file(GLOB BINARY_DEP_DLLS "${SFML_INCLUDE_DIR}/../bin/*.dll")
    file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
    file(GLOB MINGW_DEP_DLLS "C:/mingw64/bin/*.dll")
    file(COPY ${MINGW_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
endif()





