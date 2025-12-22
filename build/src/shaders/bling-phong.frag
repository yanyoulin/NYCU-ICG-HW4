#version 330 core

// TODO: Implement bling-phong shading
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

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

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D ourTexture;

void main()
{
	vec3 texColor = texture(ourTexture, TexCoord).rgb;

    vec3 norm = normalize(Normal);
    vec3 L = normalize(light.position - FragPos);
    vec3 V = normalize(viewPos - FragPos);
    vec3 H = normalize(L + V);

    vec3 ambient = light.ambient * material.ambient * texColor;
    vec3 diffuse = light.diffuse * material.diffuse * max(dot(L, norm), 0.0f) * texColor;
    vec3 specular = light.specular * material.specular * pow(max(dot(norm, H), 0.0f), material.gloss);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}