#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 4) uniform ColorObject {
    vec3 color;
} colorObject;

void main() {
    outColor = vec4(colorObject.color, 1);
}