#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D screen;
uniform vec2 resolution;
uniform vec2 fraction;

layout (location=0) out lowp vec4 fragColor;

void main(){
    vec2 uv = gl_FragCoord.xy / resolution;
    
	uv -= mod(uv, fraction);

	fragColor = texture(screen, uv);
}

