#version 330 core

out float FragColor;

in vec3 vPos;

const int permutations[128] = int[]
(
    27, 33, 14, 52, 24, 36, 46, 40, 26, 7, 49, 57, 59, 2, 42, 61, 9, 3, 12, 63, 37, 53, 17, 8, 44, 35, 30, 22, 6, 18, 60, 55,
    31, 13, 21, 5, 47, 25, 38, 28, 32, 45, 43, 48, 23, 58, 62, 41, 11, 29, 34, 54, 0, 1, 20, 19, 16, 4, 15, 50, 10, 39, 56, 51,
    27, 33, 14, 52, 24, 36, 46, 40, 26, 7, 49, 57, 59, 2, 42, 61, 9, 3, 12, 63, 37, 53, 17, 8, 44, 35, 30, 22, 6, 18, 60, 55,
    31, 13, 21, 5, 47, 25, 38, 28, 32, 45, 43, 48, 23, 58, 62, 41, 11, 29, 34, 54, 0, 1, 20, 19, 16, 4, 15, 50, 10, 39, 56, 51
);

const vec3 gradients[12] = vec3[]
(
    vec3( 1,  1,  0), vec3(-1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0,  1, -1), vec3( 0, -1, -1)
);

float fade(float t) 
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) 
{
    return a + t * (b - a);
}

float grad(int hash, vec3 p) 
{
    vec3 g = gradients[hash % 12];
    return dot(g, p);
}

float perlin(vec3 p) 
{
    vec3 Pi = floor(p);
    vec3 Pf = p - Pi;
    Pi = mod(Pi, 64.0);

    int xi = int(Pi.x);
    int yi = int(Pi.y);
    int zi = int(Pi.z);

    float xf = Pf.x;
    float yf = Pf.y;
    float zf = Pf.z;

    float u = fade(xf);
    float v = fade(yf);
    float w = fade(zf);

    int aaa = permutations[permutations[permutations[xi] + yi] + zi];
    int aba = permutations[permutations[permutations[xi] + yi + 1] + zi];
    int aab = permutations[permutations[permutations[xi] + yi] + zi + 1];
    int abb = permutations[permutations[permutations[xi] + yi + 1] + zi + 1];
    int baa = permutations[permutations[permutations[xi + 1] + yi] + zi];
    int bba = permutations[permutations[permutations[xi + 1] + yi + 1] + zi];
    int bab = permutations[permutations[permutations[xi + 1] + yi] + zi + 1];
    int bbb = permutations[permutations[permutations[xi + 1] + yi + 1] + zi + 1];

    float x1 = lerp(grad(aaa, Pf), grad(baa, Pf - vec3(1, 0, 0)), u);
    float x2 = lerp(grad(aba, Pf - vec3(0, 1, 0)), grad(bba, Pf - vec3(1, 1, 0)), u);
    float y1 = lerp(x1, x2, v);

    x1 = lerp(grad(aab, Pf - vec3(0, 0, 1)), grad(bab, Pf - vec3(1, 0, 1)), u);
    x2 = lerp(grad(abb, Pf - vec3(0, 1, 1)), grad(bbb, Pf - vec3(1, 1, 1)), u);
    float y2 = lerp(x1, x2, v);

    return lerp(y1, y2, w);
}

void main() {
    float noise = perlin(vPos);
    noise = (noise + 1.0) / 2.0;
    FragColor = noise;
}
