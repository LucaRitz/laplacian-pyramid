#include <gtest/gtest.h>
#include <laplacian-pyramid/laplacian_pyramid.hpp>
#include <chrono>
#include <functional>
#include <string>

namespace laplacian::test {

    template<class R>
    R measured(const std::function<R ()>& function, const std::string& step = "");
}

TEST(LaplacianPyramidTest, should_display_decoded_image_if_image_is_grayscale) {

    cv::Mat image = cv::imread("resources/lena.png", cv::IMREAD_GRAYSCALE);
    cv::imshow("Original", image);

    image.convertTo(image, CV_32F);

    auto pyramid = laplacian::test::measured<laplacian::LaplacianPyramid>(
            [&image]() -> laplacian::LaplacianPyramid { return laplacian::LaplacianPyramid{image, 5}; },
            "Laplace-Pyramid creation");

    auto decoded = laplacian::test::measured<cv::Mat>(
            [&pyramid]() -> cv::Mat { return pyramid.decode(); }, "Laplace-Pyramid decode");
    decoded.convertTo(decoded, CV_8U);
    cv::imshow("Decoded", decoded);
    cv::waitKey(0);
}

TEST(LaplacianPyramid, should_display_decoded_image_if_image_is_color) {

    // TODO: Implement
}

TEST(LaplacianPyramid, shold_display_decoded_image_if_image_is_quantized) {

    // TODO: Implement
}

template<class R>
R laplacian::test::measured(const std::function<R ()>& function, const std::string& step) {

    auto start = std::chrono::high_resolution_clock::now();
    auto result = function();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << (step.empty() ? "Measured step" : step) << " took " << std::to_string(duration.count()) << " ms" << std::endl;

    return result;
}