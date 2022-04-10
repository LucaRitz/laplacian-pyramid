#include <laplacian-pyramid/laplacian_pyramid.hpp>

laplacian::LaplacianPyramidException::LaplacianPyramidException(const std::string& message) :
    _message(message) {
}

std::string laplacian::LaplacianPyramidException::getMessage() const {

    return _message;
}

laplacian::LaplacianPyramid::LaplacianPyramid(const cv::Mat& image,
                                              uint8_t compressions,
                                              float quantization) :
                                              _laplacianPlanes(),
                                              _kernel(kernel()){

    const cv::Mat scaledImage = applyValidScaling(image, compressions);
    std::vector<cv::Mat> gaussians = reduceToGaussians(scaledImage, _kernel, compressions);

}

cv::Mat laplacian::LaplacianPyramid::decode() const {

    return cv::Mat{};
}

cv::Mat laplacian::LaplacianPyramid::at(uint8_t level) const {

    return cv::Mat{};
}

cv::Mat laplacian::LaplacianPyramid::operator[](uint8_t level) const {

    return cv::Mat{};
}

////////////////////////////////////////
// PRIVATE
////////////////////////////////////////

cv::Mat laplacian::LaplacianPyramid::applyValidScaling(const cv::Mat& image, uint8_t compressions) const {

    cv::Mat validScaling = image;
    bool widthValid = isValidScaling(validScaling.cols, compressions);
    bool heightValid = isValidScaling(validScaling.rows, compressions);

    while(!widthValid || !heightValid) {

        if (validScaling.rows <= 1 || validScaling.cols <= 1) {
            throw LaplacianPyramidException{"The expected scaling cannot be applied because the original image is too small!"};
        }

        widthValid = widthValid || isValidScaling((validScaling = removeLastColumn(validScaling)).cols, compressions);
        heightValid = heightValid || isValidScaling((validScaling = removeLastRow(validScaling)).rows, compressions);
    }

    return validScaling;
}

cv::Mat laplacian::LaplacianPyramid::removeLastColumn(const cv::Mat& image) const {

    return cutImage(image, image.rows, image.cols - 1);
}

cv::Mat laplacian::LaplacianPyramid::removeLastRow(const cv::Mat& image) const {

    return cutImage(image, image.rows - 1, image.cols);
}

cv::Mat laplacian::LaplacianPyramid::cutImage(const cv::Mat& image, int rows, int cols) const {

    const cv::Rect subImage( 0, 0, cols, rows);
    return image(subImage);
}

bool laplacian::LaplacianPyramid::isValidScaling(int dimension, uint8_t compressions) const {

    // The formula 'C = M_c * 2^N + 1' of the paper "The laplacian pyramid as a Compact Image Code" is wrong.
    // It should be 'C = M_c * 2^N - 1'. This results in '(C + 1) / 2^N = M_c
    // Because the indexes in the paper starts with -2, formula is moved by +2.
    // This results in the final formula '(C + 3) / 2^N = M_c
    return isInteger((static_cast<float>(dimension) + 3.0f) / static_cast<float>(std::pow(2, compressions)));
}

bool laplacian::LaplacianPyramid::isInteger(float value) const {

    return isNearlyEqual(std::floor(value), value);
}

bool laplacian::LaplacianPyramid::isNearlyEqual(float value1, float value2) const {

    static const float epsilon = 1e-5;
    return std::abs(value1 - value2) <= epsilon * std::abs(value1);
}

cv::Mat laplacian::LaplacianPyramid::kernel(float a) const {
    cv::Mat kernel(5, 1, CV_32F);

    float zeroAndFour = 0.25f - a / 2.0f;
    kernel.at<float>(0,0) = zeroAndFour;
    kernel.at<float>(1,0) = 0.25f;
    kernel.at<float>(2,0) = a;
    kernel.at<float>(3,0) = 0.25f;
    kernel.at<float>(4,0) = zeroAndFour;

    cv::Mat transposedKernel;
    cv::transpose(kernel, transposedKernel);

    return kernel * transposedKernel;
}

std::vector<cv::Mat> laplacian::LaplacianPyramid::reduceToGaussians(
        const cv::Mat& image,
        const cv::Mat& kernel,
        uint8_t compressions) const {

    std::vector<cv::Mat> gaussians;
    gaussians.push_back(image);
    cv::Mat actual = image;

    const double Mc = (static_cast<float>(actual.cols) - 1.0f) / std::pow(2.0f, compressions);
    const double Mr = (static_cast<float>(actual.rows) - 1.0f) / std::pow(2.0f, compressions);

    for (uint8_t level = 1; level < compressions; level++) {

        actual = reduceGaussian(actual, kernel,
                                static_cast<int>(Mr * std::pow(2, compressions - level) - 3),
                                static_cast<int>(Mc * std::pow(2, compressions - level)  - 3));
        gaussians.push_back(actual);

        cv::Mat blured;
        actual.convertTo(blured, CV_8U);
        cv::imshow("Blured " + level, blured);
        cv::waitKey(0);
    }

    return gaussians;
}

cv::Mat laplacian::LaplacianPyramid::reduceGaussian(
        const cv::Mat& image,
        const cv::Mat& kernel,
        int rows,
        int columns) const {

    cv::Mat filtered(rows, columns, CV_32F);
    for (int i = 0; i < rows; i++) {

        for(int j = 0; j < columns; j++) {

            float value = 0.0f;
            for(int m = 0; m < kernel.rows; m++) {

                for(int n = 0; n < kernel.cols; n++) {
                    value += kernel.at<float>(m, n) * image.at<float>(i * 2 + m, j * 2 + n);
                }
            }
            filtered.at<float>(i, j) = value;

        }
    }
    return filtered;
}