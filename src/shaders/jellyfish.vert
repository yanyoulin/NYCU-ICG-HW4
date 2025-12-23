#version 330 core
layout (location = 0) in vec3 aPos; 

uniform mat4 model;
uniform mat4 view;

out vec3 RawPos; 

void main()
{
    gl_Position = view * model * vec4(aPos, 1.0);
    RawPos = aPos; 
}