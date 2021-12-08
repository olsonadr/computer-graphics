// GLSL version
#version 420 compatibility

// uniforms
uniform mat4 matrixModelView;
uniform mat4 matrixNormal;
uniform mat4 matrixModelViewProjection;
uniform sampler3D map0;                // texture map #1
uniform bool useNoise;                 // whether to offset using noise for this
uniform float oceanThresh;             // threshold at which to make it the ocean
// uniform sampler2D map0;             // texture map #1

// vertex attribs (input)
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;

// varyings (output)
varying vec3 esPos, esVertex, esNormal;
varying vec2 texCoord0;
varying vec3 untransformedPos;
varying float esNoise;

varying  vec3  vN;		// normal vector
varying  vec3  vL;		// vector from point to light
varying  vec3  vE;		// vector from point to eye
uniform vec4 lightPosition;

void main()
{
    // Handle noise passed in 
    float noise;
    if (useNoise) {
        // vec3 npos = vertexPosition; // vPos = [-1, 1] => [0, 1]
        // vec3 npos = abs(vertexPosition); // vPos = [-1, 1] => [0, 1]
        vec3 npos = (vertexPosition+1)/2; // vPos = [-1, 1] => [0, 1]
        // vec3 npos = (vertexPosition+1); // [0, 1] => [1, 2]
        // vec3 npos = (vertexPosition+1)/2;
        // vec2 noise_coord = vec2(normalize(vertexPosition.x+vertexPosition.z),
        //                         normalize(vertexPosition.y-vertexPosition.z));
        // vec2 noise_coord = vec2(npos.x, npos.y);
        // vec2 noise_coord = vec2(npos.x+npos.z, npos.y-npos.z);
        // float noise = 1.0 + 0.5*texture3D(map0, vertexPosition).r;
        // float noise = 1.0 + texture3D(map0, vertexPosition).r;

        // noise = texture3D(map0, npos).r; // noise = [0, 1] => [1, 2]
        noise = 1.0 + texture3D(map0, npos).r; // noise = [0, 1] => [1, 2]
        // float noise = 1.0 + texture3D(map0, npos).r; // noise = [-1, 1] => [0, 2]


        // float noise = 1.0 + 0.5*texture2D(map0, vertexTexCoord).r;

        // float noise = texture2D(map0, noise_coord).r;
        // float noise = 1.0 + texture2D(map0, noise_coord).r;  // this for 2D
        // float noise = 1.0 + 0.5*texture2D(map0, noise_coord).r;

        // float noise = 1; // this for no noise
    } else {
        noise = 1;
    }

    esPos = vertexPosition;
    esNoise = noise;

    esVertex = (gl_ModelViewMatrix * vec4(noise*vertexPosition, 1.0)).xyz;
    // esVertex = (gl_ModelViewMatrix * vec4(vertexPosition, 1.0)).xyz;
    esNormal = gl_NormalMatrix * vertexNormal;
    texCoord0 = vertexTexCoord;
    // vec3 verte = vertexPosition; verte.z = dot(vec4(1), texture2D(map0, vertexTexCoord));
    // vec3 verte = vertexPosition; verte.z = texture2D(map0, vertexTexCoord).r;
    // gl_Position = gl_ModelViewProjectionMatrix * vec4(verte, 1.0);

    gl_Position = gl_ModelViewProjectionMatrix * vec4(noise*vertexPosition, 1.0);
    // gl_Position = gl_ModelViewProjectionMatrix * vec4(vertexPosition, 1.0);


    // // Original
    // esVertex = vec3(matrixModelView * vec4(vertexPosition, 1.0));
    // esNormal = vec3(matrixNormal * vec4(vertexNormal, 1.0));
    // texCoord0 = vertexTexCoord;
    // gl_Position = matrixModelViewProjection * vec4(vertexPosition, 1.0);

    // // Using gl_ standards
    // esVertex = (gl_ModelViewMatrix * vec4(vertexPosition, 1.0)).xyz;
    // esNormal = gl_NormalMatrix * vertexNormal;
    // texCoord0 = vertexTexCoord;
    // gl_Position = gl_ModelViewProjectionMatrix * vec4(vertexPosition, 1.0);


    // "New" method lighting vars    
    vec3 vert = vertexPosition.xyz;
    vec4 ECposition = gl_ModelViewMatrix * vec4( vert, 1. );
	vN = normalize( gl_NormalMatrix * vertexNormal );	// normal vector
	// vL = (gl_ModelViewMatrix * lightPosition).xyz - ECposition.xyz;	// vector from the point
	vL = (lightPosition).xyz - ECposition.xyz;	// vector from the point
							                                        //  to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point
							                    //  to the eye position 
	// gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );    
}




// // GLSL version
// #version 110
// // uniforms
// uniform mat4 matrixModelView;
// uniform mat4 matrixNormal;
// uniform mat4 matrixModelViewProjection;
// // vertex attribs (input)
// attribute vec3 vertexPosition;
// attribute vec3 vertexNormal;
// attribute vec2 vertexTexCoord;
// // varyings (output)
// varying vec3 esPos, esVertex, esNormal;
// varying vec2 texCoord0;
// void main()
// {
//     esVertex = vec3(matrixModelView * vec4(vertexPosition, 1.0));
//     esNormal = vec3(matrixNormal * vec4(vertexNormal, 1.0));
//     esPos = vertexPosition;
//     texCoord0 = vertexTexCoord;
//     gl_Position = matrixModelViewProjection * vec4(vertexPosition, 1.0);
// }