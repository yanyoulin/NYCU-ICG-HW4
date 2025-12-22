#version 330 core
layout (points) in; 
layout (triangle_strip, max_vertices = 4) out; // 輸出四邊形

in float vOffset[]; 

uniform mat4 view;
uniform mat4 projection;
uniform float time; 
uniform mat4 model;

const vec3 LOCAL_MOUTH_OFFSET = vec3(0.0, 0.6, 0.3);

out vec2 TexCoord;
out float Alpha; // 透明度

float rand(float n){
    return fract(sin(n) * 43758.5453123);
}

void main() {
    vec3 worldMouthPos = vec3(model * vec4(LOCAL_MOUTH_OFFSET, 1.0));
    
    float speed = 30.0;
    float lifeCycle = 5.0;
    float t = mod(time + vOffset[0], lifeCycle);

    vec3 localDir = normalize(vec3(0.0, 1.0, 1.0));
    vec3 worldDirection = normalize(mat3(model) * localDir);
    vec3 movement = worldDirection * (t * speed);
    float sway = sin(t * 3.0 + vOffset[0]) * 5.0; 
    
    vec3 centerPos = worldMouthPos + movement + vec3(sway, 0.0, 0.0);

    Alpha = 1.0 - (t / lifeCycle);
    vec4 pPos = projection * view * vec4(centerPos, 1.0);
    
    float r = rand(vOffset[0]);
    float baseSize = 10.0;
    float size = baseSize + (r * 15.0);

    // 左下
    gl_Position = pPos + vec4(-size, -size, 0.0, 0.0);
    TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    // 右下
    gl_Position = pPos + vec4(size, -size, 0.0, 0.0);
    TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    // 左上
    gl_Position = pPos + vec4(-size, size, 0.0, 0.0);
    TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    // 右上
    gl_Position = pPos + vec4(size, size, 0.0, 0.0);
    TexCoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}