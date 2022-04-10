#pragma once

#include "macro_definition.hpp"

#include <opencv2/opencv.hpp>
#include <vector>

namespace laplacian {

    const uint8_t DEFAULT_COMPRESSIONS = 5;
    const float DEFAULT_QUANTIZATION = 1.0f;
    const float DEFAULT_A = 1.0f;

    class EXPORT_LAPLACIAN_PYRAMID LaplacianPyramidException {
    public:
        explicit LaplacianPyramidException(const std::string& message = "");

        [[nodiscard]] std::string getMessage() const;

    private:
        std::string _message;
    };

    class EXPORT_LAPLACIAN_PYRAMID LaplacianPyramid {
    public:

        /**
         *
         * Creates a laplacian pyramid for the given image with the expected compression levels and quantization.
         * The image has to be single channeled and CV_32F encoded.
         *
         * @param image The image to encode.
         * @param compressions The compression levels.
         * @param quantization The quantization used for the reduction of entropy.
         */
        explicit LaplacianPyramid(const cv::Mat& image,
                                  uint8_t compressions = DEFAULT_COMPRESSIONS,
                                  float quantization = DEFAULT_QUANTIZATION);

        /**
         *
         * Decodes the pyramid into the original image.
         *
         * @return The image resulting from the decoding process.
         *
         */
        [[nodiscard]] cv::Mat decode() const;

        /**
         *
         * Gives an encoded laplacian image at the expected level.
         *
         * @param level The compression level of the expected laplacian image.
         *
         * @return The laplacian image at the given level.
         */
        [[nodiscard]] cv::Mat at(uint8_t level) const;

        /**
         *
         * Gives an encoded laplacian image at the expected level.
         *
         * @param level The compression level of the expected laplacian image.
         *
         * @return The laplacian image at the given level.
         */
        [[nodiscard]] cv::Mat operator[](uint8_t level) const;

    private:
        std::unordered_map<uint8_t, cv::Mat> _laplacianPlanes;
        cv::Mat _kernel;

        /**
         *
         * Validates the given image and applies a valid scaling to perform the fast formulas.
         * The resulting image shares the same memory with the given image.
         * If the image cannot be scaled down by the expected compressions, a #laplacian::LaplacianPyramidException is thrown.
         *
         * @param image The image to validate and scale.
         * @param compressions The compressions or levels of the pyramid.
         *
         * @return The scaled image.
         */
        [[nodiscard]] cv::Mat applyValidScaling(const cv::Mat& image, uint8_t compressions) const;

        /**
         *
         * Removes the last column of the given image.
         * The resulting image shares the same memory with the original image.
         *
         * @param image The original image.
         *
         * @return The image without the last column.
         */
        [[nodiscard]] cv::Mat removeLastColumn(const cv::Mat& image) const;

        /**
         *
         * Removes the last row of the given image.
         * The resulting image shares the same memory with the original image.
         *
         * @param image The original image.
         *
         * @return The image without the last row.
         */
        [[nodiscard]] cv::Mat removeLastRow(const cv::Mat& image) const;

        /**
         *
         * Cuts the given image to the size of the given rows and columns starting on the left upper corner (0, 0).
         *
         * @param image The image which is to cut.
         * @param rows The expected rows of the cut image.
         * @param cols The expected columns of the cut image.
         *
         * @return The cut image sharing the same memory with the original image.
         *
         */
        [[nodiscard]] cv::Mat cutImage(const cv::Mat& image, int rows, int cols) const;

        /**
         *
         * Checks if a valid scaling is applied.
         * The formula is taken from the paper "The Laplacian Pyramid as a Compact Image Code". The presented formula
         * may be wrong, for the implementation the followin is used: M_c = (C + 1) / 2^N.
         * Because the matrices in the paper start with an index of -2, the corresponding part of the formula is moved
         * by +2. This results in the final formula: M_c = (C + 3) / 2^N.
         *
         * The dimension is valid when the above formula results in an integer value for M_c.
         *
         * @param dimension The dimension "C" which is to be checked to validity
         * @param compressions The levels of the pyramid. This refers to "N" in the above formula.
         *
         * @return Gives true, when the formula gets an integer value for M_c
         */
        [[nodiscard]] bool isValidScaling(int dimension, uint8_t compressions) const;

        /**
         *
         * Checks if a given value is an integer type. It gets true, when the integer is nearly int.
         *
         * @param value The value to check.
         * @return Gets true, if the value is of type int or nearly of type int.
         */
        [[nodiscard]] bool isInteger(float value) const;

        /**
         *
         * Checks if two types are nearly the same.
         *
         * @param value1 The first value
         * @param value2 The last value
         * @return Gets true, when the first and the last value are nearly the same.
         */
        [[nodiscard]] bool isNearlyEqual(float value1, float value2) const;

        /**
         *
         * Gets the default kernel "w" presented in the paper "The Laplacian Pyramid as a Compact Image Code".
         * The kernel has the following properties: w(2) = a, w(0) = w(4) = 1/4 - a/2, w(1) = w(3) = 1/4
         * Note that the indexes are moved by +2.
         *
         * This one dimensional kernel "w" is transposed and multiplied by itself to generate the 2D kernel matrix.
         *
         * @param a A value to modify the kernel
         * @return The 2D kernel described above
         */
        [[nodiscard]] cv::Mat kernel(float a = DEFAULT_A) const;

        /**
         *
         * Reduces the given image with the given kernel by the given compressions and gets the results in a vector.
         *
         * @param image The image to encode
         * @param kernel The kernel for encoding
         * @param compressions The expected compression levels.
         * @return A vector of the reduced images.
         */
        [[nodiscard]] std::vector<cv::Mat> reduceToGaussians(const cv::Mat& image,
                                                             const cv::Mat& kernel,
                                                             uint8_t compressions) const;

        /**
         *
         * Reduces an image with the given kernel to the expected size (rows, columns).
         *
         * @param image The image to reduce.
         * @param kernel The kernel used for reduction.
         * @param rows The expected rows of the reduced image.
         * @param columns The expected columns of the reduced image.
         * @return The reduced image.
         */
        [[nodiscard]] cv::Mat reduceGaussian(const cv::Mat& image,
                               const cv::Mat& kernel,
                               int rows,
                               int columns) const;
    };
}