#version 450
#extension GL_ARB_separate_shader_objects : enable

//UNIFORM
layout(binding = 1) uniform sampler2D albedoTexture;

//IN
layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec3 fragNormal;

//OUT
layout(location = 0) out vec4 outColor;

void main() {
    vec3 baseColor = texture(albedoTexture, fragUV).rgb;
	float diffuse = max(0.0, dot(fragNormal, vec3(0,1,1)));

	outColor = vec4(baseColor,1); // * diffuse, 1);
}