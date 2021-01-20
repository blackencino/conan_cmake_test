project(conan_cmake_test CXX)
cmake_minimum_required(VERSION 2.8.12)

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.15/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP
                BUILD missing)

add_library(imgui_backend
            ${CMAKE_BINARY_DIR}/imgui_backend/imgui_impl_glfw.cpp
            ${CMAKE_BINARY_DIR}/imgui_backend/imgui_impl_glfw.h
            ${CMAKE_BINARY_DIR}/imgui_backend/imgui_impl_opengl3.cpp
            ${CMAKE_BINARY_DIR}/imgui_backend/imgui_impl_opengl3.h)
target_link_libraries(imgui_backend PUBLIC ${CONAN_LIBS})
target_include_directories(imgui_backend PUBLIC ${CMAKE_BINARY_DIR}/imgui_backend)

add_executable(conan_cmake_test main.cpp)
target_link_libraries(conan_cmake_test PUBLIC imgui_backend)

