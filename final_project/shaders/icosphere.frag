// GLSL version
#version 420 compatibility

// uniforms
uniform vec4 lightPosition;             // should be in the modelspace
uniform vec4 lightAmbient;              // light ambient color
uniform vec4 lightDiffuse;              // light diffuse color
uniform vec4 lightSpecular;             // light specular color
uniform vec4 materialAmbient1;           // material ambient color
uniform vec4 materialDiffuse1;           // material diffuse color
uniform vec4 materialSpecular1;          // material specular color
uniform float materialShininess1;        // material specular shininess
uniform vec4 materialAmbient2;           // material ambient color
uniform vec4 materialDiffuse2;           // material diffuse color
uniform vec4 materialSpecular2;          // material specular color
uniform float materialShininess2;        // material specular shininess
uniform float aveNoise;                 // the average value of noise
// uniform sampler3D map0;                 // texture map #1
uniform sampler2D map0;                 // texture map #1

uniform mat4 matrixModelView;
uniform mat4 matrixModelViewProjection;

// varyings
varying vec3 esVertex, esNormal;
varying vec2 texCoord0;
varying vec3 untransformedPos;

varying vec3 vN;     // normal vector
varying vec3 vL;     // vector from point to light
varying vec3 vE;     // vector from point to eye

void main()
{
    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    float materialShininess;
    float sqdist = dot(untransformedPos,untransformedPos);
    // float sqdist = dot(esVertex,esVertex);
    if (sqdist > 1+aveNoise) {
        materialAmbient = materialAmbient1;
        materialDiffuse = materialDiffuse1;
        materialSpecular = materialSpecular1;
        materialShininess = materialShininess1;
    } else {
        materialAmbient = materialAmbient2;
        materialDiffuse = materialDiffuse2;
        materialSpecular = materialSpecular2;
        materialShininess = materialShininess2;
    }

    bool new_method = false;
    if (new_method) {
        vec3 Normal = normalize(vN);
        vec3 Light = normalize(vL);
        vec3 Eye = normalize(vE);

        vec3 ambient = lightAmbient.xyz * materialAmbient.xyz;

        float d = max( dot(Normal,Light), 0. ); // only do diffuse if the light can see the point
        vec3 diffuse = d * lightDiffuse.xyz * materialDiffuse.xyz;

        // only do specular if the light can see the point
        float s = 0.;
        if( dot(Normal,Light) > 0. ) {
            vec3 ref = normalize(  reflect( -Light, Normal )  );
            s = pow( max( dot(Eye,ref),0. ), materialShininess );
        }
        vec3 specular = s * lightSpecular.xyz * materialSpecular.xyz;
        gl_FragColor = vec4( ambient + diffuse + specular,  1. );
    }
    else {
        // vec3 normal = normalize(vN);
        vec3 normal = normalize(esNormal);
        vec3 light;

        if (lightPosition.w == 0.0) {
            light = normalize((matrixModelView*lightPosition).xyz);
        } else {
            light = normalize((matrixModelView*lightPosition).xyz - esVertex);
        }
        // if (lightPosition.w == 0.0) {
        //     light = normalize(lightPosition.xyz);
        // } else {
        //     light = normalize(lightPosition.xyz - esVertex);
        // }

        vec3 view = normalize(-esVertex);
        vec3 halfv = normalize(light + view);

        vec3 color = lightAmbient.rgb * materialAmbient.rgb;        // begin with ambient
        float dotNL = max(dot(normal, light), 0.0);
        color += lightDiffuse.rgb * materialDiffuse.rgb * dotNL;    // add diffuse
        color *= texture2D(map0, texCoord0).rgb;                    // modulate texture map
        // color *= texture3D(map0, normalize(vec3(texCoord0, 0.5))).rgb;                    // modulate texture map
        // color *= texture3D(map0, esVertex).rgb;                    // modulate texture map
        // color *= texture3D(map0, untransformedPos).rgb;                    // modulate texture map
        float dotNH = max(dot(normal, halfv), 0.0);
        // color += pow(dotNH, materialShininess) * lightSpecular.rgb * materialSpecular.rgb; // add specular
        // color += 0.1*(pow(dotNH, materialShininess) * lightSpecular.rgb * materialSpecular.rgb); // add specular
        color += 0.1*(pow(dotNH, materialShininess) * lightSpecular.rgb * materialSpecular.rgb); // add specular

        // set frag color
        gl_FragColor = vec4(color, materialDiffuse.a);

        // cool camera normals coloring
        // gl_FragColor = vec4(normal, materialDiffuse.a);
    }   
}





// // GLSL version
// #version 110
// // uniforms
// uniform vec4 lightPosition;             // should be in the eye space
// uniform vec4 lightAmbient;              // light ambient color
// uniform vec4 lightDiffuse;              // light diffuse color
// uniform vec4 lightSpecular;             // light specular color
// uniform vec4 materialAmbient;           // material ambient color
// uniform vec4 materialDiffuse;           // material diffuse color
// uniform vec4 materialSpecular;          // material specular color
// uniform float materialShininess;        // material specular shininess
// uniform mat4 matrixModelView;
// uniform sampler3D map0;                 // texture map #1
// // varyings
// varying vec3 esPos, esVertex, esNormal;
// varying vec2 texCoord0;
// void main()
// {
//     vec3 normal = normalize(esNormal);
//     vec3 light;
//     // if (lightPosition.w == 0.0) {
//     //     light = normalize((matrixModelView*lightPosition).xyz);
//     // } else {
//     //     light = normalize((matrixModelView*lightPosition).xyz - esVertex);
//     // }
//     if(lightPosition.w == 0.0) {
//         light = normalize(lightPosition.xyz);
//     } else {
//         light = normalize(lightPosition.xyz - esVertex);
//     }
//     vec3 view = normalize(-esVertex);
//     vec3 halfv = normalize(light + view);

//     vec3 color = lightAmbient.rgb * materialAmbient.rgb;        // begin with ambient
//     float dotNL = max(dot(normal, light), 0.0);
//     color += lightDiffuse.rgb * materialDiffuse.rgb * dotNL;    // add diffuse
//     color *= texture3D(map0, esPos).rgb;                    // modulate texture map
//     // color *= texture3D(map0, esVertex).rgb;                    // modulate texture map
//     float dotNH = max(dot(normal, halfv), 0.0);
//     color += 0.1*pow(dotNH, materialShininess) * lightSpecular.rgb * materialSpecular.rgb; // add specular

//     // set frag color
//     gl_FragColor = vec4(color, materialDiffuse.a);
// }