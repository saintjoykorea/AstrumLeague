uniform mat4 world;
uniform mat4 viewProjection;
uniform float positionScale;
uniform vec2 visionScale;

layout (location=0) in vec3 attrPosition;

out vec2 varVisionCoord;

void main(){
    gl_Position = viewProjection * (world * vec4(attrPosition * positionScale, 1.0));
    
    varVisionCoord = attrPosition.xy * visionScale;
}

