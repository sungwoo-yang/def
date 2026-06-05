#version 300 es
precision highp float;

/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;

out vec3 vNormalInViewSpace;
out vec3 vPositionInViewSpace;
out vec4 vPositionInShadowSpace;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;
uniform mat4 uShadowMatrix;

void main()
{
    // TODO set gl_Position to position in clip space
    // TODO set vNormalInViewSpace to be normal in view space 
    // TODO set vPositionInViewSpace to be position in view space
    // TODO set vPositionInShadowSpace to be in shadow space
    gl_Position = vec4(aVertexPosition, 1.0);
}
