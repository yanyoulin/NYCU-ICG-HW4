#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// 輸出給 Geometry Shader 的介面
out VS_OUT {
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
// uniform mat4 projection; // 注意：這裡不乘 projection

void main()
{
    // 1. 只算到 View Space (觀察空間)
    gl_Position = view * model * vec4(aPos, 1.0);
    
    // 2. 傳遞紋理座標
    vs_out.TexCoord = aTexCoord;

    // 3. 計算世界空間法向量 (光照用)
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;

    // 4. 計算世界空間片段位置 (光照用)
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
}