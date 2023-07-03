#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D screen;
uniform vec2 resolution;
uniform vec3 center;
uniform float range;
uniform float angle;

in vec3 varOriginPosition;
in vec4 varCenterPosition;

layout (location=0) out lowp vec4 fragColor;

void main() {
    vec2 screenCoord = gl_FragCoord.xy / resolution;
    vec2 centerCoord = vec2(varCenterPosition.x / varCenterPosition.w, varCenterPosition.y / varCenterPosition.w) * 0.5 + 0.5;
    
	float distance = distance(center, varOriginPosition);

    if (distance < range) {
		vec2 screenCoordToUse = screenCoord - centerCoord;
		float percent = (range - distance) / range;
		float theta = percent * percent * angle * 8.0;
		float s = sin(theta);
		float c = cos(theta);
		screenCoordToUse = vec2(dot(screenCoordToUse, vec2(c, -s)), dot(screenCoordToUse, vec2(s, c)));
		screenCoordToUse += centerCoord;

        fragColor = texture(screen, screenCoordToUse);
	}
    else {
        discard;
    }
}

