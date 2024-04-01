#version 400 core
out dvec4 FragColor; // Use vec4 here
in dvec2 TexCoord;

uniform sampler1D colormap;
uniform sampler2D complexSet;

uniform int n_iterations;
uniform double threshold;

dvec2 complexMul(dvec2 a, dvec2 b) {
    double real = a.x * b.x - a.y * b.y;
    double imag = a.x * b.y + a.y * b.x;
    return dvec2(real, imag);
}

dvec2 mandelbrotFunc(dvec2 z_val, dvec2 complex_val) {
    return complexMul(z_val, z_val) + complex_val;
}

dvec3 computeColorIteration(int iter) {
    dvec3 color;
    if (iter == n_iterations) {
        color = dvec3(0.0, 0.0, 0.0);
    } else {
        if (iter == n_iterations) {
            color = dvec3(0.5, 0.5, 0.0); // Example: Change to a distinct color, e.g., orange
        } else {
            // Color for points outside the Mandelbrot set, based on escape time
            double colorIndex = log(double(iter)) / log(double(n_iterations));
            color = texture(colormap, colorIndex).rgb;
        }
    }
    return color;
}

void main() {
    dvec2 complex_val = texture(complexSet, TexCoord).rg;

    dvec2 z_value_iterated = dvec2(0.0, 0.0);
    int iter = 0;

    for (iter; iter < n_iterations; iter++) {
        z_value_iterated = mandelbrotFunc(z_value_iterated, complex_val);

        if (length(z_value_iterated) > threshold) {
            break;
        }
    }
    dvec3 color = computeColorIteration(iter);

    FragColor = dvec4(color, 1.0); // Convert dvec3 color to vec4
}