#version 460 compatibility

layout (location = 0) out vec4 FragColor;

uniform int original;

void main()
{
    //Cor Final
    if(original == 1){
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }else{
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
} 