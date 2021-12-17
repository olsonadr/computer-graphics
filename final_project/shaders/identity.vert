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
out vec3 esPos, esVertex, esNoisyVertex, esNormal;
out vec2 texCoord0;
out vec3 untransformedPos;
out float esNoise;

out vec3  vN;		// normal vector
out vec3  vL;		// vector from point to light
out vec3  vE;		// vector from point to eye

void main()
{
    vec3 npos = (vertexPosition+1)/2; // vPos = [-1, 1] => [0, 1]
    esNoise = 1.0 + texture3D(map0, npos).r; // noise = [0, 1] => [1, 2]

    esPos = vertexPosition;
    esVertex = (matrixModelView * vec4(vertexPosition, 1)).xyz;
    esNoisyVertex = (matrixModelView * vec4(esNoise*vertexPosition, 1)).xyz;
    esNormal = (matrixNormal * vec4(vertexNormal, 1)).xyz;
    texCoord0 = vertexTexCoord;

    gl_Position = matrixModelViewProjection * vec4(vertexPosition, 1);

    // Fixedlighting vars    
    vec3 vert = vertexPosition.xyz;
    vec4 ECposition = matrixModelView * vec4( vert, 1. );
	vN = normalize( mat3(matrixNormal) * vertexNormal );	// normal vector
	vL = (lightPosition).xyz - ECposition.xyz;	// vector from the point
							                    //  to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point
							                    //  to the eye position 
}