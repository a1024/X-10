#version 450

layout(binding = 4) uniform MyUBO
{
	vec4 txt;
	vec4 bk;
	float zoom;
} ubo;
layout(binding = 5) uniform sampler2D uAtlas;

layout(location = 0) in vec2 fragTexcoord;

layout(location = 0) out vec4 outColor;
void main()
{
	vec4 region=texture(uAtlas, fragTexcoord);
	float temp=clamp(ubo.zoom*(0.5f+0.45f/ubo.zoom-region.r), 0, 1);
	outColor=mix(ubo.txt, ubo.bk, temp);
	//outColor=vec4(1., temp, 0., 1.);
}