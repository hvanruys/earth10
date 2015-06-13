#version 430 core

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout(location=2) in vec2 VertexTexCoord;
layout(location=3) in vec4 VertexTangent;


struct LightInfo {
  vec4 Position;  // Light position in eye coords.
  vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out vec3 LightDir;
out vec3 ViewDir;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    // Transform normal and tangent to eye space
    Normal = normalize( NormalMatrix * VertexNormal );
    vec3 tang = normalize( NormalMatrix * vec3(VertexTangent) );
    // Compute the binormal
    vec3 binormal = normalize( cross( Normal, tang ) ) * VertexTangent.w;

    // Matrix for transformation to tangent space
    mat3 toObjectLocal = mat3(
        tang.x, binormal.x, Normal.x,
        tang.y, binormal.y, Normal.y,
        tang.z, binormal.z, Normal.z ) ;

    // Transform light direction and view direction to tangent space
    vec3 pos = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
    LightDir = normalize( toObjectLocal * (Light.Position.xyz - pos));

    ViewDir = toObjectLocal * normalize(-pos);

    TexCoord = VertexTexCoord;
    Position = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );


    gl_Position = MVP * vec4(VertexPosition,1.0);
}

