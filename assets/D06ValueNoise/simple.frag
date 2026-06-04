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

in vec2 vTextureCoordinates;

uniform sampler2D uTex2d;

layout(location = 0) out vec4 fFragmentColor;

void main()
{
    fFragmentColor = texture(uTex2d, vTextureCoordinates);
}