// Inspired by: https://www.youtube.com/watch?v=DxfEbulyFcY

// GLSL version
#version 420 compatibility
 
// function prototypes
vec2 raySphere(vec3 center, float rad, vec3 ray0, vec3 rayDir);
vec2 raySphereClip(vec3 center, float rad, vec3 ray0, vec3 rayDir, vec3 cCenter, float cRad);
float linearize_depth(float d,float zNear,float zFar);
vec4 calcLight(vec3 ray0, vec3 rayDir, float len, vec3 planetCenter, float planetRad, float atmosphereRad, vec3 dirToSun);
vec4 calcLightBaked(vec3 ray0, vec3 rayDir, float len, vec3 planetCenter, float planetRad, float atmosphereRad, vec3 dirToSun);
float opticalDepth(vec3 rayOrigin, vec3 rayDir, float len, vec3 planetCenter, float planetRad, float atmosphereRad);
float opticalDepthBaked(vec3 ray0, vec3 rayDir, float len, vec3 planetCenter, float planetRad, float atmosphereRad);
float densityAtPoint(vec3 samplePoint, vec3 planetCenter, float planetRad, float atmosphereRad);
bool pointInSphere(vec3 samplePoint, vec3 sphereCenter, float sphereRad);
float sqDstToSphere(vec3 samplePoint, vec3 sphereCenter, float sphereRad);

// globals
#define FLT_MAX 3.402823466e+38
float depthMin = 0.1;
float depthMax = 100;
int numScatterPoints = 10;
int numSamplePoints = 10;
float densityFalloff = 1.5; // [0, 13]
float inSphereEps = 0.4;
bool distantSun = true; // whether to treat sun as infinitely far away or light in scene


// uniforms
uniform vec4 lightPosition;             // should be in the modelspace
uniform vec4 lightAmbient;              // light ambient color
uniform vec4 lightDiffuse;              // light diffuse color
uniform vec4 lightSpecular;             // light specular color
uniform vec4 lightCol;                  // general direct light color (color of sun)

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
uniform bool useNoise;                  // whether to use the noise for land
uniform float oceanThresh;              // threshold at which to make it the ocean
uniform sampler3D map0;                 // noise texture map
uniform sampler2D depthTex;             // screen depth texture map (from first pass)
uniform sampler2D opticalDepthTex;      // pre-comped optical depth around a planet
uniform vec4 eyePos;
uniform float atmRad;
uniform float oceanRad;

uniform mat4 matrixModelView;
uniform mat4 matrixWorldView;
uniform mat4 matrixModelViewProjection;
uniform mat4 matrixModelViewInv;

uniform float screenWidth;
uniform float screenHeight;

uniform float scale;
uniform vec3 camPos;
uniform vec4 scatterCoef;
uniform bool useBakedOpticalDepth;

// varyings
in vec3 esPos, esVertex, esNoisyVertex, esNormal;
in vec2 texCoord0;
in float esNoise;
in flat int inAtm;

in vec3 vN;     // normal vector
in vec3 vL;     // vector from point to light
in vec3 vE;     // vector from point to eye

void main()
{
    // get params in world space instead of eye space
    vec3 viewRayOrigin = eyePos.xyz;
    vec3 viewRay = esPos - viewRayOrigin;
    vec3 viewRayDir = normalize(viewRay);
    vec3 center = vec3(0,0,0);
    float rad = length(esPos-center);

    // Manual face culling (if camera inside atm, cull front face, else cull back face)
    float pointVisibility = 1;
    if (inAtm==1 && gl_FrontFacing) pointVisibility = 0;
    else if (inAtm==0 && !gl_FrontFacing) pointVisibility = 0;

    // Get distance to atmosphere
    vec2 dists = raySphere(center, rad, viewRayOrigin, viewRayDir);
    vec2 distsClip = raySphereClip(center, rad, viewRayOrigin, viewRayDir, center, (oceanThresh + aveNoise));
    float dstTo = dists.x;

    // Get and un-linearize depth
    vec2 sceneDepthCoord = vec2(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight);
    float sceneDepth = texture(depthTex, sceneDepthCoord).r;
    float linSceneDepth = linearize_depth(sceneDepth, depthMin, depthMax);

    // Get distance through atmosphere excluding space through planet
    float dstThru = min(dists.y, linSceneDepth-dstTo);
    float dstThruClip = distsClip.y;


    if (dstThru > 0) {
        vec3 pointInAtm = viewRayOrigin + viewRayDir * dstTo;
        float planetRad = (oceanThresh + aveNoise);

        vec3 dirToSun;
        if (distantSun) dirToSun = normalize(lightPosition.xyz - center);     // all relative to planet center
        else            dirToSun = normalize(lightPosition.xyz - pointInAtm); // all relative to point in space

        vec4 light;
        if (useBakedOpticalDepth)
            light = calcLightBaked(pointInAtm, viewRayDir, dstThru, center, planetRad, rad, dirToSun);
        else
            light = calcLight(pointInAtm, viewRayDir, dstThru, center, planetRad, rad, dirToSun);

        // blended by alpha channel
        gl_FragColor = vec4(1.5*light.xyz, 1.5*light.w*pointVisibility);
    }
}



// Returns a vec2 (distTo, distThru). If ray0 in sphere, dstTo=0. If misses, rayTo=MAX, dstThru=0.
// assumes rayDir is normalized
vec2 raySphere(vec3 center, float rad, vec3 ray0, vec3 rayDir) {
    vec3 off = ray0 - center;
    // float a = 1;
    float a = dot(rayDir, rayDir);
    float b = 2*dot(rayDir, off);
    float c = dot(off, off) - dot(rad, rad);
    float d = b * b - 4 * a * c;

    // Number of intersections: d<0 => 0; d=0 => 1; d>0 => 2
    if (d > 0) {
        float s = sqrt(d);
        // float dstToNear = (-b-s) / (2*a);
        float dstToNear = max(0, (-b-s) / (2*a));
        float dstToFar = (-b+s) / (2*a);

        // Ignore intersections behind ray
        if (dstToFar >= 0) {
            return vec2(dstToNear, dstToFar-dstToNear);
        }
    }

    // Otherwise ray did not intersect or intersection behind ray
    return vec2(FLT_MAX, 0);
}


// Returns a vec2 (distTo, distThru). If ray0 in sphere, dstTo=0. If misses, rayTo=MAX, dstThru=0.
// assumes rayDir is normalized, clips distThru based on the sphere centered at cCenter w/ rad cRad
// assumes clipSphere is inside of raySphere
vec2 raySphereClip(vec3 center, float rad, vec3 ray0, vec3 rayDir, vec3 cCenter, float cRad) {
    // Get rudimentary val
    vec2 res = raySphere(center, rad, ray0, rayDir);
    
    // Account for planet clipping
    vec2 cDists = raySphere(cCenter, cRad, ray0, rayDir);
    res.y = min(res.y, cDists.x-res.x);

    // Return res
    return res;
}


float linearize_depth(float d,float zNear,float zFar) {
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}


vec4 calcLightBaked(vec3 ray0, vec3 rayDir, float len, vec3 planetCenter, float planetRad, float atmosphereRad, vec3 dirToSun) {
    vec3 scatterPoint = ray0;
    float stepSize = len / (numScatterPoints - 1);
    vec4 scatteredLight = vec4(0);

    // dir to sun is lightPosition.xyz - ray0, so curr would be (dirToSun+ray0)-scatterPoint
    for (int i = 0; i < numScatterPoints; i++) {
        vec3 currDirToSun;
        if (distantSun) currDirToSun = dirToSun; // same for all points
        else            currDirToSun = (dirToSun+ray0)-scatterPoint; // from scatterPoint to sun
        
        float sunRayLen = raySphere(planetCenter, atmosphereRad, scatterPoint, currDirToSun).y;
        float viewRayOpticalDepth = opticalDepthBaked(scatterPoint, -rayDir, stepSize*i, planetCenter, planetRad, atmosphereRad);
        // float sunRayOpticalDepth = opticalDepth(scatterPoint, currDirToSun, sunRayLen, planetCenter, planetRad, atmosphereRad);
        float sunRayOpticalDepth = opticalDepthBaked(scatterPoint, currDirToSun, sunRayLen, planetCenter, planetRad, atmosphereRad);

        // vec4 transmittance = exp(-(viewRayOpticalDepth)) * vec4(1);
        // vec4 transmittance = exp(-(sunRayOpticalDepth)) * vec4(1);
        vec4 transmittance = exp(-(sunRayOpticalDepth+viewRayOpticalDepth) * scatterCoef) * lightCol;
        // vec4 transmittance = exp(-(sunRayOpticalDepth+viewRayOpticalDepth) * scatterCoef);
        float localDensity = densityAtPoint(scatterPoint, planetCenter, planetRad, atmosphereRad);

        scatteredLight += localDensity * transmittance * scatterCoef * lightCol * stepSize;
        // scatteredLight += localDensity * transmittance * scatterCoef * stepSize;
        scatterPoint += rayDir * stepSize;
    }

    return scatteredLight;
}


vec4 calcLight(vec3 ray0, vec3 rayDir, float len, vec3 planetCenter, float planetRad, float atmosphereRad, vec3 dirToSun) {
    vec3 scatterPoint = ray0;
    float stepSize = len / (numScatterPoints - 1);
    vec4 scatteredLight = vec4(0);

    // dir to sun is lightPosition.xyz - ray0, so curr would be (dirToSun+ray0)-scatterPoint
    for (int i = 0; i < numScatterPoints; i++) {
        vec3 currDirToSun;
        if (distantSun) currDirToSun = dirToSun; // same for all points
        else            currDirToSun = (dirToSun+ray0)-scatterPoint; // from scatterPoint to sun
          
        float sunRayLen = raySphere(planetCenter, atmosphereRad, scatterPoint, currDirToSun).y; // original
        float sunRayOpticalDepth = opticalDepth(scatterPoint, currDirToSun, sunRayLen, planetCenter, planetRad, atmosphereRad);
        float viewRayOpticalDepth = opticalDepth(scatterPoint, -rayDir, stepSize*i, planetCenter, planetRad, atmosphereRad);
        // vec4 transmittance = exp(-(viewRayOpticalDepth)) * vec4(1);
        // vec4 transmittance = exp(-(sunRayOpticalDepth)) * vec4(1);
        vec4 transmittance = exp(-(sunRayOpticalDepth+viewRayOpticalDepth) * scatterCoef) * lightCol;
        // vec4 transmittance = exp(-(sunRayOpticalDepth+viewRayOpticalDepth) * scatterCoef);
        float localDensity = densityAtPoint(scatterPoint, planetCenter, planetRad, atmosphereRad);

        scatteredLight += localDensity * transmittance * scatterCoef * lightCol * stepSize;
        // scatteredLight += localDensity * transmittance * scatterCoef * stepSize;
        scatterPoint += rayDir * stepSize;
    }

    return scatteredLight;
}


float opticalDepth(vec3 ray0, vec3 rayDir, float len, vec3 planetCenter, float planetRad, float atmosphereRad) {
    vec3 samplePoint = ray0;
    float stepSize = len / (numSamplePoints - 1);
    float opticalDepth = 0;

    for (int i = 0; i < numSamplePoints; i++) {
        float localDensity = densityAtPoint(samplePoint, planetCenter, planetRad, atmosphereRad);
        opticalDepth += localDensity * stepSize;
        samplePoint += rayDir * stepSize;
    }

    return opticalDepth;
}

// use as is for rays starting in atm and going all the way through
// for rays terminating early, get a = od(ray0 to atm), b = od(rayend to atm),
// and use a-b (optical depth from ray0 to rayend);
float opticalDepthBaked(vec3 ray0, vec3 rayDir, float len, vec3 planetCenter, float planetRad, float atmosphereRad) {
    float height = length(planetCenter - ray0) - planetRad;
    // height 0 at surface, 1 at atmosphereRad above surface
    float height01 = height / (atmosphereRad - planetRad);
    // angle 0 looking up, 1 looking down toward surface
    float angle01 = (dot(normalize(planetCenter - ray0), rayDir) * 0.5 + 0.5);
    // lookup optical depth
    return texture(opticalDepthTex, vec2(angle01, height01)).r;
}


float densityAtPoint(vec3 samplePoint, vec3 planetCenter, float planetRad, float atmosphereRad) {
    // float lAtmRad = 1.2 * atmosphereRad; // slightly larger
    // float lAtmRad = 0.95 * atmosphereRad; // slightly smaller
    float lAtmRad = atmosphereRad; // original
    float heightAboveSurface = length(samplePoint - planetCenter) - planetRad;
    float height01 = heightAboveSurface / (lAtmRad - planetRad);
    float localDensity = exp(-height01 * densityFalloff) * (1-height01);
    return localDensity;
}


bool pointInSphere(vec3 samplePoint, vec3 sphereCenter, float sphereRad) {
    vec3 pointDiff = samplePoint - sphereCenter;
    float sqPointDist = dot(pointDiff, pointDiff);
    return sqPointDist - sphereRad*sphereRad < inSphereEps;
}


float sqDstToSphere(vec3 samplePoint, vec3 sphereCenter, float sphereRad) {
    vec3 pointDiff = samplePoint - sphereCenter;
    float sqPointDist = dot(pointDiff, pointDiff);
    return sqPointDist - sphereRad*sphereRad;
}