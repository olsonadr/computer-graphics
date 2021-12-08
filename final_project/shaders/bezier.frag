#version 330 compatibility

// Constants
const float PI = 3.1415926535897932384626433832795;
const float PI_2 = 1.57079632679489661923;
const float PI_4 = 0.785398163397448309616;

// Uniform inputs
uniform float uKa, uKd, uKs;    // coefficients of each type of lighting
uniform float uShininess;		// specular exponent
uniform vec3 uSpecularColor;	// light color
uniform float uFragTime;        // where in fragment animation cycle [0,1)
uniform vec3 uColor;			// object color
uniform float uColorPercent;    // percentage of color
// uniform float uNumChecks;       // how many checks in the checkerboard
uniform int uFragColorSel;      // which frac color method to use

// Fragment-specific inputs to shader (from vert, rasterizer, etc)
in vec2 vST;       // texture coords
in vec3 vN;        // normal vector
in vec3 vL;        // vector from point to light
in vec3 vE;        // vector from point to eye
in vec3 vColor;


// Fragment Shader Function
void main() {
    // Output fragment color
    switch(uFragColorSel) {
    case 0:
        gl_FragColor = uColorPercent*vec4(vColor, 1); // vColor
        break;
    case 1:
        gl_FragColor = uColorPercent*vec4(uColor, 1); // uColor
        break;
    case 2:
        gl_FragColor = uColorPercent*vec4(1,1,1,1); // white
        break;
    case 3:
        gl_FragColor = uColorPercent*vec4(0.6,0.6,0.6,1); // gray
        break;
    }


    // // Do some manipulation!

    // // Black and white spiral
    // myColor = int((vST.x+vST.y)*uNumChecks)%2 *vec3(1.);
    // // Black and white stripes
    // myColor = int(vST.x*uNumChecks)%2 *vec3(1.);
    // // Checkerboard pattern
    // myColor = ( int(vST.s*uNumChecks)+int(vST.t*uNumChecks/2) )%2 * vec3(1.);


    // // Wavy checkerboard
    // float s_offset_phase = 8*uNumChecks;
    // float s_offset_offset = 2*PI * uFragTime;
    // float s_offset_amp = 0.01;
    // float s_offset = s_offset_amp*( sin(s_offset_phase*PI*vST.t+s_offset_offset)+1 );
    // // float s_offset = s_offset_amp*( sin(s_offset_phase*PI*vST.t)+1 );
    // float t_offset_phase = 12*uNumChecks;
    // float t_offset_offset = 2*PI * uFragTime;
    // float t_offset_amp = 0.02;
    // float t_offset = t_offset_amp*( cos(t_offset_phase*PI*vST.s+t_offset_offset)+1 );
    // // float t_offset = t_offset_amp*( sin(t_offset_phase*PI*vST.s)+1 );
    // myColor = ( int(vST.s*uNumChecks+s_offset)+int(vST.t*uNumChecks/2+t_offset) )%2 * vec3(1.);



    // if (uColorPercent > 0) {
    //     if (uFragColorSel == 0) {
    //         myColor = myColor*(vColor+0.5);
    //     } else if (uFragColorSel == 1) {
    //         // Establish oscillation phase
    //         // float phase = vST.t * 16*PI;
    //         // float phase = 6*PI * (cos(2*PI*uFragTime)/2+1); // ease into higher phase then away
    //         float phase = 6*PI;

    //         // Establish oscillation shift
    //         // float shift = 0;
    //         float shift = uFragTime/phase*2*PI; // spin once every two cylces

    //         // Make red channel sin oscillate by s
    //         myColor.r *= uColorPercent * ( sin(phase*(vST.s+shift)+PI_2) + 1 ) / 2;
    //         // Make green channel cos oscillate by s
    //         myColor.g *= uColorPercent * ( cos(phase*(vST.s+shift)+PI_2) + 1 ) / 2;
    //         // Make blue channel cos(+PI_2) oscillate by s
    //         myColor.b *= uColorPercent * ( cos(phase*(vST.s+shift)+PI_2+PI_2) + 1 ) / 2;
    //     }
    // }



    // // Normalize/receive inputs
	// vec3 Normal = normalize(vN);
	// vec3 Light = normalize(vL);
	// vec3 Eye = normalize(vE);
	// vec3 myColor = vColor;

    // // Lighting calculations
	// vec3 ambient = uKa * myColor;
	// float d = max( dot(Normal,Light), 0. ); // only do diffuse if the light can see the point
	// vec3 diffuse = uKd * d * myColor;
	// float s = 0.;
	// if( dot(Normal,Light) > 0. ) { // only do specular if the light can see the point
	// 	vec3 ref = normalize(  reflect( -Light, Normal )  );
	// 	s = pow( max( dot(Eye,ref),0. ), uShininess );
	// }
	// vec3 specular = uKs * s * uSpecularColor;
	// gl_FragColor = vec4( ambient + diffuse + specular,  1. ); // lighting

}
