#ifdef GL_ES
precision mediump float;
#endif

struct Light{
    float squaredRadius;
    vec3 color;
};

uniform sampler2D u_noise;
uniform float u_time;
uniform vec2 u_resolution;
uniform vec2 u_origin;

uniform int nSLight;
uniform Light lights[50];
uniform vec3 staticPositions[200];

void main(){
    vec2 st = (u_origin+vec2(gl_FragCoord.x+u_time * 100 ,1-gl_FragCoord.y))/u_resolution;
    st /= 3.0;
    vec3 noise_color = texture2D(u_noise , st).xyz-vec3(0.45);
    float noise_val = noise_color.r * 0.6;

    vec4 final_color = vec4(1.0 , 1.0 , 1.0 , noise_val);

    // vec2 lcoord = (u_origin+vec2(gl_FragCoord.x , 1-gl_FragCoord.y)/3.0);
    // lcoord = floor(lcoord);
    
    // for(int j = 0;j < nSLight;j++){
    //     vec2 dist = staticPositions[j].xy-lcoord;
    //     float squaredDist = dist.x*dist.x + dist.y*dist.y;
    //     if(squaredDist < lights[int(staticPositions[j].z)].squaredRadius*9.0){
    //         final_color += vec4(lights[int(staticPositions[j].z)].color , 0.1);
    //     }
    // }

    gl_FragColor = final_color;
}