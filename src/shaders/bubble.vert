#version 330 core
layout (location = 0) in float startOffset; 

out float vOffset;

void main()
{
    vOffset = startOffset;
    // 不設 gl_Position，因為泡泡的起點會在 Geometry Shader 處理
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0); 
}