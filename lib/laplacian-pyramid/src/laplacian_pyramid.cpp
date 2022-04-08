#include <laplacian-pyramid/laplacian_pyramid.hpp>

laplacian::LaplacianPyramid::LaplacianPyramid(const cv::Mat& image,
                                              uint8_t compressions,
                                              float quantization) :
                                              _laplacianPlanes(),
                                              _kernel(kernel()){

    std::unordered_map<uint8_t, cv::Mat> gaussians = reduceToGaussians(image, _kernel, compressions);

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

std::unordered_map<uint8_t, cv::Mat> laplacian::LaplacianPyramid::reduceToGaussians(
        const cv::Mat& image,
        const cv::Mat& kernel,
        uint8_t compressions) {

    std::unordered_map<uint8_t, cv::Mat> gaussians;
    cv::Mat actual = image;
    for (uint8_t level = 0; level < compressions; level++) {

        actual = reduceGaussian(actual, kernel);
        gaussians.insert({level, actual});

        cv::Mat blured;
        actual.convertTo(blured, CV_8U);
        cv::imshow("Blured " + level, blured);
        cv::waitKey(0);
    }

    return gaussians;
}

cv::Mat laplacian::LaplacianPyramid::reduceGaussian(
        const cv::Mat& image,
        const cv::Mat& kernel) {

    int filteredRows = static_cast<int>(std::ceil((image.rows - (kernel.rows - 1)) / 2));
    int filteredColumns = static_cast<int>(std::ceil((image.cols - (kernel.cols - 1)) / 2));

    if (filteredRows <= 0 || filteredColumns <= 0) {
        // TODO: Throw Exception
    }

    cv::Mat filtered(filteredRows, filteredColumns, CV_32F);
    for (int i = 0; i < filteredRows; i++) {

        for(int j = 0; j < filteredColumns; j++) {

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