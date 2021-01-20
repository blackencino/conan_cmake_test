project(conan_test CXX)
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

add_library(conan_test_stuff
            imgui_backend/imgui_impl_glfw.cpp
            imgui_backend/imgui_impl_glfw.h
            imgui_backend/imgui_impl_opengl3.cpp
            imgui_backend/imgui_impl_opengl3.h)
target_link_libraries(conan_test_stuff PUBLIC ${CONAN_LIBS})

add_executable(conan_test example_glfw_opengl3.cpp)
target_link_libraries(conan_test PUBLIC conan_test_stuff)

