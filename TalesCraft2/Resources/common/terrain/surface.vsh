uniform mat4 world;
uniform mat4 viewProjection;
uniform float positionScale;

layout (location=0) in vec3 attrPosition;
#ifdef Surface
layout (location=1) in mediump vec3 attrNormal;
layout (location=2) in mediump vec2 attrLightCoord;

uniform vec3 eye;
uniform mediump vec3 lightDirection;

uniform vec2 surfaceOffset;
uniform vec2 surfaceScale;
uniform float surfaceRotation;
uniform bool shadowVisible;
uniform vec2 shadowOffset;
uniform vec2 shadowScale;

out mediump vec2 varSurfaceCoord;
out mediump vec2 varLightCoord;
out mediump vec2 varShadowCoord;
out lowp float varDiffuse;
out lowp float varSpecular;
#endif

void main() {
    vec4 pos = world * vec4(attrPosition * positionScale, 1.0);
    
    gl_Position = viewProjection * pos;
#ifdef Surface
    varLightCoord = attrLightCoord;
    varSurfaceCoord = (attrPosition.xy + surfaceOffset) * surfaceScale;
    if (surfaceRotation != 0.0) {
        float sinq = sin(surfaceRotation);
        float cosq = cos(surfaceRotation);
        varSurfaceCoord = varSurfaceCoord.x * vec2(cosq, -sinq) + varSurfaceCoord.y * vec2(sinq, cosq);
    }

	if (shadowVisible) {
		varShadowCoord = (attrPosition.xy - shadowOffset) * shadowScale;
		varShadowCoord.y = 1.0 - varShadowCoord.y;
	}

    vec3 n = normalize(mat3(world) * attrNormal);
    vec3 h = normalize(lightDirection + normalize(eye - pos.xyz / pos.w));
    varDiffuse = dot(n, lightDirection);
    varSpecular = dot(n, h);
#endif
}

