#ifdef GL_ES
precision mediump float;
#endif

#define PI 3.141592

uniform vec2 u_resolution;
uniform float u_time;

void main() {
    vec2 st = (gl_TexCoord[0].xy - gl_TexCoord[0].xy % 1)/u_resolution.xy;
    vec2 vec = vec2(0.5,0.5+0.1*sin(1.75*u_time))-st;
    float radius = length(vec)*2.0;
    float angle = atan(vec.x , vec.y);

    vec3 color = vec3(0.047058823529411764, 0.8019607843137255+0.3*sin(1.5*u_time), 0.8490196078431372);
    float magnitude = 0.15+0.1*cos(1.5*u_time);

    if(radius > 0.75-magnitude*abs(sin(4*angle+2*u_time))){
        color = vec3(1.0);
    }

    gl_FragColor = vec4(color,smoothstep(radius-0.3,radius, 0.6-magnitude*abs(sin(4*angle+2*u_time))));
}