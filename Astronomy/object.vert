#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float fog_density;

out vec3 FragPos; 
out vec2 TexCoords;
out vec3 Normal;
out float visibility;

void main()
{
    TexCoords = aTexCoords;  
    Normal =  mat3(transpose(inverse(model))) * aNormal; 
    vec4 positionRelativeToCamera = view * model * vec4(aPos, 1.0);
    gl_Position = projection * positionRelativeToCamera;
    FragPos = vec3(model * vec4(aPos, 1.0));

    float distance = length(positionRelativeToCamera.xyz);
    visibility = exp(-pow(distance*fog_density,2));
    visibility = clamp(visibility, 0.0, 1.0);
}