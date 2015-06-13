#version 430 core

in vec3 Position;
in vec3 Normal;
in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

layout(binding=0) uniform sampler2D TexEarth;
layout(binding=1) uniform sampler2D NormalMapTex;

struct LightInfo {
  vec4 Position;  // Light position in eye coords.
  vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;

struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

layout( location = 0 ) out vec4 FragColor;


vec3 phongModel( vec3 norm, vec3 diffR )
{
    //vec3 r = reflect( -LightDir, norm );
//    vec3 ambient = Light.Intensity * Material.Ka;
    vec3 ambient = Light.Intensity * diffR * Material.Ka;
    float sDotN = max( dot(LightDir, norm), 0.0 );
    vec3 diffuse = Light.Intensity * diffR * sDotN;

    return ambient + diffuse;
}

void phongModelNoBump( vec3 pos, vec3 norm, out vec3 ambAndDiff, out vec3 spec )
{
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(-pos.xyz);
    vec3 r = reflect( -s, norm );
    vec3 ambient = Light.Intensity * Material.Ka;
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
    spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
               pow( max( dot(r,v), 0.0 ), Material.Shininess );
    ambAndDiff = ambient + diffuse;
}

void main() {
    // Lookup the normal from the normal map
    vec3 ambAndDiff, spec;

    vec4 texColor = texture( TexEarth, TexCoord );

    if(gl_FrontFacing)
    {
        if( texColor.a == 1.0) // water nobump
        {
            phongModelNoBump( Position, Normal, ambAndDiff, spec );
            FragColor = (vec4( ambAndDiff, 1.0 ) * texColor) + vec4(spec,1.0);

         }
        else  // land bumpmap
        {
            vec4 normal = 2.0 * texture( NormalMapTex, TexCoord ) - 1.0;
            FragColor = vec4( phongModel(normal.xyz, texColor.rgb), 1.0 );
        }
    }
    else
        discard;
}
