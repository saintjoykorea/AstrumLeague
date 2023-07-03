#define MaxLightAmbientDiffuse     1.2

uniform mat4 viewProjection;

#if defined(Light) || defined(DepthBiasPerspective)
uniform vec3 eye;
#endif

#ifdef World
uniform mat4 world;
#endif

#ifdef Shadow
uniform mat4 shadow;
#endif

//Color 0:vertex only 1:uniform only 2:all

#if Color >= 1
uniform lowp vec4 color;
#endif

#ifdef Offset
uniform vec2 offset;
#endif

#ifdef Light
uniform lowp float lightAmbient;
uniform lowp vec3 lightColor;
uniform mediump vec3 lightDirection;

uniform lowp vec4 materialDiffuse;
#endif

#if defined(DepthBiasOrtho) || defined(DepthBiasPerspective)
uniform float depthBias;
#endif
#ifdef DepthBiasPerspective
uniform float depthBiasPerspective;
#endif

layout (location=0) in vec3 attrPosition;

#if Color != 1
layout (location=1) in lowp vec4 attrColor;
#endif
out lowp vec4 varColor;

#ifdef Texture
layout (location=2) in vec2 attrTextureCoord;
out vec2 varTextureCoord;
#endif

#ifdef Light
layout (location=3) in mediump vec3 attrNormal;
out lowp float varSpecular;
#endif

#ifdef Shadow
out float varShadowZ;
#endif

void main(){
    vec4 pos;
#ifdef World
    pos = world * vec4(attrPosition, 1.0);
#else
    pos = vec4(attrPosition, 1.0);
#endif
    
#ifdef Shadow
    varShadowZ = pos.z / pos.w;
    pos = shadow * pos;
#endif
    
#ifdef DepthBiasOrtho
    pos.z += depthBias * pos.w;
#endif
    
    gl_Position = viewProjection * pos;
#ifdef Offset
    gl_Position.xy += offset * gl_Position.w;
#endif
    
#ifdef DepthBiasPerspective
    float eyed = distance(pos.xyz / pos.w, eye);
    gl_Position.z = (gl_Position.z + depthBiasPerspective) * eyed / (eyed + depthBias);
#endif
    
#if Color == 0
    varColor = attrColor;
#elif Color == 1
    varColor = color;
#else
    varColor = attrColor * color;
#endif
    
#ifdef Light
#ifdef World
    vec3 n = normalize(mat3(world) * attrNormal);
#else
    vec3 n = attrNormal;
#endif
    vec3 h = normalize(lightDirection + normalize(eye - pos.xyz / pos.w));
    
    vec3 ambient = varColor.rgb * lightAmbient;
    vec3 diffuse = max(dot(n, lightDirection), 0.0) * lightColor;
    
    varColor.rgb = materialDiffuse.rgb * min(ambient + diffuse, MaxLightAmbientDiffuse);
    varColor.a *= materialDiffuse.a;
    
    varSpecular = dot(n, h);
#endif
    
#ifdef Texture
	varTextureCoord = attrTextureCoord;
#endif
}

