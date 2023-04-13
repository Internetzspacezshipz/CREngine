#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor.x = color.x;
	OutColor.y = color.y;
	OutColor.z = color.z;
	OutColor.w = 1;

	//Position must be less than 1.0 to be rendered.
	gl_Position = vec4(position, 0.5f);
}
