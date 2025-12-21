#version 330 core

// TODO: Implement metallic shading

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main() 
{
    vec3 I = normalize(FragPos - viewPos);
    vec3 N = normalize(Normal);
    vec3 R = reflect(I, N);

    vec3 reflectionColor = texture(skybox, R).rgb;
    
    FragColor = vec4(reflectionColor, 1.0);
}	
