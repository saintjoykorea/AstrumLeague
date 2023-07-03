#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

uniform sampler2D screen;
uniform vec2 resolution;
uniform vec2 projection;

layout (location=0) out lowp vec4 fragColor;

const mediump float n = float((Radius + 1) * (Radius + 1));

void main(void){
    vec2 uv = gl_FragCoord.xy / resolution;
    
    mediump vec3 m[4];
    mediump vec3 s[4];
    for (int k = 0; k < 4; ++k) {
        m[k] = vec3(0.0);
        s[k] = vec3(0.0);
    }
    
    for (int j = -Radius; j <= 0; ++j)  {
        for (int i = -Radius; i <= 0; ++i)  {
            lowp vec3 c = texture(screen, uv + vec2(i, j) / projection).rgb;
            m[0] += c;
            s[0] += c * c;
        }
    }
    
    for (int j = -Radius; j <= 0; ++j)  {
        for (int i = 0; i <= Radius; ++i)  {
            lowp vec3 c = texture(screen, uv + vec2(i, j) / projection).rgb;
            m[1] += c;
            s[1] += c * c;
        }
    }
    
    for (int j = 0; j <= Radius; ++j)  {
        for (int i = 0; i <= Radius; ++i)  {
            lowp vec3 c = texture(screen, uv + vec2(i, j) / projection).rgb;
            m[2] += c;
            s[2] += c * c;
        }
    }
    
    for (int j = 0; j <= Radius; ++j)  {
        for (int i = -Radius; i <= 0; ++i)  {
            lowp vec3 c = texture(screen, uv + vec2(i,j) / projection).rgb;
            m[3] += c;
            s[3] += c * c;
        }
    }
    
    mediump float min_sigma2 = 1e+2;
    for (int k = 0; k < 4; ++k) {
        m[k] /= n;
        s[k] = abs(s[k] / n - m[k] * m[k]);
        
        float sigma2 = s[k].r + s[k].g + s[k].b;
        if (sigma2 < min_sigma2) {
            min_sigma2 = sigma2;
            fragColor = vec4(m[k], 1.0);
        }
    }
}

