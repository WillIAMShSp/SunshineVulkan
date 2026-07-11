#version 450


layout(location = 0) in vec2 uvcords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texsampler;

void main() {

    outColor = vec4(texture(texsampler, uvcords));
   

    //outColor = vec4(0.0, 1.0, 0.2, 1.0);
}