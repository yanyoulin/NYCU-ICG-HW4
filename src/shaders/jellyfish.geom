#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 256) out;

uniform mat4 projection;
uniform float time;

in vec3 RawPos[];

out vec3 Color;
out vec2 TexCoord;
flat out float RandomSeed;

const float PI = 3.1415926;
float size_scale = 2.0; 

// 畫觸手
void CreateTentacle(vec3 center, float angleOffset, float seed) {
    float legLength = 15.0 * size_scale; 
    float width = 1.0 * size_scale;      
    int segments = 5;       
    
    for(int i = 0; i <= segments; i++) {
        float t = float(i) / float(segments);
        float y = -t * legLength;
        
        float wiggleX = sin(time * 5.0 + y * (0.5 / size_scale) + angleOffset) * 2.0 * t * size_scale; 
        float wiggleZ = cos(time * 3.0 + y * (0.5 / size_scale) + angleOffset) * 2.0 * t * size_scale;

        vec3 currentPos = center + vec3(wiggleX, y, wiggleZ);
        vec3 offset = vec3(width * (1.0 - t), 0.0, 0.0); 
        
        gl_Position = projection * vec4(currentPos - offset, 1.0);
        Color = vec3(0.8, 0.4, 0.8);
        TexCoord = vec2(0.0, t);
        RandomSeed = seed;
        EmitVertex();
        
        gl_Position = projection * vec4(currentPos + offset, 1.0);
        Color = vec3(0.8, 0.4, 0.8);
        TexCoord = vec2(1.0, t);
        RandomSeed = seed;
        EmitVertex();
    }
    EndPrimitive();
}

void main() {
    vec3 center = gl_in[0].gl_Position.xyz;
    float mySeed = RawPos[0].x * 12.989 + RawPos[0].z * 78.233;
    
    float radius = 8.0 * size_scale;
    int steps = 12; 
    int layers = 6; 

    for (int j = 0; j < layers; j++) {
        float v0 = float(j) / float(layers);
        float v1 = float(j + 1) / float(layers);

        float lat0 = (PI * 0.5) * float(j) / float(layers);
        float lat1 = (PI * 0.5) * float(j + 1) / float(layers);
        
        float y0 = sin(lat0) * radius;
        float r0 = cos(lat0) * radius;
        
        float y1 = sin(lat1) * radius;
        float r1 = cos(lat1) * radius;

        float breathe = 1.0 + 0.05 * sin(time * 3.0);

        for (int i = 0; i <= steps; i++) {
            float u = float(i) / float(steps);

            float theta = 2.0 * PI * float(i) / float(steps);
            float x = cos(theta);
            float z = sin(theta);

            vec3 p0 = center + vec3(x * r0 * breathe, y0, z * r0 * breathe);
            vec3 p1 = center + vec3(x * r1 * breathe, y1, z * r1 * breathe);

            gl_Position = projection * vec4(p0, 1.0);
            Color = vec3(1.0, 0.6 - float(j)*0.1, 0.8); 
            TexCoord = vec2(u, v0);
            RandomSeed = mySeed;
            EmitVertex();

            gl_Position = projection * vec4(p1, 1.0);
            Color = vec3(1.0, 0.6 - float(j+1)*0.1, 0.8);
            TexCoord = vec2(u, v1);
            RandomSeed = mySeed;
            EmitVertex();
        }
        EndPrimitive();
    }

    // 畫觸手
    float tOff = 3.0 * size_scale;
    float tY = 0.0;

    CreateTentacle(center + vec3( tOff, tY,  tOff), 0.0, mySeed);
    CreateTentacle(center + vec3(-tOff, tY,  tOff), 1.5, mySeed);
    CreateTentacle(center + vec3( tOff, tY, -tOff), 3.0, mySeed);
    CreateTentacle(center + vec3(-tOff, tY, -tOff), 4.5, mySeed);
}