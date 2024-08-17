#version 450

layout(location = 0) in vec2 inPosition;

layout(binding = 0) uniform TransformObject {
    vec2 position;
} transform;

void main() {
    gl_Position = vec4(inPosition + transform.position, 0.0, 1.0);
}