#pragma once

#include "macro_definition.hpp"

#include <opencv2/opencv.hpp>
#include <unordered_map>

namespace laplacian {

    const uint8_t DEFAULT_COMPRESSIONS = 5;
    const float DEFAULT_QUANTIZATION = 1.0f;
    const float DEFAULT_A = 1.0f;

    /**
     * Generates a type.
     *
     * @param value The value to take.
     *
     */
    class EXPORT_LAPLACIAN_PYRAMID LaplacianPyramid {
    public:

        explicit LaplacianPyramid(const cv::Mat& image,
                                  uint8_t compressions = DEFAULT_COMPRESSIONS,
                                  float quantization = DEFAULT_QUANTIZATION);

        [[nodiscard]] cv::Mat decode() const;
        [[nodiscard]] cv::Mat at(uint8_t level) const;

        [[nodiscard]] cv::Mat operator[](uint8_t level) const;

    private:
        std::unordered_map<uint8_t, cv::Mat> _laplacianPlanes;
        const cv::Mat _kernel;

        cv::Mat kernel(float a = DEFAULT_A) const;
        std::unordered_map<uint8_t, cv::Mat> reduceToGaussians(const cv::Mat& image,
                                                             const cv::Mat& kernel,
                                                             uint8_t compressions);
        cv::Mat reduceGaussian(const cv::Mat& image,
                               const cv::Mat& kernel);
    };
}