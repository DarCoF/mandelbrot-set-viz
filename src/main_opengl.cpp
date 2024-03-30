#include <iostream>
#include <complex>
#include <vector>
#include <random>
// GLEW header for OpenGL extension handling
#include <GL/glew.h>
// SFML headers for windowing, input and OpenGL
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>


// Local
#include "../headers/event_manager.hpp"
#include "../headers/utils_shader.hpp"
#include "../headers/utils.hpp"

int main() {
    const int width = 1080;
    const int height = 720;
    const int maxIterations = 200;
    float threshold = 2;
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    
    sf::RenderWindow window(sf::VideoMode(width, height), "OpenGL Mandelbrot Set", sf::Style::Default, settings);
    window.setActive(true);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        // Handle the error, for example:
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << "\n";
        return -1;
    }glewExperimental = GL_TRUE;

    //---------------- VBO definition
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
            -1.0f, 1.0f, 0.0f, 1.0f, // Top-left
            1.0f, 1.0f, 1.0f, 1.0f, // Top-right
    };
    unsigned int indices[] = {0, 1, 2, 1, 2, 3};
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // --------- VAO definition
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindVertexArray(0);
    
    // ----------- Load shader program
    GLuint shaderProgram = utils_shaders::LoadShaders("vertex_shader_d.vert", "fragment_shader_d.frag");
    if (shaderProgram == 0) {
        return -1;
    }
    glUseProgram(shaderProgram); // Use the shader program

    // ------------ create COMPLEX VALUES TEXTURE ----------------------------
    float real_min = -2.0f, real_max = 1.0f, imag_min = -1.5f, imag_max = 1.5f;
    std::vector<float> complex_set = gen_complex_set_2_shader(
            width, height, real_min, real_max, imag_min, imag_max
    );

    GLuint tex_complex;
    glGenTextures(1, &tex_complex);
    glBindTexture(GL_TEXTURE_2D, tex_complex);
    // GL_RG stores 2 values per pixel - Red, Green
    // Red will store real part of complex number and Green - imaginary
    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, complex_set.data()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // -------------------------------------------------------------

    // --------------- create COLORMAP TEXTURE ------------------------------
    int n_colors = 256;
    std::vector<float> grayscale_colormap = generate_smooth_colormap(n_colors);

    GLuint tex_colormap;
    glGenTextures(1, &tex_colormap);
    glBindTexture(GL_TEXTURE_1D, tex_colormap);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, n_colors, 0, GL_RGB, GL_FLOAT, grayscale_colormap.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // ------------ get locations of dynamic uniform parameters
    GLint loc_threshold = glGetUniformLocation(shaderProgram, "threshold");
    GLint loc_n_iterations = glGetUniformLocation(shaderProgram, "n_iterations");
    GLint loc_colormap = glGetUniformLocation(shaderProgram, "colormap");
    GLint loc_complex_set = glGetUniformLocation(shaderProgram, "complexSet");


    // Event manager
    MandelbrotEventManager eventManager(1, {-0.5, 0});

    int tex_unit_complex_set = 1;
    int tex_unit_colormap = 0;

    while (window.isOpen()) {

        // Handler user inputs (zoom and pan)
        bool needRedraw = eventManager.handleEvents(window, complex_set);

         // Print the needsRedraw value
        std::cout << "Needs Redraw: " << (needRedraw ? "Yes" : "No") << std::endl;


        // Render on the whole framebuffer, complete from the lower left corner to
        // the upper right
        glViewport(0, 0, width, height);
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);

        if (needRedraw) {
            glBindTexture(GL_TEXTURE_2D, tex_complex); // Bind the texture
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RG, GL_FLOAT, complex_set.data());
            glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        }

        // Update uniform values based on user input
        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0 + tex_unit_complex_set);
        glBindTexture(GL_TEXTURE_2D, tex_complex);
        glUniform1i(loc_complex_set, tex_unit_complex_set);

        glActiveTexture(GL_TEXTURE0 + tex_unit_colormap);
        glBindTexture(GL_TEXTURE_1D, tex_colormap);
        glUniform1i(loc_colormap, tex_unit_colormap);

        glUniform1f(loc_threshold, threshold);
        glUniform1i(loc_n_iterations, maxIterations);

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            
        window.display();

    }
    // Properly de-allocate resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    // Delete textures
    glDeleteTextures(1, &tex_complex);
    glDeleteTextures(1, &tex_colormap);
    return 0;
};