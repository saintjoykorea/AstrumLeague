#ifdef UsingDepth
# if !defined(UsingLowQuality) && defined(GL_FRAGMENT_PRECISION_HIGH)
#  define UsingDepthTexture
# else
#  define UsingDepthVar
# endif
#endif

uniform mat4 world;
uniform mat4 viewProjection;

uniform mediump float positionScale;

#ifdef UsingPerturb
uniform mediump vec2 perturbScale;
uniform mediump vec2 perturbFlowForward;
#endif

#ifdef UsingFoam
uniform mediump vec2 foamScale;
uniform mediump vec2 foamFlowForward;
#endif

#ifdef UsingWave
uniform float waveProgress;
uniform mediump float waveWidth;
uniform mediump float waveHeight;
#endif

#ifdef UsingSpecular
uniform mediump vec3 lightDirection;
uniform vec3 eye;
#endif

#ifdef UsingVision
uniform mediump vec2 visionScale;
#endif

layout (location=0) in vec3 attrPosition;

#ifdef UsingDepthVar
layout (location=1) in mediump float attrAltitude;

out mediump float varDepth;
#endif

#ifdef UsingDistortion
out vec4 varWavePosition;
#endif

#ifdef UsingPerturb
out vec2 varPerturbCoord;
#endif

#ifdef UsingFoam
out vec2 varFoamCoord;
#endif

#ifdef UsingSpecular
out mediump vec3 varNormal;
out mediump vec3 varSpecular;
#endif

#ifdef UsingVision
out vec2 varVisionCoord;
#endif

void main() {
    vec4 pos = vec4(attrPosition * positionScale, 1.0);
#ifdef UsingWave
# ifdef UsingDistortion
    varWavePosition = viewProjection * world * pos;
# endif
    vec2 gridPosition = waveWidth * attrPosition.xy + waveProgress;
    pos.z += sin(gridPosition.x) * cos(gridPosition.y) * waveHeight;
# ifdef UsingDepthVar
    varDepth = pos.z - attrAltitude * positionScale;
# endif
    pos = world * pos;
    gl_Position = viewProjection * pos;
# ifdef UsingSpecular
    varNormal = normalize(mat3(world) * normalize(vec3(cos(gridPosition.x) * cos(gridPosition.y) * waveWidth * waveHeight,
                                                       sin(gridPosition.x) * sin(gridPosition.y) * waveWidth * waveHeight,
                                                       positionScale)));
# endif
#else
# ifdef UsingDepthVar
    varDepth = pos.z - attrAltitude * positionScale;
# endif
    pos = world * pos;
    gl_Position = viewProjection * pos;
# ifdef UsingDistortion
    varWavePosition = gl_Position;
# endif
# ifdef UsingSpecular
    varNormal = normalize(vec3(world[2][0], world[2][1], world[2][2]));
# endif
#endif

#ifdef UsingSpecular
    varSpecular = normalize(lightDirection + normalize(eye - pos.xyz));
#endif
    
#ifdef UsingPerturb
    varPerturbCoord = attrPosition.xy * perturbScale + perturbFlowForward;
#endif
    
#ifdef UsingFoam
    varFoamCoord = attrPosition.xy * foamScale + foamFlowForward;
#endif
    
#ifdef UsingVision
    varVisionCoord = attrPosition.xy * visionScale;
#endif
}
