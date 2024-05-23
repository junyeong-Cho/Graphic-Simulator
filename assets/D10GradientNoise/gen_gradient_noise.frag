#version 460

uniform float uZ;
uniform int uPattern;

out float FragColor;

const int tableSize = 256;
const int tableSizeMask = tableSize - 1;
int permutationTable[tableSize * 2];
vec3 gradients[tableSize];

void initPermutationTable(int seed) {
    ivec2 s = ivec2(seed, seed + 1);
    for (int i = 0; i < tableSize; ++i) {
        permutationTable[i] = i;
    }
    for (int i = tableSize - 1; i > 0; --i) {
        s = ivec2(s.y, (s.x + s.y) % 255);
        int j = s.y % (i + 1);
        int temp = permutationTable[i];
        permutationTable[i] = permutationTable[j];
        permutationTable[j] = temp;
    }
    for (int i = 0; i < tableSize; ++i) {
        permutationTable[tableSize + i] = permutationTable[i];
    }
}

void initGradients(int seed) 
{
    vec3[12] fixedDirections = vec3[12](vec3(1, 1, 0), vec3(-1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0),
                                        vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
                                        vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, 1, -1), vec3(0, -1, -1));
    for (int i = 0; i < tableSize; ++i) 
    {
        gradients[i] = fixedDirections[i % 12];
    }
}


vec3 grad(int hash) {
    return gradients[hash & 15];
}

float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float noise(vec3 p) {
    vec3 Pi = floor(p);
    vec3 Pf = fract(p);

    Pi = mod(Pi, 64.0);

    int ix = int(Pi.x) & tableSizeMask;
    int iy = int(Pi.y) & tableSizeMask;
    int iz = int(Pi.z) & tableSizeMask;

    int ixy = permutationTable[ix + permutationTable[iy]];
    int ixy1 = permutationTable[ix + 1 + permutationTable[iy + 1]];

    int g000 = permutationTable[ixy + iz];
    int g001 = permutationTable[ixy + iz + 1];
    int g010 = permutationTable[ixy + 1 + iz];
    int g011 = permutationTable[ixy + 1 + iz + 1];
    int g100 = permutationTable[ixy1 + iz];
    int g101 = permutationTable[ixy1 + iz + 1];
    int g110 = permutationTable[ixy1 + 1 + iz];
    int g111 = permutationTable[ixy1 + 1 + iz + 1];

    vec3 grad000 = grad(g000);
    vec3 grad001 = grad(g001);
    vec3 grad010 = grad(g010);
    vec3 grad011 = grad(g011);
    vec3 grad100 = grad(g100);
    vec3 grad101 = grad(g101);
    vec3 grad110 = grad(g110);
    vec3 grad111 = grad(g111);

    vec3 fadePf = vec3(fade(Pf.x), fade(Pf.y), fade(Pf.z));

    float n000 = dot(grad000, Pf);
    float n100 = dot(grad100, Pf - vec3(1.0, 0.0, 0.0));
    float n010 = dot(grad010, Pf - vec3(0.0, 1.0, 0.0));
    float n110 = dot(grad110, Pf - vec3(1.0, 1.0, 0.0));
    float n001 = dot(grad001, Pf - vec3(0.0, 0.0, 1.0));
    float n101 = dot(grad101, Pf - vec3(1.0, 0.0, 1.0));
    float n011 = dot(grad011, Pf - vec3(0.0, 1.0, 1.0));
    float n111 = dot(grad111, Pf - vec3(1.0, 1.0, 1.0));

    float n00 = lerp(fadePf.x, n000, n100);
    float n10 = lerp(fadePf.x, n010, n110);
    float n01 = lerp(fadePf.x, n001, n101);
    float n11 = lerp(fadePf.x, n011, n111);

    float n0 = lerp(fadePf.y, n00, n10);
    float n1 = lerp(fadePf.y, n01, n11);

    return lerp(fadePf.z, n0, n1);
}

void main() {
    int seed = 2016;  // Seed for permutation table and gradients
    initPermutationTable(seed);
    initGradients(seed);

    vec3 p = vec3(gl_FragCoord.xy, uZ);
    float n = noise(p);
    n = (n + 1.0) / 2.0;

    float result = 0.0;
    const int numLayers = 5;
    float amplitude = 0.5;
    float frequency = 1.0;

    switch (uPattern) {
        case 0: // Plain Gradient Noise
            result = n;
            break;
        case 1: // Fractal Sum
            for (int i = 0; i < numLayers; ++i) 
            {
                result += n * amplitude;
                p *= 2.0;
                n = noise(p);
                amplitude *= 0.5;
            }
            break;
        case 2: // Turbulence
            for (int i = 0; i < numLayers; ++i) 
            {
                result += abs(2.0 * n - 1.0) * amplitude;
                p *= 2.0;
                n = noise(p);
                amplitude *= 0.5;
            }
            break;
        case 3: // Marble
            for (int i = 0; i < numLayers; ++i) 
            {
                result += n * amplitude;
                p *= 2.0;
                n = noise(p);
                amplitude *= 0.5;
            }
            result = sin((gl_FragCoord.x + result * 10.0) * 3.14159265 / 180.0);
            result = (result + 1.0) / 2.0;
            break;
        case 4: // Wood
            result = n * 10.0;
            result = fract(result);
            break;
    }

    FragColor = result;
}