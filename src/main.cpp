#include <SFML/Graphics.hpp>
#include <complex>
#include <mutex>
#include <vector>
#include <thread>


// Global mutext for image manipulation
std::mutex imageMutex;

// Function to calculate the Mandelbrot iteration count for a given point
int mandelbrotIterationCount(const std::complex<double>& z0, int maxIterations) {
    std::complex<double> z = z0;
    for(int i = 0; i < maxIterations; ++i) {
        if(std::abs(z) > 2.0) return i;
        z = z*z + z0;
    }
    return maxIterations;
}

// Function to map a value from one range to another
double map(double value, double inMin, double inMax, double outMin, double outMax) {
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

// Generate a color map based on the number of iterations
sf::Color getColor(int iteration, int maxIterations) {
    int r, g, b;
    double t = (double)iteration / (double)maxIterations;

    // Example gradient: from blue to red
    r = (int)(9*(1-t)*t*t*t*255);
    g = (int)(15*(1-t)*t*t*t*255);
    b = (int)(8.5*(1-t)*t*t*t*255);

    return sf::Color(r, g, b);
}

// Event manager for user input control
class MandelbrotEventManager {
public:
    MandelbrotEventManager(double initialZoom, const std::complex<double>& initialCenter)
        : zoom(initialZoom), center(initialCenter) {}

    // Process events, adjust zoom and center based on input
    void handleEvents(sf::RenderWindow& window) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else
                handleZoomAndPan(event);
        }
    }

    double getZoom() const { return zoom; }
    std::complex<double> getCenter() const { return center; }

private:
    double zoom;
    std::complex<double> center;

    void handleZoomAndPan(const sf::Event& event) {
        if (event.type == sf::Event::MouseWheelScrolled) {
            zoom *= (event.mouseWheelScroll.delta > 0) ? 0.9 : 1.1; // Adjust zoom factor
        } else if (event.type == sf::Event::KeyPressed) {
            double panSpeed = 0.1 * zoom; // Adjust pan speed based on zoom level
            switch (event.key.code) {
                case sf::Keyboard::Left:
                    center.real(center.real() - panSpeed);
                    break;
                case sf::Keyboard::Right:
                    center.real(center.real() + panSpeed);
                    break;
                case sf::Keyboard::Up:
                    center.imag(center.imag() - panSpeed);
                    break;
                case sf::Keyboard::Down:
                    center.imag(center.imag() + panSpeed);
                    break;
                default:
                    break; // No action for other keys
            }
        }
    }
};

// Divide the image in sections
void renderSection(sf::Image& image, int startX, int endX, int startY, int endY, double zoom, std::complex<double> center, int maxIterations, int width, int height) {
    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            std::complex<double> point(map(x, 0, width, center.real() - zoom, center.real() + zoom),
                                       map(y, 0, height, center.imag() - zoom, center.imag() + zoom));
            int iteration = mandelbrotIterationCount(point, maxIterations);
            sf::Color color = getColor(iteration, maxIterations);
            imageMutex.lock();
            image.setPixel(x, y, color);
            imageMutex.unlock();
        }
    }
}

int main() {
    const int width = 2560;
    const int height = 1440;
    const int maxIterations = 200;
    
    
    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot Set");
    sf::Image image;
    image.create(width, height, sf::Color(0, 0, 0));
    sf::Texture texture;
    sf::Sprite sprite;
    bool needRedraw = true;

    // Event manager
    MandelbrotEventManager eventManager(1, {-0.5, 0});

    while (window.isOpen()) {
        eventManager.handleEvents(window);

        if (needRedraw) {
            const int threadCount = std::thread::hardware_concurrency();
            std::vector<std::thread> threads;
            int stripWidth = width / threadCount;

            for (int i = 0; i < threadCount; ++i) {
                int startX = i * stripWidth;
                int endX = (i + 1) * stripWidth;
                if (i == threadCount - 1) {
                    endX = width; // Ensure the last strip covers the rest of the image
                }
                // Launch threads
                threads.emplace_back(renderSection, std::ref(image), startX, endX, 0, height, eventManager.getZoom(), eventManager.getCenter(), maxIterations, width, height);
            }

            // Join threads
            for (auto& t : threads) {
               
                t.join();
            }

            // After all threads complete, update the texture and sprite
            texture.loadFromImage(image);
            sprite.setTexture(texture);

            //needRedraw = false; // Reset the flag as we've just redrawn
        }

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }

    return 0;
}