#version 450

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;

layout(location = 0) out vec3 FragColor;

layout(push_constant) uniform Push
{
	mat4 Transform;
	vec3 Color;
} PushOb;

void main()
{
	gl_Position = PushOb.Transform * vec4(Position, 1.0);
	FragColor = Color;
}
