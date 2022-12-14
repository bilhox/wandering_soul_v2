struct Light{
    vec2 position;
    float radius;
    float intensity;
    vec3 color;
};

uniform vec2 viewOrigin;
uniform int nLight;
uniform Light lights[700];
layout(origin_upper_left) in vec4 gl_FragCoord;

void main()
{
    vec4 final_color = vec4(0, 0, 0, 0);
    for(int i = 0;i < nLight;i++){
        float dist = distance(lights[i].position-viewOrigin,gl_FragCoord.xy);
        if(dist < lights[i].radius){
            final_color = vec4(lights[i].color , lights[i].intensity);
            break;
        }
    }
    gl_FragColor = final_color;
}