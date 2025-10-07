#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    // Convert from pixel coordinates to normalized device coordinates
    vec2 ndc = vec2(
        (inPosition.x / 1280.0) * 2.0 - 1.0,  // assuming 1280 width
        (inPosition.y / 720.0) * 2.0 - 1.0    // assuming 720 height
    );

    // Flip y-axis so top-left is (0, 0)
    ndc.y = -ndc.y;

    gl_Position = vec4(ndc, 0.0, 1.0);
    fragColor = inColor;
}