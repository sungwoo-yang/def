#version 300 es
precision       highp float;
precision highp sampler2DShadow;

/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) out vec4 fFragmentColor;

in vec3 vNormalInViewSpace;
in vec3 vPositionInViewSpace;
in vec4 vPositionInShadowSpace;

// TODO use the shadpw map
// uniform sampler2DShadow uShadowMap;

uniform vec3  uFogColor;
uniform float uFogDensity;
uniform vec3  uDiffuse;
uniform vec3  uAmbient;
uniform float uShininess;
uniform vec3  uSpecularColor;
uniform vec3  uLightPosition;
uniform bool  uDoShadowBehindLight;

void main()
{
    vec3       n    = normalize(vNormalInViewSpace);
    vec3       l    = normalize(uLightPosition - vPositionInViewSpace);
    float      nl   = max(dot(n, l), 0.0f);
    const vec3 eye  = vec3(0, 0, 1);
    vec3       h    = normalize(l + eye);
    float      spec = max(0.0f, dot(n, h));
    spec            = pow(spec, uShininess);
    vec3 diffuse    = nl * uDiffuse;

    // TODO get shadow value from doing a projection texture lookup
    float shadow = 1.0;
    //TODO if not uDoShadowBehindLight and z position in shadow space is less than 0
        // hardcode shadow value to 1 to make full light behind the light source
    

    // If the fragment is in shadow, use ambient light only.
    vec3        color       = uAmbient + shadow * (diffuse + spec * uSpecularColor);
    // TODO apply gamma correction with  pow(color, vec3(1.0f / 2.2f))

    // TODO apply fogdensity
    float       fogAmount   = 0.0f;
    fFragmentColor          = vec4(mix(color, uFogColor, fogAmount), 1.0f);
}
