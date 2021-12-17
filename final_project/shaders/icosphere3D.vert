// GLSL version
#version 420 compatibility

// uniforms
uniform mat4 matrixModelView;
uniform mat4 matrixNormal;
uniform mat4 matrixModelViewProjection;
uniform sampler3D map0;                // texture map #1

uniform bool useNoise;                 // whether to offset using noise for this
uniform float oceanThresh;             // threshold at which to make it the ocean

uniform vec4 lightPosition;

// vertex attribs (input)
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;

// varyings (output)
out vec3 esPos, esVertex, esNormal;
out vec2 texCoord0;
out vec3 untransformedPos;
out float esNoise;

out vec3  vN;		// normal vector
out vec3  vL;		// vector from point to light
out vec3  vE;		// vector from point to eye

void main()
{
    // Handle noise passed in 
    vec3 npos = (vertexPosition+1)/2; // vPos = [-1, 1] => [0, 1]
    float noise = 1.0 + texture3D(map0, npos).r; // noise = [0, 1] => [1, 2]

    esPos = vertexPosition;
    esNoise = noise;

    vec4 noisyVert;
    if (useNoise) {
        noisyVert = vec4(noise*vertexPosition, 1.0);
        // noisyVert = vec4(noise*vertexPosition, 1.0);
    } else {
        noisyVert = vec4(vertexPosition, 1.0);
    }

    // esVertex = (gl_ModelViewMatrix * noisyVert).xyz;
    // esNormal = gl_NormalMatrix * vertexNormal;
    esVertex = (matrixModelView * noisyVert).xyz;
    esNormal = mat3(matrixNormal) * vertexNormal;
    texCoord0 = vertexTexCoord;

    // gl_Position = gl_ModelViewProjectionMatrix * noisyVert;
    gl_Position = matrixModelViewProjection * noisyVert;


    // Fixedlighting vars    
    vec3 vert = vertexPosition.xyz;
    // vec4 ECposition = gl_ModelViewMatrix * vec4( vert, 1. );
    vec4 ECposition = matrixModelView * vec4( vert, 1. );
	// vN = normalize( gl_NormalMatrix * vertexNormal );	// normal vector
	vN = normalize( mat3(matrixNormal) * vertexNormal );	// normal vector
	vL = (vec4(-1.5, 1, -1, 1)).xyz - ECposition.xyz;	// vector from the point
	// vL = (lightPosition).xyz - ECposition.xyz;	// vector from the point
							                                        //  to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point
							                    //  to the eye position 
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