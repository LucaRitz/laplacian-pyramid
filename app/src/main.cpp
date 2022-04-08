#include <laplacian-pyramid/laplacian_pyramid.hpp>

int main(int argc, char* argv[]) {

    cv::Mat image = cv::imread("resources/lena.png", cv::IMREAD_GRAYSCALE);
    image.convertTo(image, CV_32F);
    auto result = laplacian::LaplacianPyramid{image, 5};

    return 0;
}