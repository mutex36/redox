#version 450
#extension GL_ARB_separate_shader_objects : enable

//UNIFORM
layout(binding = 1) uniform sampler2D albedoTexture;
layout(binding = 2) uniform sampler2D normalTexture;

//IN
layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec3 fragNormal;

//OUT
layout(location = 0) out vec4 outColor;

vec3 lightDir = normalize(vec3(1));
vec3 lightColor = vec3(1);
float ambientStrength = 0.8;

float lambert(vec3 N, vec3 L) {
	return max(dot(N, L), 0.0);
}

void main() {
    vec3 baseColor = texture(albedoTexture, fragUV).rgb;
    vec3 normalColor = texture(normalTexture, fragUV).rgb;

    vec3 normal = normalize(fragNormal);

    vec3 ambientColor = ambientStrength * lightColor;
    float diffuse = lambert(normal, lightDir);
	vec3 diffuseColor = diffuse * lightColor;

	vec3 result = (ambientColor + diffuseColor) * baseColor;
	outColor = vec4(result, 1);
}