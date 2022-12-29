#ifdef GL_ES
precision mediump float;
#endif

#define PI 3.141592

uniform vec2 u_resolution;
uniform float u_time;

void main() {
    vec2 st = (gl_TexCoord[0].xy);
    st /= u_resolution;
    vec2 vec = vec2(0.5,0.5+0.1*sin(1.75*u_time))-st;
    float radius = length(vec)*2.0;
    float angle = atan(vec.x , vec.y);

    vec3 color = vec3(0.047058823529411764, 0.9019607843137255, 0.9490196078431372);
    float magnitude = 0.15+0.1*cos(2*u_time);

    if(radius > 0.75-magnitude*abs(sin(4*angle+2*u_time))){
        color = vec3(1.0);
    }

    gl_FragColor = vec4(st.x , st.y , 0.0 , 1.0);
    gl_FragColor = vec4(color,smoothstep(radius-0.1,radius+(0.1+0.1*cos(u_time)), 0.7-magnitude*abs(sin(4*angle+2*u_time))));
}