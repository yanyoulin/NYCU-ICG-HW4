#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
} gs_in[];

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform float time;
uniform mat4 projection;

vec3 GetFaceNormal() {
    vec3 a = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    vec3 b = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    return normalize(cross(a, b));
}

void main() {
    vec3 faceNormal = GetFaceNormal();

    for(int i = 0; i < 3; i++) {
        vec3 pos = gl_in[i].gl_Position.xyz;
        vec3 vertexNormal = normalize(gs_in[i].Normal); 

        
        //波浪從頭到腳
        float wavePhase = time * 8.0 - pos.y * 0.1;
        float sinWave = sin(wavePhase); 
        float extrudeAmt = 0.5 + 2.5 * sinWave;

        vec3 finalPos = pos + vertexNormal * extrudeAmt;

        gl_Position = projection * vec4(finalPos, 1.0);
        TexCoord = gs_in[i].TexCoord;
        Normal = faceNormal; 
        FragPos = finalPos;

        EmitVertex();
    }
    EndPrimitive();
}