#include <thrust/host_vector.h>
#include <thrust/device_vector.h>


#include <iostream>
#include <cuda.h>

#define GLM_FORCE_CUDA
#define GLM_FORCE_ALIGNED_GENTYPES
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
// #define GLM_FORCE_CXX17
#include <glm/glm.hpp>

void do_vecadd_test() {

    std::cout << "CUDACC: " << CUDA_VERSION << std::endl;
    // H has storage for 4 integers
    thrust::host_vector<int> H(4);

    // initialize individual elements
    H[0] = 14;
    H[1] = 20;
    H[2] = 38;
    H[3] = 46;

    // H.size() returns the size of vector H
    std::cout << "H has size " << H.size() << std::endl;

    // print contents of H
    for (int i = 0; i < H.size(); i++)
        std::cout << "H[" << i << "] = " << H[i] << std::endl;

    // resize H
    H.resize(2);

    std::cout << "H now has size " << H.size() << std::endl;

    // Copy host_vector H to device_vector D
    thrust::device_vector<int> D = H;

    // elements of D can be modified
    D[0] = 99;
    D[1] = 88;

    // print contents of D
    for (int i = 0; i < D.size(); i++)
        std::cout << "D[" << i << "] = " << D[i] << std::endl;

    // H and D are automatically deleted when the function returns

    thrust::host_vector<glm::vec4> h_positions(4096);
    thrust::device_vector<glm::vec4> d_positions = h_positions; 


}
