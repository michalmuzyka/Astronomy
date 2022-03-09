#version 410 core
in vec3 vertex_color;   
out vec4 FragColor;

void main (void)  
{    
    FragColor = vec4(vertex_color, 1.0);   
}