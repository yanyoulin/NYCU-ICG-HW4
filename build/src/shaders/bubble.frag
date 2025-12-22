#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in float Alpha;

void main()
{
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(TexCoord, center);
    
    if(dist > 0.5) discard; // 切掉四邊形的角，變圓形

    // 邊緣發光
    float rim = smoothstep(0.4, 0.5, dist);
    
    // 淡藍色，透明度隨高度變化
    FragColor = vec4(0.6, 0.8, 1.0, 0.6 * Alpha + rim * 0.4); 
}