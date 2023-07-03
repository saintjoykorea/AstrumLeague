#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D screen;
uniform vec2 resolution;
uniform vec3 center;
uniform float range;
uniform lowp float convex;

in vec3 varOriginPosition;
in vec4 varCenterPosition;

layout (location=0) out lowp vec4 fragColor;

void main() {
    vec2 screenCoord = gl_FragCoord.xy / resolution;
    vec2 centerCoord = vec2(varCenterPosition.x / varCenterPosition.w, varCenterPosition.y / varCenterPosition.w) * 0.5 + 0.5;
    
	float distance = distance(varOriginPosition, center);

	if (distance < range) {
        float diff = 1.0 - (convex * (range - distance) / range);
        vec2 diffuv = screenCoord - centerCoord;
        
        fragColor = texture(screen, centerCoord + diffuv * diff);
	}
    else {
        discard;
    }
}
