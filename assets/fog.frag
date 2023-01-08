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
uniform float u_scale;

uniform int nSLight;
uniform Light lights[50];
uniform vec3 staticPositions[200];

void main(){
    vec2 st = (u_origin*3.0+vec2(gl_TexCoord[0].x-u_time * 50 , gl_TexCoord[0].y))/u_resolution;
    st /= 3.0;
    vec3 noise_color = texture2D(u_noise , st).xyz-vec3(0.45);
    float noise_val = noise_color.r * 0.75;

    vec4 final_color = vec4(1.0 , 1.0 , 1.0 , noise_val);

    vec2 lcoord = (u_origin+gl_TexCoord[0].xy/u_scale);
    lcoord = floor(lcoord);
    
    for(int j = 0;j < nSLight;j++){
        float lightRadius = lights[int(staticPositions[j].z)].squaredRadius+100*sin(u_time);
        vec2 dist = staticPositions[j].xy-lcoord;
        float squaredDist = dist.x*dist.x + dist.y*dist.y;
        if(squaredDist < lightRadius*u_scale){
            float alpha = (lightRadius*u_scale-squaredDist)/(lightRadius*u_scale);
            final_color += vec4(lights[int(staticPositions[j].z)].color , 0.175*alpha);
        }
    }

    gl_FragColor = final_color;
}