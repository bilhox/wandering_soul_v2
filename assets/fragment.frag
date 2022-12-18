struct Light{
    float squaredRadius;
    vec3 color;
};

uniform vec2 viewOrigin;
uniform int nLight;
uniform Light lights[50];
uniform vec3 positions[700];
layout(origin_upper_left) in vec4 gl_FragCoord;

void main()
{
    vec4 final_color = vec4(0, 0, 0, 0);
    for(int i = 0;i < nLight;i++){
        vec2 dist = positions[i].xy-viewOrigin-gl_FragCoord.xy;
        float squaredDist = dist.x*dist.x + dist.y*dist.y;
        if(squaredDist < lights[int(positions[i].z)].squaredRadius){
            final_color += vec4(lights[int(positions[i].z)].color , 1);
        }
    }
    gl_FragColor = final_color;
}