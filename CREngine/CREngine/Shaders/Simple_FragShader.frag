#version 450

layout(location = 0) in vec4 InColor;

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = InColor;
	OutColor.w = 1.0;

	OutColor.x = 1.0;
	OutColor.y = 1.0;
	OutColor.z = 0.5;
}