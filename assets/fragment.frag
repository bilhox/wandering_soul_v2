struct Light{
    vec2 position;
    float radius;
    float intensity;
    vec3 color;
};

uniform vec2 viewOrigin;
uniform int nLight;
uniform Light lights[50];
uniform vec3 positions[700];
layout(origin_upper_left) in vec4 gl_FragCoord;

void main()
{
    vec4 final_color = vec4(1, 0, 0, 0);
    for(int i = 0;i < nLight;i++){
        float dist = distance(positions[i].xy-viewOrigin,gl_FragCoord.xy);
        if(dist < lights[int(positions[i].z)].radius){
            final_color = vec4(lights[int(positions[i].z)].color , lights[int(positions[i].z)].intensity);
            break;
        }
    }
    gl_FragColor = final_color;
}