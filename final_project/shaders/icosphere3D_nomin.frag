// GLSL version
#version 420 compatibility

// uniforms
uniform vec4 lightPosition;             // should be in the modelspace
uniform vec4 lightAmbient;              // light ambient color
uniform vec4 lightDiffuse;              // light diffuse color
uniform vec4 lightSpecular;             // light specular color

uniform vec4 materialAmbientLand1;      // land1 (hills) material ambient color
uniform vec4 materialDiffuseLand1;      // land1 (hills) material diffuse color
uniform vec4 materialSpecularLand1;     // land1 (hills) material specular color
uniform float materialShininessLand1;   // land1 (hills) material specular shininess

uniform vec4 materialAmbientLand2;      // land2 (mountain top) material ambient color
uniform vec4 materialDiffuseLand2;      // land2 (mountain top) material diffuse color
uniform vec4 materialSpecularLand2;     // land2 (mountain top) material specular color
uniform float materialShininessLand2;   // land2 (mountain top) material specular shininess

uniform vec4 materialAmbientOcean;      // ocean material ambient color
uniform vec4 materialDiffuseOcean;      // ocean material diffuse color
uniform vec4 materialSpecularOcean;     // ocean material specular color
uniform float materialShininessOcean;   // ocean material specular shininess
uniform float aveNoise;                 // the average value of noise
uniform float minNoise;                 // the min value of noise
uniform float maxNoise;                 // the max value of noise
uniform bool fixedLighting;             // whether to fix the lighting
uniform float oceanThresh;              // threshold at which to make it the ocean
uniform sampler3D map0;                 // texture map #1

uniform mat4 matrixModelView;
uniform mat4 matrixWorldView;
uniform mat4 matrixModelViewProjection;

// varyings
in vec3 esPos, esVertex, esNormal;
in vec2 texCoord0;
in float esNoise;

in vec3 vN;     // normal vector
in vec3 vL;     // vector from point to light
in vec3 vE;     // vector from point to eye

void main()
{
    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    float materialShininess;

    float sandThresh = 0.02;
    float mountThresh = 0.06;
    float mountTopThreshRat = 0.4;
    // float mountThresh = 0.05;
    // float mountTopThreshRat = 0.5;
    float interpThresh = 0.02;
    bool interp = false;

    if (esNoise > oceanThresh+aveNoise) {
        if (interp) {
            float pct = clamp((esNoise - (oceanThresh+mountThresh+aveNoise)) / interpThresh, 0, 1);
            materialAmbient = mix(materialAmbientLand1, materialAmbientLand2, pct);
            materialDiffuse = mix(materialDiffuseLand1, materialDiffuseLand2, pct);
            materialSpecular = mix(materialSpecularLand1, materialSpecularLand2, pct);
            materialShininess = mix(materialShininessLand1, materialShininessLand2, pct);
        } else {
            // if (esNoise > mountTopThreshRat*(maxNoise-minNoise)+minNoise+1 ) {
            // if (esNoise > mountTopThreshRat*((1+maxNoise)-(oceanThresh+aveNoise+mountThresh)) ) {
            // if (esNoise > oceanThresh+aveNoise+mountTopThreshRat*((1+maxNoise)-(oceanThresh+aveNoise)) ) {
            if (esNoise > oceanThresh+aveNoise+mountThresh+mountTopThreshRat*((1+maxNoise)-(oceanThresh+aveNoise+mountThresh)) ) {
                vec4 white = vec4(1);
                materialAmbient = 0.6*white;
                materialDiffuse = 0.8*white;
                materialSpecular = 0.8*white;
                materialShininess = 0.4;
            } else if (esNoise > oceanThresh+aveNoise+mountThresh) {
                materialAmbient = materialAmbientLand2;
                materialDiffuse = materialDiffuseLand2;
                materialSpecular = materialSpecularLand2;
                materialShininess = materialShininessLand2;
            } else if (esNoise > oceanThresh+aveNoise+sandThresh) {
                materialAmbient = materialAmbientLand1;
                materialDiffuse = materialDiffuseLand1;
                materialSpecular = materialSpecularLand1;
                materialShininess = materialShininessLand1;
            } else {
                vec3 sand = vec3(153, 138, 84)/255;
                materialAmbient = vec4(sand, materialAmbientLand1.w);
                materialDiffuse = vec4(sand, materialDiffuseLand1.w);
                materialSpecular = vec4(sand, materialSpecularLand1.w);
                materialShininess = materialShininessLand1/2;
            }
        }
        
    } else {
        materialAmbient = materialAmbientOcean;
        materialDiffuse = materialDiffuseOcean;
        materialSpecular = materialSpecularOcean;
        materialShininess = materialShininessOcean;
    }

    // Fixed lighting source (attached to eye)
    if (fixedLighting) {
        vec3 Normal = normalize(vN);
        vec3 Light = normalize(vL);
        vec3 Eye = normalize(vE);

        vec3 ambient = lightAmbient.w * lightAmbient.xyz * materialAmbient.xyz;

        float d = max( dot(Normal,Light), 0. ); // only do diffuse if the light can see the point
        vec3 diffuse = d * lightDiffuse.w * lightDiffuse.xyz * materialDiffuse.xyz;

        // only do specular if the light can see the point
        float s = 0.;
        if( dot(Normal,Light) > 0. ) {
            vec3 ref = normalize(  reflect( -Light, Normal )  );
            s = pow( max( dot(Eye,ref),0. ), materialShininess );
        }
        vec3 specular = s * lightSpecular.w * lightSpecular.xyz * materialSpecular.xyz;
        gl_FragColor = vec4( ambient + diffuse + specular,  1. );
    }
    
    // In-scene lighting source
    else {
        vec3 normal = normalize(esNormal);
        vec3 light;

        if (lightPosition.w == 0.0) {
            light = normalize((matrixWorldView*lightPosition).xyz);
        } else {
            light = normalize((matrixWorldView*lightPosition).xyz - esVertex);
        }
        vec3 Eye = normalize(vE);

        vec3 color = lightAmbient.rgb * materialAmbient.rgb;        // begin with ambient
        float dotNL = max(dot(normal, light), 0.0);
        color += lightDiffuse.rgb * materialDiffuse.rgb * dotNL;    // add diffuse
        float s = 0.; // only do specular if the light can see the point
        if( dot(normal,light) > 0. ) {
            vec3 ref = normalize(  reflect( -light, normal )  );
            s = pow( max( dot(Eye,ref),0. ), materialShininess );
        }
        color += s * lightSpecular.rgb * materialSpecular.rgb; // add specular

        // set frag color
        gl_FragColor = vec4(color, materialDiffuse.a);

        // // cool camera normals coloring
        // gl_FragColor = vec4(normal, materialDiffuse.a);
    }   
}