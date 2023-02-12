#version 450

layout(location = 0) in vec4 FragColor;

layout (location = 0) out vec4 OutColor;

layout(push_constant) uniform Push
{
	mat2 Transform;
	vec2 Offset;
	vec3 Color;
} PushOb;

void main()
{
	OutColor = vec4(PushOb.Color, 1.0);
}