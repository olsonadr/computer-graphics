// GLSL version
#version 420 compatibility

// function prototypes
float inSphereEps = 0.5;
bool pointInSphere(vec3 samplePoint, vec3 sphereCenter, float sphereRad);

// uniforms
uniform mat4 matrixNormal;
uniform mat4 matrixModelView;
uniform mat4 matrixModelViewProjection;
uniform mat4 matrixWorldView;
uniform mat4 matrixWorldViewProjection;
uniform sampler3D map0;                // texture map #1

uniform bool useNoise;                 // whether to offset using noise for this
uniform float oceanThresh;             // threshold at which to make it the ocean

uniform vec4 lightPosition;

uniform vec4 eyePos;

uniform bool useModelOverWorld;

// vertex attribs (input)
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;

// varyings (output)
out vec3 esPos, esVertex, esNoisyVertex, esNormal;
out vec2 texCoord0;
out vec3 untransformedPos;
out float esNoise;
out flat int inAtm;

out vec3  vN;		// normal vector
out vec3  vL;		// vector from point to light
out vec3  vE;		// vector from point to eye

void main()
{

    // Noise
    vec3 npos = (vertexPosition+1)/2; // vPos = [-1, 1] => [0, 1]
    esNoise = 1.0 + texture3D(map0, npos).r; // noise = [0, 1] => [1, 2]

    // Model position
    esPos = vertexPosition;
    vec3 vert = vertexPosition;

    // Offset position by in or out of atmosphere
    vec3 viewRayOrigin = eyePos.xyz;
    vec3 center = vec3(0,0,0);
    float rad = length(esPos-center);
    inAtm = pointInSphere(viewRayOrigin, center, rad) ? 1 : 0;
    // if (inAtm==1) vert *= 1+(inSphereEps/rad);
    // if (inAtm==1) vert *= 1+(3*inSphereEps/rad);

    // Use and output position
    esVertex = (matrixModelView * vec4(vert, 1)).xyz;
    esNoisyVertex = (matrixModelView * vec4(esNoise*vert, 1)).xyz;
    esNormal = (matrixNormal * vec4(vertexNormal, 1)).xyz;
    texCoord0 = vertexTexCoord;
    gl_Position = matrixModelViewProjection * vec4(vert, 1);

    // Fixedlighting vars    
    // vec4 ECposition = matrixWorldView * vec4( vert, 1. );
    vec4 ECposition = matrixModelView * vec4( vert, 1. );
	vN = normalize( mat3(matrixNormal) * vertexNormal );	// normal vector
	vL = (lightPosition).xyz - ECposition.xyz;	// vector from the point
							                    //  to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point
							                    //  to the eye position 
}

bool pointInSphere(vec3 samplePoint, vec3 sphereCenter, float sphereRad) {
    vec3 pointDiff = samplePoint - sphereCenter;
    float sqPointDist = dot(pointDiff, pointDiff);
    return sqPointDist - sphereRad*sphereRad < inSphereEps;
}



    // // Use and output position
    // if (useModelOverWorld) {
    //     esVertex = (matrixModelView * vec4(vert, 1)).xyz;
    //     esNoisyVertex = (matrixModelView * vec4(esNoise*vert, 1)).xyz;
    //     esNormal = (matrixNormal * vec4(vertexNormal, 1)).xyz;
    //     texCoord0 = vertexTexCoord;
    //     gl_Position = matrixModelViewProjection * vec4(vert, 1);
    // } else {
    //     esVertex = (matrixWorldView * vec4(vert, 1)).xyz;
    //     esNoisyVertex = (matrixWorldView * vec4(esNoise*vert, 1)).xyz;
    //     esNormal = (matrixNormal * vec4(vertexNormal, 1)).xyz;
    //     texCoord0 = vertexTexCoord;
    //     gl_Position = matrixWorldViewProjection * vec4(vert, 1);
    // }
