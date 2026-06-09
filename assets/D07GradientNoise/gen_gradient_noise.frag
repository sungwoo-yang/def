#version 300 es
precision highp float;

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

in vec2 vTextureCoordinates;

uniform float uZ;
uniform int uPattern;

layout(location = 0) out vec4 fFragmentColor;

const float PERIOD = 64.0;
const float PI = 3.1415926535897932384626433832795;

const int permutations[128] = int[](
    // size 64
    27, 33, 14, 52, 24, 36, 46, 40, 26, 7, 49, 57, 59, 2, 42, 61,
    9, 3, 12, 63, 37, 53, 17, 8, 44, 35, 30, 22, 6, 18, 60, 55,
    31, 13, 21, 5, 47, 25, 38, 28, 32, 45, 43, 48, 23, 58, 62, 41,
    11, 29, 34, 54, 0, 1, 20, 19, 16, 4, 15, 50, 10, 39, 56, 51,

    // repeat
    27, 33, 14, 52, 24, 36, 46, 40, 26, 7, 49, 57, 59, 2, 42, 61,
    9, 3, 12, 63, 37, 53, 17, 8, 44, 35, 30, 22, 6, 18, 60, 55,
    31, 13, 21, 5, 47, 25, 38, 28, 32, 45, 43, 48, 23, 58, 62, 41,
    11, 29, 34, 54, 0, 1, 20, 19, 16, 4, 15, 50, 10, 39, 56, 51
);

int wrapPeriod(int value)
{
    return value & 63;
}

int permute(int value)
{
    return permutations[value & 127];
}

int hash3D(int x, int y, int z)
{
    int hashX = permute(wrapPeriod(x));
    int hashY = permute(hashX + wrapPeriod(y));
    int hashZ = permute(hashY + wrapPeriod(z));

    return hashZ;
}

vec3 gradientDirection(int hashValue)
{
    int gradientIndex = hashValue % 12;

    if (gradientIndex == 0)
    {
        return vec3(1.0, 1.0, 0.0);
    }
    else if (gradientIndex == 1)
    {
        return vec3(-1.0, 1.0, 0.0);
    }
    else if (gradientIndex == 2)
    {
        return vec3(1.0, -1.0, 0.0);
    }
    else if (gradientIndex == 3)
    {
        return vec3(-1.0, -1.0, 0.0);
    }
    else if (gradientIndex == 4)
    {
        return vec3(1.0, 0.0, 1.0);
    }
    else if (gradientIndex == 5)
    {
        return vec3(-1.0, 0.0, 1.0);
    }
    else if (gradientIndex == 6)
    {
        return vec3(1.0, 0.0, -1.0);
    }
    else if (gradientIndex == 7)
    {
        return vec3(-1.0, 0.0, -1.0);
    }
    else if (gradientIndex == 8)
    {
        return vec3(0.0, 1.0, 1.0);
    }
    else if (gradientIndex == 9)
    {
        return vec3(0.0, -1.0, 1.0);
    }
    else if (gradientIndex == 10)
    {
        return vec3(0.0, 1.0, -1.0);
    }

    return vec3(0.0, -1.0, -1.0);
}

float fade(float t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float gradientDot(int latticeX, int latticeY, int latticeZ, vec3 offset)
{
    int hashValue = hash3D(latticeX, latticeY, latticeZ);
    vec3 gradient = gradientDirection(hashValue);

    return dot(gradient, offset);
}

float gradientNoise(vec3 position)
{
    ivec3 lattice0 = ivec3(floor(position));
    ivec3 lattice1 = lattice0 + ivec3(1);

    vec3 localPosition = fract(position);
    vec3 faded = vec3(
        fade(localPosition.x),
        fade(localPosition.y),
        fade(localPosition.z)
    );

    float n000 = gradientDot(lattice0.x, lattice0.y, lattice0.z, localPosition - vec3(0.0, 0.0, 0.0));
    float n100 = gradientDot(lattice1.x, lattice0.y, lattice0.z, localPosition - vec3(1.0, 0.0, 0.0));
    float n010 = gradientDot(lattice0.x, lattice1.y, lattice0.z, localPosition - vec3(0.0, 1.0, 0.0));
    float n110 = gradientDot(lattice1.x, lattice1.y, lattice0.z, localPosition - vec3(1.0, 1.0, 0.0));

    float n001 = gradientDot(lattice0.x, lattice0.y, lattice1.z, localPosition - vec3(0.0, 0.0, 1.0));
    float n101 = gradientDot(lattice1.x, lattice0.y, lattice1.z, localPosition - vec3(1.0, 0.0, 1.0));
    float n011 = gradientDot(lattice0.x, lattice1.y, lattice1.z, localPosition - vec3(0.0, 1.0, 1.0));
    float n111 = gradientDot(lattice1.x, lattice1.y, lattice1.z, localPosition - vec3(1.0, 1.0, 1.0));

    float x00 = mix(n000, n100, faded.x);
    float x10 = mix(n010, n110, faded.x);
    float x01 = mix(n001, n101, faded.x);
    float x11 = mix(n011, n111, faded.x);

    float y0 = mix(x00, x10, faded.y);
    float y1 = mix(x01, x11, faded.y);

    return mix(y0, y1, faded.z);
}

float remapTo01(float value)
{
    return clamp(value * 0.5 + 0.5, 0.0, 1.0);
}

float fractalSum(vec3 position)
{
    float amplitude = 0.5;
    float sum = 0.0;
    float totalAmplitude = 0.0;

    for (int layer = 0; layer < 5; ++layer)
    {
        sum += gradientNoise(position) * amplitude;
        totalAmplitude += amplitude;

        position *= 2.0;
        amplitude *= 0.5;
    }

    return remapTo01(sum / totalAmplitude);
}

float turbulence(vec3 position)
{
    float amplitude = 0.5;
    float sum = 0.0;
    float totalAmplitude = 0.0;

    for (int layer = 0; layer < 5; ++layer)
    {
        sum += abs(gradientNoise(position)) * amplitude;
        totalAmplitude += amplitude;

        position *= 2.0;
        amplitude *= 0.5;
    }

    return clamp(sum / totalAmplitude, 0.0, 1.0);
}

float marble(vec3 position)
{
    float noiseValue = fractalSum(position);
    float wave = sin((position.x / 8.0 + noiseValue * 2.0) * 2.0 * PI);

    return wave * 0.5 + 0.5;
}

float wood(vec3 position)
{
    float noiseValue = fractalSum(position);
    float rings = fract((position.x + position.y) / 8.0 + noiseValue * 0.75);

    return rings;
}

float evaluatePattern(vec3 position)
{
    if (uPattern == 1)
    {
        return fractalSum(position);
    }
    else if (uPattern == 2)
    {
        return turbulence(position);
    }
    else if (uPattern == 3)
    {
        return marble(position);
    }
    else if (uPattern == 4)
    {
        return wood(position);
    }

    return remapTo01(gradientNoise(position));
}

void main()
{
    vec3 noisePosition = vec3(vTextureCoordinates * PERIOD, uZ);
    float noiseValue = evaluatePattern(noisePosition);

    fFragmentColor = vec4(noiseValue, noiseValue, noiseValue, 1.0);
}