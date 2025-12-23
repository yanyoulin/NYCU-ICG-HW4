#version 330 core
out vec4 FragColor;

in vec2 TexCoord; 
in vec3 Normal;   // 雖然這裡沒用到光照計算，但為了介面一致先接進來
in vec3 FragPos;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}