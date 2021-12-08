#version 330 compatibility

// Constants
const float PI = 3.1415926535897932384626433832795;
const float PI_2 = 1.57079632679489661923;
const float PI_4 = 0.785398163397448309616;

// Uniform inputs
uniform float uVertTime;     // where in vertex animation cycle [0,1)
// uniform float uColorPercent; // what percentage of rainbow to use
// uniform float uNumChecks;    // how many checks in the checkerboard
// uniform int uVertAnimSel; // which vert animation to use

uniform float numPoints;

// Per-vertex inputs
// layout(location = 0) in float vT;
// layout(location = 1) in vec3 vP[4];
// layout(location = 1) in vec3 vP;

// in vec2 vT;
// // in float vT;
// in vec3 vP0;
// in vec3 vP1;
// in vec3 vP2;
// in vec3 vP3;
// // in vec3 vP4;

in vec4 vP0;
// in vec4 vP1;
// in vec4 vP2;
// in vec4 vP3;

in vec4 vP;



// Outputs to fragment shader
out vec3 vN;		// normal vector
out vec3 vL;		// vector from point to light
out vec3 vE;		// vector from point to eye
out vec2 vST;       // texture coords
out vec3 vColor;

// Parameters
vec3 LightPosition = vec3(  0., 5., 5. );

// Vertex Shader Function
void main() { 
    // Calculalate stuff for per-fragment lighting
	vec4 vert = vec4(gl_Vertex.xyz, 1);
	vec4 ECposition = gl_ModelViewMatrix * vert;
	vN = normalize( gl_NormalMatrix * gl_Normal );	// normal vector
	vL = LightPosition - ECposition.xyz;		// vector from the point
							                    //  to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point
							                    //  to the eye position

    // // Get TexCoords
    // vST = gl_MultiTexCoord0.st;

    // // Do some manipulation

    // // Create spikes around the model at regular S,T intervals
    // float r = 0.2;
    // float st_ratio = 2.;
    // float numSpikes = uNumChecks / 4;
    // float sq_spike_rad = 1/(100*st_ratio*numSpikes);

    // float target_s1 = floor(2*numSpikes*vST.s)/(2*numSpikes);
    // float target_s2 = ceil(2*numSpikes*vST.s)/(2*numSpikes);
    // float target_t1 = floor(numSpikes*vST.t)/(numSpikes);
    // float target_t2 = ceil(numSpikes*vST.t)/(numSpikes);

    // float ss1 = (vST.s-target_s1)*(vST.s-target_s1)*(st_ratio*st_ratio);
    // float ss2 = (vST.s-target_s2)*(vST.s-target_s2)*(st_ratio*st_ratio);
    // float tt1 = (vST.t-target_t1)*(vST.t-target_t1);
    // float tt2 = (vST.t-target_t2)*(vST.t-target_t2);

    // float distToSp1 = ss1 + tt1;
    // float distToSp2 = ss2 + tt1;
    // float distToSp3 = ss1 + tt2;
    // float distToSp4 = ss2 + tt2;
    // float distToOrigin = dot(vert,vert);

    // float cos_o = cos(2*PI*uVertTime+PI);
    // float sin_o = sin(2*PI*uVertTime);

    // // Surface out animation
    // if (uVertAnimSel == 0) {
    //     if (distToSp1 < sq_spike_rad)
    //         vert *= (distToOrigin)+((cos_o+1)*r/2)*(1-distToSp1/sq_spike_rad); // animated from surface out
    //     if (distToSp2 < sq_spike_rad)
    //         vert *= (distToOrigin)+((cos_o+1)*r/2)*(1-distToSp2/sq_spike_rad); // animated from surface out
    //     if (distToSp3 < sq_spike_rad)
    //         vert *= (distToOrigin)+((cos_o+1)*r/2)*(1-distToSp3/sq_spike_rad); // animated from surface out
    //     if (distToSp4 < sq_spike_rad)
    //         vert *= (distToOrigin)+((cos_o+1)*r/2)*(1-distToSp4/sq_spike_rad); // animated from surface out
    // } else if (uVertAnimSel == 1) {
    //     if (distToSp1 < sq_spike_rad)
    //         vert *= (distToOrigin)+(sin_o*r)*(1-distToSp1/sq_spike_rad); // animated in and out of surface
    //     if (distToSp2 < sq_spike_rad)
    //         vert *= (distToOrigin)+(sin_o*r)*(1-distToSp2/sq_spike_rad); // animated in and out of surface
    //     if (distToSp3 < sq_spike_rad)
    //         vert *= (distToOrigin)+(sin_o*r)*(1-distToSp3/sq_spike_rad); // animated in and out of surface
    //     if (distToSp4 < sq_spike_rad)
    //         vert *= (distToOrigin)+(sin_o*r)*(1-distToSp4/sq_spike_rad); // animated in and out of surface
    // }


    // Wow bezier stuff
    vert = vec4(0,0,0,0);

    // vert = vec4(vP.xyz, 1);
    // vert = vP0;
    // float t = vP0[3];
    vert = vP;
    float t = vP[3];


    // vert.y = vT;

    // vert.x = vP[0].x;
    // vert.y = vP[0].y;
    // vert.z = vP[0].z;

    // float t = vP0[3];
    // // float t = vT;
    // // float t = vT[0];
    // float it = 1.0 - t;
    // float it_2 = it*it;
    // float it_3 = it_2*it;
    // float t_2 = t*t;
    // float t_3 = t_2*t;

    // vert = it_3*vP0 + 3*t*it_2*vP1 + 3*t_2*it*vP2 + t_3*vP3;
    // vert = it_3*vP0.xyz + 3*t*it_2*vP1.xyz + 3*t_2*it*vP2.xyz + t_3*vP3.xyz;
    // vert = pow(1-t,3)*vP0 + 3*t*pow(1-t,2)*vP1 + 3*pow(t,2)*(1-t)*vP2 + pow(t,3)*vP3;



    // vert = vP0.xyz;
    // vert = vP1.xyz;
    // vert = vP0;
    // vert = vP1;

    // vert.x = vP0.x;
    // vert.y = vP0.y;
    // vert.z = vP0.z;

    // vert.x = vT[0];
    // vert.y = vT[1];
    // vert.y = vT;
    // vert.y = vT[0];
    // vert.y = vT[1];

    // vert.z = uVertTime;


    // Output manipulated position
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert.xyz, 1. );
	// gl_Position = vert;


    // Color to fragment shader

    // Output vColor as a rainbow from [0,1] t values
    // vColor = clamp(vec3(3*t-2, 3*t-1, 3*t), 0, 1); // t=[0,1] => r=clamp[-2,1], g=clamp[-1,2], b=clamp[0,3]
    // vColor = clamp(vec3(3*t, 3*t-1, 3*t-2), 0, 1); // t=[0,1] => r=clamp[0,3], g=clamp[-1,2], b=clamp[-2,1]   (r,g,b up to 1)
    // vColor = abs(2*(clamp(vec3(3*t, 3*t-1, 3*t-2), 0, 1))-1);
    // vColor = abs(1*(clamp(vec3(3*t, 3*t-1, 3*t-2), 0, 1))-0.5);




    // Establish oscillation phase and shift
    float phase = PI;
    float shift = t/phase*2*PI; // spin once every two cylces

    // Make r,g,b channels sin oscillate by t
    vColor.r = ( sin(phase*(t+shift)) + 1 ) / 2;
    vColor.g = ( sin(phase*(t+shift)+PI_2) + 1 ) / 2;
    vColor.b = ( sin(phase*(t+shift)+PI_2+PI_2) + 1 ) / 2;

    // Establish point size
    gl_PointSize = 3;



    // abs(2*t-1) // t[0,1] => [0,1,0]
    // vColor = vec3(1,1,1) * vT;
    // vColor = (1-t)*vec3(1,1,1);
    // vColor = (t)*vec3(1,1,1);
    // vColor = vec3(1,vP1.z,1);
    // vColor = vert;
    // vColor = uColorPercent*vert;
}