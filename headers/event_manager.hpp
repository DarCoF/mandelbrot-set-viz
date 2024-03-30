#include <complex>
#include <vector>
#include <iostream>
// SFML headers for windowing, input and OpenGL
#include <SFML/Graphics.hpp>


void complex_set_adjust_real(std::vector<float> &complex_set, const float &real_delta) {
    int n_elems = static_cast<int>(complex_set.size());
    for (int i = 0; i < n_elems; i += 2) {
        complex_set[i] += real_delta;
    }
}


void complex_set_adjust_imag(std::vector<float> &complex_set, const float &imag_delta) {
    int n_elems = static_cast<int>(complex_set.size());
    for (int i = 1; i < n_elems; i += 2) {
        complex_set[i] += imag_delta;
    }
}

void complex_set_adjust_scale_centered(std::vector<float> &complex_set, float scale) {
    if (complex_set.empty()) {
        std::cerr << "Warning: Input vector is empty." << std::endl;
        return;
    }

    // Assuming the first two values are the min real and min imaginary parts, 
    // and the last two values are the max real and max imaginary parts.
    float rmin = complex_set.at(0);
    float imin = complex_set.at(1);
    float rmax = complex_set.at(complex_set.size() - 4);
    float imax = complex_set.at(complex_set.size() - 3);

    float r_center = (rmin + rmax) / 2;
    float i_center = (imin + imax) / 2;

    for (size_t i = 0; i < complex_set.size(); i += 2) {
        // Adjust real part
        complex_set[i] = ((complex_set[i] - r_center) * scale) + r_center;

        // Adjust imaginary part
        complex_set[i + 1] = ((complex_set[i + 1] - i_center) * scale) + i_center;
    }
}

void complex_set_adjust_view(std::vector<float> &complex_set, const std::complex<float>& newCenter, float scaleFactor, const sf::Vector2u& windowSize) {
    if (complex_set.empty()) {
        std::cerr << "Warning: Input vector is empty." << std::endl;
        return;
    }

    // Directly use the first and last values in complex_set as min and max for real and imaginary parts
    float realMin = complex_set[0];
    float imagMin = complex_set[1];
    float realMax = complex_set[complex_set.size() - 2];
    float imagMax = complex_set[complex_set.size() - 1];

    float viewWidth = realMax - realMin;
    float viewHeight = imagMax - imagMin;

    // Adjust the viewWidth and viewHeight based on scaleFactor
    viewWidth /= scaleFactor;
    viewHeight /= scaleFactor;

    // Compute the new min and max for real and imaginary parts based on the new center and adjusted view dimensions
    realMin = newCenter.real() - viewWidth / 2.0f;
    realMax = newCenter.real() + viewWidth / 2.0f;
    imagMin = newCenter.imag() - viewHeight / 2.0f;
    imagMax = newCenter.imag() + viewHeight / 2.0f;

    // Recalculate complex_set points
    for (size_t i = 0, index = 0; i < complex_set.size(); i += 2, ++index) {
        float normalizedX = static_cast<float>(index % windowSize.x) / (windowSize.x - 1);
        float normalizedY = static_cast<float>(index / windowSize.x) / (windowSize.y - 1);
        
        complex_set[i] = realMin + normalizedX * (realMax - realMin); // Real part
        complex_set[i + 1] = imagMin + normalizedY * (imagMax - imagMin); // Imaginary part
    }
}

std::complex<float> screenToComplex(const sf::Vector2i& pixelPos, const std::vector<float>& complex_set, const sf::Vector2u& windowSize) {
    // Directly use the first and last values in complex_set as min and max for real and imaginary parts
    float realMin = complex_set[0];
    float imagMin = complex_set[1];
    float realMax = complex_set[complex_set.size() - 2];
    float imagMax = complex_set[complex_set.size() - 1];

    // Calculate view width and height based on the real and imaginary ranges
    float viewWidth = realMax - realMin;
    float viewHeight = imagMax - imagMin;

    // Map pixel position to complex plane coordinates
    float real = realMin + (pixelPos.x / static_cast<float>(windowSize.x)) * viewWidth;
    float imag = imagMax - (pixelPos.y / static_cast<float>(windowSize.y)) * viewHeight; // Subtract from imagMax for correct y-axis orientation

    return std::complex<float>(real, imag);
}

// Event manager for user input control
class MandelbrotEventManager {
public:
    MandelbrotEventManager(float initialZoom, const std::complex<float>& initialCenter)
        : zoom(initialZoom), center(initialCenter) {}

    float real_delta = 0.025f;
    float imag_delta = 0.025f;
    float real_delta_ = real_delta;
    float imag_delta_ = imag_delta;
    
    // Process events, adjust zoom and center based on input
    bool handleEvents(sf::RenderWindow& window, std::vector<float> &complex_set) {
        std::cout << "Event" << std::endl;
        bool needRedraw = false;
        sf::Event event;
        while (window.pollEvent(event)) {
            std::cout << "Inside PollEvent" << std::endl;
            if (event.type == sf::Event::Closed) {
                std::cout << "Event window closed" << std::endl;
                window.close();
            } else {
                std::cout << "Event other" << std::endl;
                needRedraw = handleZoomAndPan(event, complex_set, window);

            }
        }
        return needRedraw;
    }

    float getZoom() const { return zoom; }
    std::complex<float> getCenter() const { return center; }

private:
    float zoom;
    std::complex<float> center;

    bool handleZoomAndPan(const sf::Event& event, std::vector<float> &complex_set, sf::RenderWindow& window) {
        std::cout << "Inside Event Handler" << std::endl;
        bool needRedraw = false;
        float panSpeed = 0.025 * std::abs(1.0 / getZoom()); // Adjust pan speed based on zoom level
        if (event.type == sf::Event::MouseWheelScrolled) {
            // // Adjust zoom factor based on scroll direction
            float scaleFactor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
            zoom *= scaleFactor;
            // // Recompute the complex set with the new zoom level
            complex_set_adjust_scale_centered(complex_set, scaleFactor);
            needRedraw = true;

            // // Get mouse position in screen coordinates
            // sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            // // // Convert to complex plane coordinates based on current view
            // std::complex<float> mousePosComplex = screenToComplex(pixelPos, complex_set, window.getSize());
            // // // Adjust zoom factor based on scroll direction
            // float scaleFactor = (event.mouseWheelScroll.delta > 0) ? 0.9 : 1.1;
            // zoom *= scaleFactor;
            // // // Update view center to keep mouse position stationary in complex plane
            // float scaleFactorMod = 1.0 - scaleFactor;
            // center += (mousePosComplex - center) * scaleFactorMod;
            // complex_set_adjust_view(complex_set, center, scaleFactor, window.getSize());
            // needRedraw = true;
        } else if (event.type == sf::Event::KeyPressed) {
            std::cout << "Key Pressed" << std::endl;
            switch (event.key.code) {
                case sf::Keyboard::Left:
                    std::cout << "Move left" << std::endl;
                    complex_set_adjust_real(complex_set, -panSpeed);
                    needRedraw = true;
                    break;
                case sf::Keyboard::Right:
                    std::cout << "Move right" << std::endl;
                    complex_set_adjust_real(complex_set, panSpeed);
                    needRedraw = true;
                    break;
                case sf::Keyboard::Up:
                    std::cout << "Move up" << std::endl;
                    complex_set_adjust_imag(complex_set, -panSpeed);
                    needRedraw = true;
                    break;
                case sf::Keyboard::Down:
                    std::cout << "Move down" << std::endl;
                    complex_set_adjust_imag(complex_set, panSpeed);
                    needRedraw = true;
                    break;
                default:
                    break; // No action for other keys
            }
        }
        return needRedraw;
    }
};