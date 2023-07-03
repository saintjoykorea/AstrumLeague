uniform mat4 worldViewProjection;

layout (location=0) in vec3 attrPosition;

void main(){
    gl_Position = worldViewProjection * vec4(attrPosition, 1.0);
}

