#version 450

layout(location = 0) in vec2 Position;
layout(location = 1) in vec3 Color;

layout(location = 0) out vec3 FragColor;

layout(push_constant) uniform Push
{
	vec2 Offset;
	vec3 Color;
} PushOb;

void main()
{
	gl_Position = vec4(Position + PushOb.Offset, 0.0, 1.0);
	FragColor = Color + PushOb.Color;
}
