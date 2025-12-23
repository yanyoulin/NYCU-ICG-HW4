#version 330 core
layout (location = 0) in vec3 aPos; 

uniform mat4 model;
uniform mat4 view;

// [新增] 輸出原始座標給 Geometry Shader
out vec3 RawPos; 

void main()
{
    gl_Position = view * model * vec4(aPos, 1.0);
    
    // [新增] 傳遞原始座標 (這是不會變的)
    RawPos = aPos; 
}