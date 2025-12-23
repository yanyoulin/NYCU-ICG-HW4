#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 TexCoord;
flat in float RandomSeed; // [新增] 接收種子

// 簡易亂數函式
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

// 畫圓函式 (處理接縫)
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
    vec3 spotColor = vec3(0.6, 0.2, 0.5); // 深紫色斑點

    float spots = 0.0;

    // --- 隨機產生 3 個斑點 ---
    for(int i = 0; i < 3; i++) {
        // 利用 (Seed + i) 產生每一顆不一樣的參數
        
        // 1. 隨機水平位置 U (0.0 ~ 1.0)
        float r_u = rand(vec2(RandomSeed, float(i))); 
        
        // 2. 隨機垂直位置 V (0.2 ~ 0.7) -> 避免生在頭頂或太下面
        float r_v = 0.2 + 0.5 * rand(vec2(RandomSeed + 10.0, float(i)));
        
        // 3. 隨機大小 (0.05 ~ 0.10)
        float r_size = 0.05 + 0.05 * rand(vec2(RandomSeed + 20.0, float(i)));

        // 疊加斑點
        spots += circle(TexCoord, vec2(r_u, r_v), r_size);
    }
    // ----------------------

    spots = clamp(spots, 0.0, 1.0);
    vec3 finalColor = mix(baseColor, spotColor, spots);

    FragColor = vec4(finalColor, 1.0);
}