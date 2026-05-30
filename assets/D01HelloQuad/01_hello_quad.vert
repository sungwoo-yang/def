#version 300 es
precision highp float;

/**
 * \file
 * \author Rudy Castan
 * \author Sungwoo Yang
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec3 vColor;
out vec2 vTextureCoordinates;

uniform float uTime;

void main()
{
    float scale = 0.85 + 0.15 * sin(uTime * 2.0);
    float wave  = 0.04 * sin(uTime * 3.0 + aVertexPosition.y * 8.0);

    vec3 animatedPosition = aVertexPosition;
    animatedPosition.x += wave;
    animatedPosition.xy *= scale;

    gl_Position         = vec4(animatedPosition, 1.0);
    vColor              = aVertexColor;
    vTextureCoordinates = aVertexTextureCoordinates;
}
