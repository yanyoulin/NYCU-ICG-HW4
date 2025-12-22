#version 330 core
layout (location = 0) in float startOffset; // 每個泡泡的隨機起始時間

out float vOffset;

void main()
{
    vOffset = startOffset;
    // 不設 gl_Position，因為泡泡的起點(嘴巴)會在 Geometry Shader 用
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0); 
}