#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uvcords;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texsampler;

void main() {

    outColor = texture(texsampler, uvcords);
    //outColor = vec4(0.0, 1.0, 0.2, 1.0);
}