#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D visionTexture;

in vec2 varVisionCoord;

layout (location=0) out lowp vec4 fragColor;

void main(void){
	lowp float v = texture(visionTexture, varVisionCoord).r;
	
    fragColor = vec4(v, v, v, 1.0);
}
