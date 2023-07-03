#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

#define MaxLightAmbientDiffuse     1.2

#ifdef Surface
uniform sampler2D surfaceTexture;
uniform sampler2D lightTexture;
uniform sampler2D shadowTexture;

uniform lowp vec4 color;
uniform lowp float lightAmbient;
uniform lowp vec3 lightColor;
uniform lowp vec4 materialDiffuse;
uniform lowp float materialSpecular;
uniform mediump float materialShininess;
uniform lowp vec3 materialEmission;
uniform bool shadowVisible;

in mediump vec2 varSurfaceCoord;
in mediump vec2 varLightCoord;
in mediump vec2 varShadowCoord;
in lowp float varDiffuse;
in lowp float varSpecular;
#endif

layout (location=0) out lowp vec4 fragColor;

void main(void){
#ifdef Surface
    lowp vec3 surface = texture(surfaceTexture, varSurfaceCoord).rgb;
    lowp vec3 light = texture(lightTexture, varLightCoord).rgb;
    lowp float shadow = shadowVisible ? min(texture(shadowTexture, varShadowCoord).r, light.b) : light.b;

    lowp vec3 ambient = color.rgb * lightAmbient * light.r * 2.0;
    lowp vec3 diffuse = max(varDiffuse, 0.0) * lightColor * shadow;
    lowp vec3 specular = materialSpecular * pow(max(varSpecular, 0.0), materialShininess) * lightColor;
    
    fragColor.rgb = min(surface * materialDiffuse.rgb * min(ambient + diffuse, MaxLightAmbientDiffuse) + specular + materialEmission, 1.0);
    fragColor.a = color.a * materialDiffuse.a * light.g;
    
    fragColor.rgb *= fragColor.a;
#else
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}
