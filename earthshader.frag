#version 430 core

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
  vec3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

layout( location = 0 ) out vec4 FragColor;


vec3 phongModel( vec3 norm, vec3 diffR  ) {
    vec3 r = reflect( -LightDir, norm );
    vec3 ambient = Light.Intensity * Material.Ka;
    float sDotN = max( dot(LightDir, norm), 0.0 );
    vec3 diffuse = Light.Intensity * diffR * sDotN;

    vec3 spec = vec3(0.0);

    if( sDotN > 0.0 )
    {
        vec4 texColor = texture( TexEarth, TexCoord );
        if( texColor.a == 1.0)
            spec = vec3(0.0);
        else
            spec = Light.Intensity * Material.Ks *
                   pow( max( dot(r,ViewDir), 0.0 ), Material.Shininess );
    }
    return ambient + diffuse + spec;
}

//void main()
//{
//    vec3 ambAndDiff, spec;

//    vec4 normal = 2.0 * texture( NormalMapTex, TexCoord ) - 1.0;
//    vec4 texColor = texture( TexEarth, TexCoord );

//    phongModel(normal.xyz, texColor.rgb, ambAndDiff, spec);
//    if(texColor.a == 1.0)
//        FragColor = (vec4( ambAndDiff, 1.0) * texColor);
//    else  // alpha value over water = 254
//        FragColor = (vec4( ambAndDiff, 1.0) * texColor) + vec4(spec, 1.0);



//}

void main() {
    // Lookup the normal from the normal map
    vec4 normal = 2.0 * texture( NormalMapTex, TexCoord ) - 1.0;

    vec4 texColor = texture( TexEarth, TexCoord );
    FragColor = vec4( phongModel(normal.xyz, texColor.rgb), 1.0 );
}
