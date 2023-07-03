#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

#ifdef UsingScreen
uniform sampler2D screen;

in vec2 varScreenCoord;
#endif

layout (location=0) out lowp vec4 fragColor;

void main(){
#ifdef UsingScreen
	fragColor = texture(screen, varScreenCoord);
#endif
}

