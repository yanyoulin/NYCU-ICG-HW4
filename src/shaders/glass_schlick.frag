#version 330 core

// TODO: Implement glass shading with schlick method

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube skybox;

// Air = 1.0, Glass = 1.52 
const float n1 = 1.0;
const float n2 = 1.52;

void main()
{          
    vec3 I = normalize(FragPos - viewPos);
    vec3 N = normalize(Normal);

    float R0 = pow((n1 - n2) / (n1 + n2), 2.0f);

    float R_theta = R0 + (1.0f - R0) * pow(1.0f + dot(I, N), 5.0f);

    float eta = n1 / n2;
    
    vec3 R = reflect(I, N);
    vec3 T = refract(I, N, eta);

    vec3 C_reflect = texture(skybox, R).rgb;
    vec3 C_refract = texture(skybox, T).rgb;

    vec3 finalColor = R_theta * C_reflect + (1 - R_theta) * C_refract;

    FragColor = vec4(finalColor, 1.0);
} 