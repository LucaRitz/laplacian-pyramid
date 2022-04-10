#include <laplacian-pyramid/laplacian_pyramid.hpp>

int main(int argc, char* argv[]) {

    cv::Mat image = cv::imread("resources/lena.png", cv::IMREAD_GRAYSCALE);
    {
        cv::imshow("Original", image);
    }
    image.convertTo(image, CV_32F);
    auto pyramid = laplacian::LaplacianPyramid{image, 5};

    {
        auto decoded = pyramid.decode();
        decoded.convertTo(decoded, CV_8U);
        cv::imshow("Decoded", decoded);
        cv::waitKey(0);
    }

    return 0;
}