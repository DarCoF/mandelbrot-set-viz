#include <iostream>
#include <complex>
#include <vector>
#include <random>


std::vector<double> generate_grayscale_colormap(int n_colors) {
    std::vector<double> colormap(n_colors * 3); // *3 for RGB
    for (int i = 0; i < n_colors; ++i) {
        double intensity = static_cast<double>(i) / (n_colors - 1);
        colormap[3 * i] = intensity;     // Red
        colormap[3 * i + 1] = intensity; // Green
        colormap[3 * i + 2] = intensity; // Blue
    }
    return colormap;
};

std::vector<double> generate_random_colormap(int n_colors) {
    std::vector<double> colormap(n_colors*3);

    const int fixed_seed = 12345;
    std::mt19937 eng(fixed_seed);
    std::uniform_real_distribution<> distr(0.0, 1.0);

    for (int i = 0; i < n_colors; i++) {
        double intensity = (static_cast<double>(i) * 2) / (n_colors - 1);
        colormap[3 * i] = distr(eng) * intensity;
        colormap[3 * i + 1] = distr(eng) * intensity;
        colormap[3 * i + 2] = distr(eng) * intensity;
    }

    return colormap;
}

std::vector<double> generate_smooth_colormap(int n_colors) {
    std::vector<double> colormap(n_colors * 3);

    // Define start and end colors for the gradient
    std::vector<double> startColor = {0.0f, 0.5f, 1.0}; // Blue
    std::vector<double> endColor = {1.0f, 0.5f, 0.0f}; // Red

    for (int i = 0; i < n_colors; i++) {
        double ratio = static_cast<double>(i) / (n_colors - 1);
        // Interpolate between startColor and endColor
        colormap[3 * i] = startColor[0] + ratio * (endColor[0] - startColor[0]); // Red
        colormap[3 * i + 1] = startColor[1] + ratio * (endColor[1] - startColor[1]); // Green
        colormap[3 * i + 2] = startColor[2] + ratio * (endColor[2] - startColor[2]); // Blue
    }

    return colormap;
}

std::vector<double> gen_complex_set_2_shader(int width, int height, double real_min, double real_max, double imag_min, double imag_max) {
    std::vector<double> complex_set(width * height * 2); // *2 because we store two floats for each pixel
    double delta_real = (real_max - real_min) / width;
    double delta_imag = (imag_max - imag_min) / height;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            complex_set[2 * (y * width + x)] = real_min + x * delta_real; // Real part
            complex_set[2 * (y * width + x) + 1] = imag_min + y * delta_imag; // Imaginary part
        }
    }
    return complex_set;
};








