#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

#ifdef UsingDepth
# if !defined(UsingLowQuality) && defined(GL_FRAGMENT_PRECISION_HIGH)
#  define UsingDepthTexture
# else
#  define UsingDepthVar
# endif
#endif

#ifdef UsingPerturb
uniform sampler2D perturbTexture;
uniform lowp float perturbWeight;
# ifdef UsingCross
uniform mediump vec2 perturbFlowCross;
# endif
#endif

#ifdef UsingFoam
uniform sampler2D foamTexture;
uniform lowp float foamWeight;
# ifdef UsingFoamShallowDepth
uniform mediump float foamShallowDepth;
# endif
# ifdef UsingCross
uniform mediump vec2 foamFlowCross;
# endif
#endif

uniform lowp vec4 shallowColor;

#ifdef UsingDepthMax
uniform lowp vec4 deepColor;
uniform mediump float depthMax;
#endif

#ifdef UsingDepthTexture
uniform highp sampler2D depthTexture;
uniform float near;
uniform float far;
#endif

#ifdef UsingSpecular
uniform mediump float specPower;
uniform lowp vec3 lightColor;
#endif

#ifndef UsingLowQuality
uniform sampler2D destTexture;
#endif

#ifdef UsingVision
uniform sampler2D visionTexture;
#endif

#ifndef UsingLowQuality
uniform vec2 resolution;
#endif

#ifdef UsingDepthVar
in mediump float varDepth;
#endif

#ifdef UsingDistortion
in vec4 varWavePosition;
#endif

#ifdef UsingPerturb
in vec2 varPerturbCoord;
#endif

#ifdef UsingFoam
in vec2 varFoamCoord;
#endif

#ifdef UsingSpecular
in mediump vec3 varNormal;
in mediump vec3 varSpecular;
#endif

#ifdef UsingVision
in vec2 varVisionCoord;
#endif

layout (location=0) out lowp vec4 fragColor;

#ifdef UsingDepthTexture
float convertToLinearDepth(float d) {
    return (near * far) / (far - d * (far - near));
}
#endif

void main() {
#ifndef UsingLowQuality
    vec2 originCoord = gl_FragCoord.xy / resolution;
#endif
      
#ifdef UsingDepthTexture
    float d = convertToLinearDepth(texture(depthTexture, originCoord).r * 2.0 - 1.0) - (1.0 / gl_FragCoord.w);
    if (d < 0.0) {
        discard;
    }
#elif defined(UsingDepthVar)
    float d = varDepth;
#endif
    
#ifdef UsingDistortion
    vec2 waveCoord = vec2(varWavePosition.x / varWavePosition.w, varWavePosition.y / varWavePosition.w) * 0.5 + 0.5;
#endif

#ifdef UsingPerturb
    vec3 perturbation;
# ifdef UsingCross
    perturbation = (texture(perturbTexture, varPerturbCoord + perturbFlowCross).rgb + texture(perturbTexture, varPerturbCoord - perturbFlowCross).rgb) * 0.5;
# else
    perturbation = texture(perturbTexture, varPerturbCoord).rgb;
# endif
    perturbation = (perturbation * 2.0 - 1.0) * perturbWeight;
# ifdef UsingDistortion
    waveCoord += perturbation.xy;
# endif
#endif
    
    lowp vec4 srcColor;
#ifndef UsingLowQuality
    lowp vec3 destColor;
#endif

#ifdef UsingDepthMax
    float a = smoothstep(0.0, depthMax, d);

    srcColor = mix(shallowColor, deepColor, a);
# ifndef UsingLowQuality
#  ifdef UsingDistortion
    destColor = texture(destTexture, mix(originCoord, waveCoord, a)).rgb;
#  else
    destColor = texture(destTexture, originCoord).rgb;
#  endif
# endif
#else
    srcColor = shallowColor;
# ifndef UsingLowQuality
#  ifdef UsingDistortion
    destColor = texture(destTexture, waveCoord).rgb;
#  else
    destColor = texture(destTexture, originCoord).rgb;
#  endif
# endif
#endif

#ifdef UsingVision
    srcColor.rgb *= texture(visionTexture, varVisionCoord).r;
#endif

#ifdef UsingFoam
    vec3 foamColor;
# ifdef UsingCross
    foamColor = (texture(foamTexture, varFoamCoord + foamFlowCross).rgb + texture(foamTexture, varFoamCoord - foamFlowCross).rgb) * 0.5;
# else
    foamColor = texture(foamTexture, varFoamCoord).rgb;
# endif
    srcColor.rgb *= (foamColor * 2.0 - 1.0) * foamWeight + 1.0;
# ifndef UsingLowQuality    
    destColor = mix(destColor, srcColor.rgb, srcColor.a);
#  ifdef UsingFoamShallowDepth
    if (d <= foamShallowDepth) {
        destColor += max(foamColor - 0.5, 0.0) * (1.0 - d / foamShallowDepth);
    }
#  endif
# endif
#else
# ifndef UsingLowQuality    
    destColor = mix(destColor, srcColor.rgb, srcColor.a);
# endif
#endif

#ifndef UsingLowQuality    
# ifdef UsingSpecular
    vec3 n = varNormal;
#  ifdef UsingPerturb
    n = normalize(n + perturbation);
#  endif
    destColor += lightColor * pow(dot(varSpecular, n), specPower);
# endif
    fragColor = vec4(destColor, 1.0);
#else
	fragColor = srcColor;
	fragColor.rgb *= fragColor.a;
#endif
}
