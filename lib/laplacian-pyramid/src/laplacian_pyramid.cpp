#include <laplacian-pyramid/laplacian_pyramid.hpp>
#include <algorithm>

laplacian::LaplacianPyramidException::LaplacianPyramidException(const std::string& message) :
    std::exception(message.c_str()) {
}

laplacian::LaplacianPyramid::LaplacianPyramid(const cv::Mat& image,
                                              uint8_t compressions,
                                              float quantization) :
                                              _laplacianPlanesQuantized(),
                                              _kernel(kernel()){

    const auto scaledImage = applyValidScaling(image, compressions);
    const auto gaussians = reduceToGaussians(scaledImage, _kernel, compressions);
    const auto upsampledGaussians = upsample(gaussians, _kernel);
    const auto laplacianPlanes = buildLaplacianPlanes(gaussians, upsampledGaussians);
    _laplacianPlanesQuantized = quantization == 0 ? laplacianPlanes : quantize(laplacianPlanes, quantization);
}

cv::Mat laplacian::LaplacianPyramid::decode() const {

    cv::Mat reconstructed = _laplacianPlanesQuantized.at(levels() - 1);

    for (int level = levels() - 2; level >= 0; level--) {

        const auto& laplacian = _laplacianPlanesQuantized.at(level);
        const auto upsampled = upsample(reconstructed, laplacian.rows, laplacian.cols, _kernel);

        reconstructed = laplacian + upsampled;
    }

    return reconstructed;
}

cv::Mat laplacian::LaplacianPyramid::at(uint8_t level) const {

    return _laplacianPlanesQuantized.at(level);
}

cv::Mat laplacian::LaplacianPyramid::operator[](uint8_t level) const {

    return at(level);
}

uint8_t laplacian::LaplacianPyramid::levels() const {

    return _laplacianPlanesQuantized.size();
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
    }

    return gaussians;
}

cv::Mat laplacian::LaplacianPyramid::reduceGaussian(
        const cv::Mat& image,
        const cv::Mat& kernel,
        int rows,
        int columns) const {

    cv::Mat filtered(rows, columns, CV_32F);
    int kernelHalfRows = (kernel.rows / 2);
    int kernelHalfCols = (kernel.cols / 2);

    for (int i = 0; i < rows; i++) {

        for(int j = 0; j < columns; j++) {

            float value = 0.0f;
            for(int m = -kernelHalfRows; m <= kernelHalfRows; m++) {

                int row = (i << 1) - m;
                for(int n = -kernelHalfCols; n <= kernelHalfCols; n++) {

                    int col = (j << 1) - n;

                    if (row >= 0 && col >= 0) {

                        row = std::min(row, image.rows - 1);
                        col = std::min(col, image.cols - 1);
                        value += kernel.at<float>(m + kernelHalfRows, n + kernelHalfCols) * image.at<float>(row, col);
                    }
                }
            }
            filtered.at<float>(i, j) = value;

        }
    }

    return filtered;
}

std::vector<cv::Mat> laplacian::LaplacianPyramid::upsample(const std::vector<cv::Mat>& images,
                                                           const cv::Mat& kernel) const {

    std::vector<cv::Mat> upsampled;

    for (int level = 1; level < images.size(); level++) {

        const auto& imageOfUpperLevel = images.at(level - 1);

        upsampled.emplace_back(upsample(images.at(level), imageOfUpperLevel.rows, imageOfUpperLevel.cols, kernel));
    }

    return upsampled;
}

cv::Mat laplacian::LaplacianPyramid::upsample(const cv::Mat& image, int rows, int cols, const cv::Mat& kernel) const {

    cv::Mat upsampled(rows, cols, CV_32F);

    int kernelHalfRows = (kernel.rows / 2);
    int kernelHalfCols = (kernel.cols / 2);

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            float value = 0.0f;
            for(int m = -kernelHalfRows; m <= kernelHalfRows; m++) {

                float row = static_cast<float>((i - m)) / 2.0f;
                for(int n = -kernelHalfCols; n <= kernelHalfCols; n++) {

                    float col = static_cast<float>((j - n)) / 2.0f;

                    if (isInteger(row) && row >= 0 &&
                        isInteger(col) && col >= 0) {

                        int rowI = std::min(static_cast<int>(row), image.rows - 1);
                        int colI = std::min(static_cast<int>(col), image.cols - 1);

                        value += kernel.at<float>(m + kernelHalfRows, n + kernelHalfCols) * image.at<float>(rowI, colI);
                    }
                }
            }
            upsampled.at<float>(i, j) = 4 * value;
        }
    }

    return upsampled;
}

std::vector<cv::Mat> laplacian::LaplacianPyramid::buildLaplacianPlanes(const std::vector<cv::Mat>& gaussians,
                                          const std::vector<cv::Mat>& upsampled) {

    if (gaussians.size() != (upsampled.size() + 1)) {
        throw LaplacianPyramidException("The gaussian vector has to have one more image than the upsampled!");
    }

    std::vector<cv::Mat> laplacian;

    for (int level = 0; level <= gaussians.size() - 2; level++) {

        laplacian.emplace_back(gaussians.at(level) - upsampled.at(level));
    }
    laplacian.push_back(gaussians.at(gaussians.size() - 1));

    return laplacian;
}

std::vector<cv::Mat> laplacian::LaplacianPyramid::quantize(const std::vector<cv::Mat>& laplacianPlanes,
                              float quantization) const {

    // TODO: Implement uniform quantization

    return laplacianPlanes;
}
