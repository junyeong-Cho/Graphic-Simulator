#version 300 es

uniform float uZ;
uniform int uPattern;

in vec2 TexCoord;

out float FragColor;

const int permutations[128] = int[]
(
    27, 33, 14, 52, 24, 36, 46, 40, 26, 7, 49, 57, 59, 2, 42, 61, 9, 3, 12, 63, 37, 53, 17, 8, 44, 35, 30, 22, 6, 18, 60, 55,
    31, 13, 21, 5, 47, 25, 38, 28, 32, 45, 43, 48, 23, 58, 62, 41, 11, 29, 34, 54, 0, 1, 20, 19, 16, 4, 15, 50, 10, 39, 56, 51,
    27, 33, 14, 52, 24, 36, 46, 40, 26, 7, 49, 57, 59, 2, 42, 61, 9, 3, 12, 63, 37, 53, 17, 8, 44, 35, 30, 22, 6, 18, 60, 55,
    31, 13, 21, 5, 47, 25, 38, 28, 32, 45, 43, 48, 23, 58, 62, 41, 11, 29, 34, 54, 0, 1, 20, 19, 16, 4, 15, 50, 10, 39, 56, 51
);


float quintic(float t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float quinticDeriv(float t)
{
    return 30.0 * t * t * (t * (t - 2.0) + 1.0);
}

float lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

float gradientDotV(int perm, vec3 p)
{
    switch (perm & 15)
    {
        case  0: return  p.x + p.y;
        case  1: return -p.x + p.y;
        case  2: return  p.x - p.y;
        case  3: return -p.x - p.y;
        case  4: return  p.x + p.z;
        case  5: return -p.x + p.z;
        case  6: return  p.x - p.z;
        case  7: return -p.x - p.z;
        case  8: return  p.y + p.z;
        case  9: return -p.y + p.z;
        case 10: return  p.y - p.z;
        case 11: return -p.y - p.z;
        case 12: return  p.x + p.y;
        case 13: return -p.x + p.y;
        case 14: return -p.y + p.z;
        case 15: return -p.y - p.z;
    }
    return 0.0;
}


float noise(vec3 p)
{
    vec3 Pi = floor(p);
    vec3 Pf = fract(p);

    int ix = int(Pi.x) & 63;
    int iy = int(Pi.y) & 63;
    int iz = int(Pi.z) & 63;

    int ix1 = (ix + 1) & 63;
    int iy1 = (iy + 1) & 63;
    int iz1 = (iz + 1) & 63;

    int ixy = permutations[(ix + permutations[iy]) ];
    int ixy1 = permutations[(ix1 + permutations[iy])];
    int ixy2 = permutations[(ix + permutations[iy1])];
    int ixy3 = permutations[(ix1 + permutations[iy1])];

    int g000 = permutations[(ixy + iz) ];
    int g001 = permutations[(ixy + iz1)];
    int g010 = permutations[(ixy2 + iz)];
    int g011 = permutations[(ixy2 + iz1)];
    int g100 = permutations[(ixy1 + iz) ];
    int g101 = permutations[(ixy1 + iz1)];
    int g110 = permutations[(ixy3 + iz) ];
    int g111 = permutations[(ixy3 + iz1)];


    vec3 fadePf = vec3(quintic(Pf.x), quintic(Pf.y), quintic(Pf.z));

    float n000 = gradientDotV(g000, Pf);
    float n100 = gradientDotV(g100, Pf - vec3(1.0, 0.0, 0.0));
    float n010 = gradientDotV(g010, Pf - vec3(0.0, 1.0, 0.0));
    float n110 = gradientDotV(g110, Pf - vec3(1.0, 1.0, 0.0));
    float n001 = gradientDotV(g001, Pf - vec3(0.0, 0.0, 1.0));
    float n101 = gradientDotV(g101, Pf - vec3(1.0, 0.0, 1.0));
    float n011 = gradientDotV(g011, Pf - vec3(0.0, 1.0, 1.0));
    float n111 = gradientDotV(g111, Pf - vec3(1.0, 1.0, 1.0));

    float n00 = lerp(fadePf.x, n000, n100);
    float n10 = lerp(fadePf.x, n010, n110);
    float n01 = lerp(fadePf.x, n001, n101);
    float n11 = lerp(fadePf.x, n011, n111);

    float n0 = lerp(fadePf.y, n00, n10);
    float n1 = lerp(fadePf.y, n01, n11);

    return lerp(fadePf.z, n0, n1);
}

void main()
{
    vec3 p = vec3(TexCoord * 64.0, uZ);
    float n = noise(p);
    n = (n + 1.0) / 2.0; // Remap to [0,1]

    float result = 0.0;
    const int numLayers = 5;
    float amplitude = 0.5;
    float frequency = 1.0;

    float lacunarity = 2.0f;
    float gain = 0.5f;

    switch (uPattern)
    {
        case 0: // Plain Gradient Noise
            result = n;
            break;
        case 1: // Fractal Sum
            for (int i = 0; i < numLayers; ++i)
            {
                result += amplitude * abs(2.0 * n - 1.0);
                p *= lacunarity;
                n = noise(p);
                amplitude *= gain;
            }
            break;
        case 2: // Turbulence
            for (int i = 0; i < numLayers; ++i)
            {
                result += amplitude * abs(n);
                p *= lacunarity;
                n = noise(p);
                amplitude *= gain;
            }
            break;
        case 3: // Marble
            vec3 pos = vec3(TexCoord * 64.0, uZ);
            result = 0.0f;
            amplitude = 0.5f;

            for (int i = 0; i < 3; ++i) 
            {
                float n = amplitude * noise(pos);
                result += n;
                pos *= 2.0f;
                amplitude *= 0.5f;
            }

            float MY_PI = 3.1415926535897932384626433832795028f;
            float x_component = sin((pos.x + result * 10.0f) * 2.0f * MY_PI / 25.0f);

            result = (x_component + 1.0f) / 2.0f;
            break;

        case 4: // Wood
            result = 10.0 * n;
            result = fract(result);
            break;
    }

    FragColor = result;
}
