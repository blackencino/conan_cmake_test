#pragma once

#ifdef __CUDACC__
#include <cuda.h>
#define GLM_FORCE_CUDA
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_ALIGNED_GENTYPES
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
// #define GLM_FORCE_CXX17
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

