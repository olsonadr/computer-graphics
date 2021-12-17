#version 330

// uniform vec4 viewport;
// uniform float depthMin; // the min value used to linearize depth
// uniform float depthMax; // the max value used to linearize depth

layout (location = 0) out float frag_depth;
// // float depthMin = 0.1;
// // float depthMax = 1;
// float depthMin = 0.1;
// float depthMax = 1000;
float depthMin = 0.1;
float depthMax = 100;
// float depthMax = 10;
// float depthMin = gl_DepthRange.near;
// float depthMax = gl_DepthRange.far;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * depthMin * depthMax) / (depthMax + depthMin - z * (depthMax - depthMin));
}

void main() {
    float depth = gl_FragCoord.z;
    // float depth = gl_FragCoord.z;

    float linearDepth = (LinearizeDepth(depth) - depthMin) / (depthMax - depthMin);
    // float linearDepth = (gl_FragCoord.z);

    // frag_depth = linearDepth ;
    frag_depth = depth ;


    // frag_depth = 2.0 * depthMin * depthMax / (depthMax + depthMin - (2.0 * depth - 1.0) * (depthMax - depthMin));



    // vec4 viewport = vec4(0, 0, 1800, 1200);

    // vec4 ndcPos;
    // ndcPos.xy = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;
    // ndcPos.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) /
    //             (gl_DepthRange.far - gl_DepthRange.near);
    // ndcPos.w = 1.0;

    // vec4 clipPos = ndcPos / gl_FragCoord.w;
    // vec4 eyePos = invPersMatrix * clipPos;

    // float ndc_depth = clip_space_pos.z / clip_space_pos.w;
    // float depth = (((farZ-nearZ) * ndc_depth) + nearZ + farZ) / 2.0;

    
    // float n = depthMin, f = depthMax;
    // depth = gl_FragCoord.z;
    // float z_ndc = 2.0 * depth - 1.0;
    // float z_eye = 2.0 * n * f / (f + n - z_ndc * (f - n));

    // frag_depth = z_eye;

}