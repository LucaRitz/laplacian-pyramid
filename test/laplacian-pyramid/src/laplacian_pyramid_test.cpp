#include <gtest/gtest.h>
#include <laplacian-pyramid/laplacian_pyramid.hpp>
#include <chrono>
#include <functional>
#include <string>

namespace laplacian::test {

    template<class R>
    R measured(const std::function<R ()>& function, const std::string& step = "");
}

TEST(LaplacianPyramid, should_display_decoded_image_if_image_is_grayscale) {

    for (int i = 0; i < 100; i++) {
        cv::Mat image = cv::imread("resources/lena.png", cv::IMREAD_GRAYSCALE);
        image.convertTo(image, CV_32F);

        auto pyramid = laplacian::test::measured<laplacian::LaplacianPyramid>(
                [&image]() -> laplacian::LaplacianPyramid { return laplacian::LaplacianPyramid{image, 5}; },
                "Laplace-Pyramid creation");

        auto decoded = laplacian::test::measured<cv::Mat>(
                [&pyramid]() -> cv::Mat { return pyramid.decode(); }, "Laplace-Pyramid decode");
        decoded.convertTo(decoded, CV_8U);

        if (i == 99) {
            image.convertTo(image, CV_8U);
            cv::resize(decoded, decoded, cv::Size{image.rows, image.cols});
            cv::Mat difference = image - decoded;
            cv::imshow("Original", image);
            cv::imshow("Decoded", decoded);
            cv::imshow("Difference", difference);
            cv::waitKey(0);
        }
    }
}

TEST(LaplacianPyramid, should_display_decoded_image_if_image_is_color) {

    cv::Mat image = cv::imread("resources/lena.png", cv::IMREAD_COLOR);
    image.convertTo(image, CV_32F);

    cv::Mat bgr[3];
    cv::split(image, bgr);

    auto bluePyramid = laplacian::test::measured<laplacian::LaplacianPyramid>(
            [&bgr]() -> laplacian::LaplacianPyramid { return laplacian::LaplacianPyramid{bgr[0], 5}; },
            "Laplace-Pyramid blue creation");
    auto greenPyramid = laplacian::test::measured<laplacian::LaplacianPyramid>(
            [&bgr]() -> laplacian::LaplacianPyramid { return laplacian::LaplacianPyramid{bgr[1], 5}; },
            "Laplace-Pyramid green creation");
    auto redPyramid = laplacian::test::measured<laplacian::LaplacianPyramid>(
            [&bgr]() -> laplacian::LaplacianPyramid { return laplacian::LaplacianPyramid{bgr[2], 5}; },
            "Laplace-Pyramid red creation");

    auto decodedBlue = laplacian::test::measured<cv::Mat>(
            [&bluePyramid]() -> cv::Mat { return bluePyramid.decode(); }, "Laplace-Pyramid blue decode");
    auto decodedGreen = laplacian::test::measured<cv::Mat>(
            [&greenPyramid]() -> cv::Mat { return greenPyramid.decode(); }, "Laplace-Pyramid green decode");
    auto decodedRed = laplacian::test::measured<cv::Mat>(
            [&redPyramid]() -> cv::Mat { return redPyramid.decode(); }, "Laplace-Pyramid red decode");
    cv::Mat decoded;
    bgr[0] = decodedBlue;
    bgr[1] = decodedGreen;
    bgr[2] = decodedRed;
    cv::merge(bgr, 3, decoded);
    decoded.convertTo(decoded, CV_8U);

    image.convertTo(image, CV_8U);
    cv::resize(decoded, decoded, cv::Size{image.rows, image.cols});
    cv::Mat difference = image - decoded;
    cv::imshow("Original", image);
    cv::imshow("Decoded", decoded);
    cv::imshow("Difference", difference);
    cv::waitKey(0);
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