#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 ExplosionColor; 

uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    FragColor = mix(texColor, ExplosionColor, 0.5); // 0.5 是混合程度，可調整
}