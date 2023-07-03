layout (location=0) in vec2 attrPosition;

#ifdef UsingScreen
layout (location=1) in vec2 attrScreenCoord;

out vec2 varScreenCoord;
#endif

void main(){
	gl_Position = vec4(attrPosition, 0.0, 1.0);
#ifdef UsingScreen
	varScreenCoord = attrScreenCoord;
#endif
}

