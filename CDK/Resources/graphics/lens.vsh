uniform mat4 worldViewProjection;
uniform vec3 center;

layout (location=0) in vec3 attrPosition;

out vec3 varOriginPosition;
out vec4 varCenterPosition;

void main(){
    gl_Position = worldViewProjection * vec4(attrPosition, 1.0);

    varOriginPosition = attrPosition;
    varCenterPosition = worldViewProjection * vec4(center, 1.0);
}

