#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

#ifdef AlphaTest
uniform lowp float alphaTest;
#endif

#ifdef Texture
uniform sampler2D texture0;
# if Texture == 2
uniform sampler2D texture1;
# endif
in vec2 varTextureCoord;
# ifdef Convolution
uniform vec2 convolutionTextureOffset;
uniform float convolution[(Convolution * 2 + 1) * (Convolution * 2 + 1)];
# endif
#endif

#ifdef Light
uniform lowp vec3 lightColor;
uniform lowp float materialSpecular;
uniform mediump float materialShininess;
uniform lowp vec3 materialEmission;

in lowp float varSpecular;
#endif

#ifdef Shadow
in float varShadowZ;
#endif

in lowp vec4 varColor;

layout (location=0) out lowp vec4 fragColor;

#ifdef Texture
lowp vec4 sampleTexture(vec2 textureCoord) {
# if Texture == 1
    return texture(texture0, textureCoord);
# elif Texture == 2
    return vec4(texture(texture0, textureCoord).rgb, texture(texture1, textureCoord).r);
# elif Texture == 3
	return vec4(1.0, 1.0, 1.0, texture(texture0, textureCoord).r);
# elif Texture == 4
	lowp float c = texture(texture0, textureCoord).r;
	return vec4(c, c, c, 1.0);
# elif Texture == 5
	lowp vec2 c = texture(texture0, textureCoord).rg;
	return vec4(c.r, c.r, c.r, c.g);
# endif
}
#endif

void processSource() {
#ifdef Texture
# ifdef Convolution
    lowp vec4 centerColor = sampleTexture(varTextureCoord);
    fragColor = vec4(0.0, 0.0, 0.0, centerColor.a);
#  if Convolution == 1
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x, -convolutionTextureOffset.y)).rgb * convolution[0];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(0.0, -convolutionTextureOffset.y)).rgb * convolution[1];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x, -convolutionTextureOffset.y)).rgb * convolution[2];
    
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x, 0.0)).rgb * convolution[3];
    fragColor.rgb += centerColor.rgb * convolution[4];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x, 0.0)).rgb * convolution[5];
    
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x, convolutionTextureOffset.y)).rgb * convolution[6];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(0.0, convolutionTextureOffset.y)).rgb * convolution[7];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x, convolutionTextureOffset.y)).rgb * convolution[8];
#  elif Convolution == 2
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x*2.0, -convolutionTextureOffset.y*2.0)).rgb * convolution[0];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x, -convolutionTextureOffset.y*2.0)).rgb * convolution[1];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(0.0, -convolutionTextureOffset.y*2.0)).rgb * convolution[2];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x, -convolutionTextureOffset.y*2.0)).rgb * convolution[3];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x*2.0, -convolutionTextureOffset.y*2.0)).rgb * convolution[4];
    
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x*2.0, -convolutionTextureOffset.y)).rgb * convolution[5];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x, -convolutionTextureOffset.y)).rgb * convolution[6];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(0.0, -convolutionTextureOffset.y)).rgb * convolution[7];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x, -convolutionTextureOffset.y)).rgb * convolution[8];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x*2.0, -convolutionTextureOffset.y)).rgb * convolution[9];
    
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x*2.0, 0.0)).rgb * convolution[10];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x, 0.0)).rgb * convolution[11];
    fragColor.rgb += centerColor.rgb * convolution[12];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x, 0.0)).rgb * convolution[13];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x*2.0, 0.0)).rgb * convolution[14];

    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x*2.0, convolutionTextureOffset.y)).rgb * convolution[15];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x, convolutionTextureOffset.y)).rgb * convolution[16];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(0.0, convolutionTextureOffset.y)).rgb * convolution[17];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x, convolutionTextureOffset.y)).rgb * convolution[18];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x*2.0, convolutionTextureOffset.y)).rgb * convolution[19];
    
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x*2.0, convolutionTextureOffset.y*2.0)).rgb * convolution[20];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(-convolutionTextureOffset.x, convolutionTextureOffset.y*2.0)).rgb * convolution[21];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(0.0, -convolutionTextureOffset.y*2.0)).rgb * convolution[22];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x, convolutionTextureOffset.y*2.0)).rgb * convolution[23];
    fragColor.rgb += sampleTexture(varTextureCoord + vec2(convolutionTextureOffset.x*2.0, convolutionTextureOffset.y*2.0)).rgb * convolution[24];
#  endif
# else
    fragColor = sampleTexture(varTextureCoord);
# endif
# if defined(MaskingModeRed)
    fragColor = vec4(1.0, 1.0, 1.0, fragColor.r);
# elif defined(MaskingModeGreen)
    fragColor = vec4(1.0, 1.0, 1.0, fragColor.g);
# elif defined(MaskingModeBlue)
    fragColor = vec4(1.0, 1.0, 1.0, fragColor.b);
# elif defined(MaskingModeAlpha)
    fragColor = vec4(1.0, 1.0, 1.0, fragColor.a);
# endif
    fragColor *= varColor;
#else
    fragColor = varColor;
#endif
    
#ifdef Light
    lowp vec3 specular = materialSpecular * pow(max(varSpecular, 0.0), materialShininess) * lightColor;
    
    fragColor.rgb += specular + materialEmission;
#endif
}

#ifdef Lightness
uniform lowp float lightness;

void processLightness(){
    fragColor.rgb += lightness;
}
#endif

#ifdef Brightness
uniform lowp float brightness;

void processBrightness(){
    fragColor.rgb *= brightness;
}
#endif

#ifdef Contrast
uniform lowp float contrast;

void processContrast(){
	const vec3 LumAvg = vec3(0.5, 0.5, 0.5);

	fragColor.rgb = mix(LumAvg, fragColor.rgb, contrast);
}
#endif

#ifdef Saturation
uniform lowp float saturation;

void processSaturation(){
	const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
	vec3 intensity = vec3(dot(fragColor.rgb, LumCoeff));
	fragColor.rgb = mix(intensity, fragColor.rgb, saturation);
}
#endif

void main(){
#ifdef Shadow
    if (varShadowZ < 0.0) discard;
#endif
    processSource();
#ifdef AlphaTest
    if (fragColor.a <= alphaTest)
        discard;
#endif
#ifdef Lightness
    processLightness();
#endif
#ifdef Brightness
    processBrightness();
#endif
#ifdef Contrast
    processContrast();
#endif
#ifdef Saturation
    processSaturation();
#endif
    fragColor.rgb = min(fragColor.rgb, 1.0) * fragColor.a;
}
