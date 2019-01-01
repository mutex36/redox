#version 450
#extension GL_ARB_separate_shader_objects : enable

//UNIFORM
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp_buffer;

//IN
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

//OUT
layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragNormal;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
	mat4 mvp = mvp_buffer.model * mvp_buffer.view * mvp_buffer.proj;
	gl_Position = vec4(inPosition, 1.0) * mvp;

	fragUV = inUV;
	fragNormal = inNormal * mat3(mvp_buffer.model);
}