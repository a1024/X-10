#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 fragTexcoord;

void main()
{
	gl_Position=vec4(position.x, -position.y, 0., 1.);
	fragTexcoord=texcoord;
}