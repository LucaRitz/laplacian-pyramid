# Laplacian-Pyramid
This library implements the laplacian-pyramid concept described in the
paper "The Laplacian Pyramid as a Compact Image Code".

For usage have a look at [laplacian_pyramid_test.cpp](test/laplacian-pyramid/src/laplacian_pyramid_test.cpp) or on the below code snippet.

```cpp

#include <laplacian-pyramid/laplacian_pyramid.hpp>

int main(int argc, char* argv[]) {

    cv::Mat image = cv::imread("resources/lena.png", cv::IMREAD_GRAYSCALE);
    cv::imshow("Original", image);
    
    image.convertTo(image, CV_32F);
    
    auto pyramid = laplacian::LaplacianPyramid{image, 5}; // Pyramid creation
    auto decoded = pyramid.decode(); // Decoding into cv::Mat
    
    decoded.convertTo(decoded, CV_8U);
    cv::imshow("Decoded", decoded);
    cv::waitKey(0);

    return 0;
}
```

# Build and run
The library is built as a shared-lib with a minimum required CMake-Version of 3.15.1.<br>
Tested platforms:
- Windows 10 / MSVC 14.29.30133

## Recommended usage in cmake:
```cmake
include(FetchContent)

option(BUILD_TEST "" OFF)
option(BUILD_DOCUMENTATION "" OFF)
option(BUILD_CODE_DOCUMENTATION "" OFF)
option(BUILD_EXT_LIBS "" OFF)

FetchContent_Declare(
        laplacian_pyramid
        GIT_REPOSITORY https://github.com/LucaRitz/laplacian-pyramid
        GIT_TAG        main
)
FetchContent_MakeAvailable(laplacian_pyramid)

unset(BUILD_TEST)
unset(BUILD_DOCUMENTATION)
unset(BUILD_CODE_DOCUMENTATION)
unset(BUILD_EXT_LIBS)
```

For an example setup of a project, have a look at [laplacian-pyramid-test](https://github.com/LucaRitz/laplacian-pyramid-test)