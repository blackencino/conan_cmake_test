cmake_minimum_required(VERSION 3.8.0)
project(conan_cmake_test LANGUAGES CXX CUDA)

enable_language(CUDA)

set(CMAKE_CXX_STANDARD	17)				#	isn’t	part	of	the	projects	
set(CMAKE_CUDA_STANDARD	17)			#	export	information.
set(CMAKE_CXX_EXTENSIONS	OFF)	#	target_compile_features	are!
set(CMAKE_CUDA_EXTENSIONS	OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

#------------------------------------------------------------------------------
add_library(imgui_backend STATIC
            ${CMAKE_BINARY_DIR}/imgui_backend/imgui_impl_glfw.h
            ${CMAKE_BINARY_DIR}/imgui_backend/imgui_impl_opengl3.h
            ${CMAKE_BINARY_DIR}/imgui_backend/imgui_impl_opengl3.cpp            
            ${CMAKE_BINARY_DIR}/imgui_backend/imgui_impl_glfw.cpp)
target_link_libraries(imgui_backend 
                      PUBLIC
                      ${CONAN_LIBS})
target_include_directories(imgui_backend PUBLIC ${CMAKE_BINARY_DIR}/imgui_backend)

#------------------------------------------------------------------------------
add_library(cuda_support STATIC
            src/vecadd_tests_cuda.cuh
            src/vecadd_tests_cuda.cu)
target_link_libraries(cuda_support
                      PUBLIC
                      ${CONAN_LIBS})
set_target_properties(cuda_support 
                      PROPERTIES 
                      CUDA_ARCHITECTURES 75
                      CUDA_SEPARABLE_COMPILATION ON)

#------------------------------------------------------------------------------
add_library(graphics_utils STATIC
            src/graphics_utils/aabb.h
            src/graphics_utils/glm_common.h
            src/graphics_utils/camera.h
            src/graphics_utils/opengl_wrappers.h
            src/graphics_utils/opengl_program.h
            src/graphics_utils/std_shaders.h

            src/graphics_utils/camera.cpp
            src/graphics_utils/opengl_wrappers.cpp
            src/graphics_utils/opengl_program.cpp
            src/graphics_utils/std_shaders.cpp
            )
target_link_libraries(graphics_utils 
                      PUBLIC 
                      ${CONAN_LIBS})
target_include_directories(graphics_utils PUBLIC
                           ${CMAKE_SOURCE_DIR}/src)


#------------------------------------------------------------------------------
add_executable(opengl_cuda_test 
               src/graphics.h
               src/graphics.cpp
               src/main.cpp)
target_link_libraries(opengl_cuda_test 
                      PUBLIC
                      imgui_backend
                      cuda_support
                      graphics_utils
                      ${CONAN_LIBS})
set_target_properties(opengl_cuda_test 
                      PROPERTIES 
                      CUDA_ARCHITECTURES 75
                      CUDA_SEPARABLE_COMPILATION ON)


# Stuff for doing cuda correctly when you want to be able to
# have files not be compiled 
#  if(NOT CUDA_FOUND)
#    set_source_files_properties(${EXAMPLE_SOURCE_CUDA_FILES} PROPERTIES LANGUAGE
#                                                                        CXX)
#    if("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xClang"
#       OR "x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xAppleClang"
#       OR "x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xGNU"
#       OR "x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xIntel")
#      target_compile_options(${EXAMPLE_NAME} PRIVATE "-xc++")
#    endif()
#  endif()
#  if(NOT CUDA_FOUND)
#    set_target_properties(${EXAMPLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
#  endif()
