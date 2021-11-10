#version 330 compatibility

// Constants
const float PI = 3.1415926535897932384626433832795;
const float PI_2 = 1.57079632679489661923;
const float PI_4 = 0.785398163397448309616;

// Uniform inputs
uniform float uVertTime;     // where in vertex animation cycle [0,1)
uniform float uColorPercent; // what percentage of rainbow to use
uniform float uNumChecks;    // how many checks in the checkerboard
uniform int uVertAnimSel; // which vert animation to use


// Outputs to fragment shader
out vec3 vN;		// normal vector
out vec3 vL;		// vector from point to light
out vec3 vE;		// vector from point to eye
out vec2 vST;       // texture coords
out vec3 vColor;

// Parameters
vec3 LightPosition = vec3(  0., 5., 5. );

// Shader function
void main() { 
    // Calculalate stuff for per-fragment lighting
	vec3 vert = gl_Vertex.xyz;
	vec4 ECposition = gl_ModelViewMatrix * vec4( vert, 1. );
	vN = normalize( gl_NormalMatrix * gl_Normal );	// normal vector
	vL = LightPosition - ECposition.xyz;		// vector from the point
							                    //  to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point
							                    //  to the eye position

    // Get TexCoords
    vST = gl_MultiTexCoord0.st;

    // Do some manipulation

    // Create spikes around the model at regular S,T intervals
    float r = 0.2;
    float st_ratio = 2.;
    float numSpikes = uNumChecks / 4;
    float sq_spike_rad = 1/(100*st_ratio*numSpikes);

    float target_s1 = floor(2*numSpikes*vST.s)/(2*numSpikes);
    float target_s2 = ceil(2*numSpikes*vST.s)/(2*numSpikes);
    float target_t1 = floor(numSpikes*vST.t)/(numSpikes);
    float target_t2 = ceil(numSpikes*vST.t)/(numSpikes);

    float ss1 = (vST.s-target_s1)*(vST.s-target_s1)*(st_ratio*st_ratio);
    float ss2 = (vST.s-target_s2)*(vST.s-target_s2)*(st_ratio*st_ratio);
    float tt1 = (vST.t-target_t1)*(vST.t-target_t1);
    float tt2 = (vST.t-target_t2)*(vST.t-target_t2);

    float distToSp1 = ss1 + tt1;
    float distToSp2 = ss2 + tt1;
    float distToSp3 = ss1 + tt2;
    float distToSp4 = ss2 + tt2;
    float distToOrigin = dot(vert,vert);

    float trig_cos_off = cos(2*PI*uVertTime+PI);
    float trig_sin_off = sin(2*PI*uVertTime);

    // Surface out animation
    if (uVertAnimSel == 0) {
        if (distToSp1 < sq_spike_rad)
            vert *= (distToOrigin)+((trig_cos_off+1)*r/2)*(1-distToSp1/sq_spike_rad); // animated from surface out
        if (distToSp2 < sq_spike_rad)
            vert *= (distToOrigin)+((trig_cos_off+1)*r/2)*(1-distToSp2/sq_spike_rad); // animated from surface out
        if (distToSp3 < sq_spike_rad)
            vert *= (distToOrigin)+((trig_cos_off+1)*r/2)*(1-distToSp3/sq_spike_rad); // animated from surface out
        if (distToSp4 < sq_spike_rad)
            vert *= (distToOrigin)+((trig_cos_off+1)*r/2)*(1-distToSp4/sq_spike_rad); // animated from surface out
    } else if (uVertAnimSel == 1) {
        if (distToSp1 < sq_spike_rad)
            vert *= (distToOrigin)+(trig_sin_off*r)*(1-distToSp1/sq_spike_rad); // animated in and out of surface
        if (distToSp2 < sq_spike_rad)
            vert *= (distToOrigin)+(trig_sin_off*r)*(1-distToSp2/sq_spike_rad); // animated in and out of surface
        if (distToSp3 < sq_spike_rad)
            vert *= (distToOrigin)+(trig_sin_off*r)*(1-distToSp3/sq_spike_rad); // animated in and out of surface
        if (distToSp4 < sq_spike_rad)
            vert *= (distToOrigin)+(trig_sin_off*r)*(1-distToSp4/sq_spike_rad); // animated in and out of surface
    }



        // vert.x = sqrt(sq_dist_from_origin)+(r)*(1-distToSp1/sq_spike_rad); // static spikes from surface
        // vert.x = sqrt(sq_dist_from_origin)+((trig_cos_off+1)*r/2)*(1-distToSp1/sq_spike_rad); // animated from surface out
        // vert.x = sqrt(sq_dist_from_origin)+(trig_sin_off*r)*(1-distToSp1/sq_spike_rad); // animated in and out of surface out
        // vert *= sqrt(sq_dist_from_origin)+(r)*(1-distToSp1/sq_spike_rad); // static spikes from surface
        // vert *= sqrt(sq_dist_from_origin)+((trig_cos_off+1)*r/2)*(1-distToSp1/sq_spike_rad); // animated from surface out
        // vert *= sqrt(sq_dist_from_origin)+(trig_sin_off*r)*(1-distToSp1/sq_spike_rad); // animated in and out of surface out




    // Output manipulated position
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );

    // Output vColor as this position
    vColor = uColorPercent*vert;
}