#version 330 core

// TODO:
// Implement Gouraud shading

out vec4 FragColor;

in vec3 LightingColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
	vec3 texColor = texture(ourTexture, TexCoord).rgb;
    FragColor = vec4(LightingColor * texColor, 1.0);
}