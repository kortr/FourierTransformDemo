#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout(binding = 0)uniform Position_UBO{
	mat4 model;
}POS_UBO;
layout(push_constant) uniform uPushConstant {
        mat4 projection;
} pc;
void main() {
	outColor = inColor;
	gl_Position = pc.projection * POS_UBO.model * vec4(inPos, 1.0);
}
