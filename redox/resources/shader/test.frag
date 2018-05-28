#version 450
#extension GL_ARB_separate_shader_objects : enable

//UNIFORM
layout(binding = 1) uniform sampler2D texSampler;

//IN
layout(location = 0) in vec2 fragUV;

//OUT
layout(location = 0) out vec4 outColor;

void main() {

    outColor = texture(texSampler, fragUV);;
}