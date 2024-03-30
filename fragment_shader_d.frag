#version 460 core
out vec4 FragColor;
in dvec2 TexCoord;

uniform sampler1D colormap;
uniform sampler2D complexSet;

uniform int n_iterations;
uniform float threshold;


dvec2 complexMul(dvec2 a, dvec2 b) {
    float real = a.x * b.x - a.y * b.y;
    float imag = a.x * b.y + a.y * b.x;
    return dvec2(real, imag);
}

dvec2 mandelbrotFunc(dvec2 z_val, dvec2 complex_val) {
    return dvec2(complexMul(z_val, z_val) + complex_val);
}


// coonsider using nonlinear (e.g., logarithmic) scale
vec3 computeColorIteration(int iter) {
    vec3 color;
    if (iter == n_iterations) {
        // last iteration - always black color
        color = vec3(0.0, 0.0, 0.0);
    }
    else {
        // important - cast to float before the divison

        // version 1: linear scale, greyscale:
//        float color_ = float(iter) / float(n_iterations);
//        color = vec3(color_, color_, color_);

        // version 2: log scale, greyscale:
//         float color_ = log(float(iter)) / log(float(n_iterations));
//         color = vec3(color_, color_, color_);

        //version 3: log scale, colormap:
        if (iter == n_iterations) {
            // Points that are considered inside the Mandelbrot set
            color = vec3(0.5, 0.5, 0.0); // Example: Change to a distinct color, e.g., orange
        } else {
            // Points that escape are colored based on the logarithmic scale
            float color_ = log(float(iter)) / log(float(n_iterations));
            color = texture(colormap, color_).rgb;
}
    }
    return color;
}

void main()
{
    // complex_val.x - real, complex_val.y - imag
    dvec2 complex_val = texture(complexSet, TexCoord).rg; // Assuming TexCoord is available

    dvec2 z_value_iterated = dvec2(0.0, 0.0);
    int iter = 0;

    for (iter; iter < n_iterations; iter++) {
        z_value_iterated = mandelbrotFunc(z_value_iterated, complex_val);

        if (length(z_value_iterated) > threshold) {
            break;
        }
    }
    vec3 color = computeColorIteration(iter);

    FragColor = vec4(color.r, color.g, color.b, 1.0);
}