#version 430 core

in vec3 Normal;
in float angle;

layout( location = 0 ) out vec4 FragColor;

void main()
{
        if( dot(Normal, vec3(0.0f, 0.0f, 1.0f)) > angle)
            FragColor = vec4( 0.5f, 0.5f, 0.0f, 1.0);
        else
            discard;
}

