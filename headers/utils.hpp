#include <iostream>
#include <complex>
#include <vector>
#include <random>


std::vector<float> generate_grayscale_colormap(int n_colors) {
    std::vector<float> colormap(n_colors * 3); // *3 for RGB
    for (int i = 0; i < n_colors; ++i) {
        float intensity = static_cast<float>(i) / (n_colors - 1);
        colormap[3 * i] = intensity;     // Red
        colormap[3 * i + 1] = intensity; // Green
        colormap[3 * i + 2] = intensity; // Blue
    }
    return colormap;
};

std::vector<float> generate_random_colormap(int n_colors) {
    std::vector<float> colormap(n_colors*3);

    const int fixed_seed = 12345;
    std::mt19937 eng(fixed_seed);
    std::uniform_real_distribution<> distr(0.0, 1.0);

    for (int i = 0; i < n_colors; i++) {
        float intensity = (static_cast<float>(i) * 2) / (n_colors - 1);
        colormap[3 * i] = distr(eng) * intensity;
        colormap[3 * i + 1] = distr(eng) * intensity;
        colormap[3 * i + 2] = distr(eng) * intensity;
    }

    return colormap;
}

std::vector<float> generate_smooth_colormap(int n_colors) {
    std::vector<float> colormap(n_colors * 3);

    // Define start and end colors for the gradient
    std::vector<float> startColor = {0.0f, 0.5f, 1.0}; // Blue
    std::vector<float> endColor = {1.0f, 0.5f, 0.0f}; // Red

    for (int i = 0; i < n_colors; i++) {
        float ratio = static_cast<float>(i) / (n_colors - 1);
        // Interpolate between startColor and endColor
        colormap[3 * i] = startColor[0] + ratio * (endColor[0] - startColor[0]); // Red
        colormap[3 * i + 1] = startColor[1] + ratio * (endColor[1] - startColor[1]); // Green
        colormap[3 * i + 2] = startColor[2] + ratio * (endColor[2] - startColor[2]); // Blue
    }

    return colormap;
}

std::vector<float> gen_complex_set_2_shader(int width, int height, float real_min, float real_max, float imag_min, float imag_max) {
    std::vector<float> complex_set(width * height * 2); // *2 because we store two floats for each pixel
    float delta_real = (real_max - real_min) / width;
    float delta_imag = (imag_max - imag_min) / height;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            complex_set[2 * (y * width + x)] = real_min + x * delta_real; // Real part
            complex_set[2 * (y * width + x) + 1] = imag_min + y * delta_imag; // Imaginary part
        }
    }
    return complex_set;
};








