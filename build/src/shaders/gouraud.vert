#version 330 core

// TODO:
// Implement Gouraud shading
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 LightingColor;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float gloss;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main()
{
	vec3 Pos = vec3(model * vec4(aPos, 1.0f));
    vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);

    vec3 L = normalize(light.position - Pos);
    vec3 V = normalize(viewPos - Pos);
    vec3 R = reflect(-L, N);

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * material.diffuse * max(dot(L, N), 0.0f);
    vec3 specular = light.specular * material.specular * pow(max(dot(V, R), 0.0f), material.gloss);

    LightingColor = ambient + diffuse + specular;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}