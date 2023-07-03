uniform mat4 world;
uniform mat4 viewProjection;
uniform lowp float alpha;

layout (location=0) in vec3 attrPosition;
layout (location=1) in lowp vec3 attrColor;

out lowp vec4 varColor;

void main() {
    gl_Position = viewProjection * (world * vec4(attrPosition, 1.0));
    
    varColor = vec4(attrColor * alpha, alpha);
}

