#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 TexCoord;
flat in float RandomSeed; 

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float circle(vec2 uv, vec2 pos, float radius) {
    float dx = abs(uv.x - pos.x);
    if (dx > 0.5) dx = 1.0 - dx;
    float dy = uv.y - pos.y;
    float d = sqrt(dx*dx + dy*dy);
    return 1.0 - smoothstep(radius - 0.02, radius + 0.02, d);
}

void main()
{
    vec3 baseColor = Color;
    vec3 spotColor = vec3(0.6, 0.2, 0.5); // 深紫斑點

    float spots = 0.0;

    // 隨機產生3個斑點
    for(int i = 0; i < 3; i++) {
        float r_u = rand(vec2(RandomSeed, float(i))); 
        float r_v = 0.2 + 0.5 * rand(vec2(RandomSeed + 10.0, float(i)));
        float r_size = 0.05 + 0.05 * rand(vec2(RandomSeed + 20.0, float(i)));

        spots += circle(TexCoord, vec2(r_u, r_v), r_size);
    }

    spots = clamp(spots, 0.0, 1.0);
    vec3 finalColor = mix(baseColor, spotColor, spots);

    FragColor = vec4(finalColor, 1.0);
}