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

// 計算面法線 (用來製造"細胞/晶體"的視覺質感)
vec3 GetFaceNormal() {
    vec3 a = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    vec3 b = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    return normalize(cross(a, b));
}

void main() {
    // 雖然模型不縮小，但我們用"面法線"來做光照，會有鑽石/細胞切面的感覺
    vec3 faceNormal = GetFaceNormal();

    for(int i = 0; i < 3; i++) {
        vec3 pos = gl_in[i].gl_Position.xyz;
        vec3 vertexNormal = normalize(gs_in[i].Normal); // 使用原本的頂點法線移動，才不會裂開

        // --- 波浪參數 ---
        
        // 1. 波浪計算 (從頭流到腳)
        // pos.y : 使用頂點高度，讓波浪更平滑
        float wavePhase = time * 8.0 - pos.y * 0.1;
        float sinWave = sin(wavePhase); // -1 ~ 1

        // 2. 膨脹計算
        // 基礎膨脹 (0.5) + 波浪起伏 (2.5)
        // 這樣雖然會起伏，但不會縮到比原本還小(避免穿模)
        float extrudeAmt = 0.5 + 2.5 * sinWave;

        // --- 核心修改 ---
        
        // 我們 "不" 做縮放 (shrink)，直接把點往外推
        // 使用 vertexNormal (頂點法線) 推擠，模型就不會裂開
        vec3 finalPos = pos + vertexNormal * extrudeAmt;

        // 轉成 Clip Space
        gl_Position = projection * vec4(finalPos, 1.0);
        
        // 傳遞資料
        TexCoord = gs_in[i].TexCoord;
        
        // [視覺重點] 
        // 這裡傳出去的是 faceNormal (面法線)，而不是原本的平滑法線
        // 這會讓光影看起來是一格一格的 (Low Poly 風格)，但模型本身是連在一起的
        Normal = faceNormal; 
        
        FragPos = finalPos;

        EmitVertex();
    }
    EndPrimitive();
}