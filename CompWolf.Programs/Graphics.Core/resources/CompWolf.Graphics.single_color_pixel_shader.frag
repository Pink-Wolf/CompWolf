#version 450

layout(binding = 4) uniform ColorObject {
    vec3 color;
} colorObject;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(colorObject.color, 1);
}