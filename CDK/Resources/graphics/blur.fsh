#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

#define BlurTypeWeight		0
#define BlurTypeDirection	1
#define BlurTypeCenter		2

uniform sampler2D screen;
uniform vec2 resolution;
#if BlurType != BlurTypeCenter
uniform vec2 direction;
#endif

#if BlurType == BlurTypeCenter
uniform vec2 centerCoord;
uniform float range;

in vec3 varOriginPosition;
#endif

layout (location=0) out lowp vec4 fragColor;

void main(){
    vec2 uv = vec2(gl_FragCoord.xy / resolution);
    
    fragColor = vec4(0.0);
#if BlurType == BlurTypeWeight
    vec2 off1 = direction * 1.411764705882353 / resolution;
    vec2 off2 = direction * 3.2941176470588234 / resolution;
    vec2 off3 = direction * 5.176470588235294 / resolution;
    fragColor += texture(screen, uv) * 0.1964825501511404;
    fragColor += texture(screen, uv + off1) * 0.2969069646728344;
    fragColor += texture(screen, uv - off1) * 0.2969069646728344;
    fragColor += texture(screen, uv + off2) * 0.09447039785044732;
    fragColor += texture(screen, uv - off2) * 0.09447039785044732;
    fragColor += texture(screen, uv + off3) * 0.010381362401148057;
    fragColor += texture(screen, uv - off3) * 0.010381362401148057;
#else
#if BlurType == BlurTypeCenter
	vec2 direction = uv - centerCoord;
#endif
	float d = length(direction);
#if BlurType == BlurTypeCenter
	d *= range;
#endif
	float c = ceil(d) + 1.0;
	vec2 off = direction / (resolution * c);
#if BlurType == BlurTypeCenter
	off *= range;
#endif
	float r = 1.0 / c;
	
	for (float i = 0.0; i < c; i += 1.0) {
		fragColor += texture(screen, uv + i * off) * r;
	}
#endif
}

