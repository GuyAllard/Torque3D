#ifndef _TRIPLANAR_COMMON_HLSL_
#define _TRIPLANAR_COMMON_HLSL_

// triplanar UV cordinates from world space position and world space normal
void worldSpaceTriplanarUVs(float3 wsPosition, float3 wsNormal, out float3 tri_u, out float3 tri_v)
{
    float2 uvX;
    float2 uvY;
    float2 uvZ;
    
    uvX = wsPosition.yz;
    uvY = wsPosition.xz;
    uvZ = wsPosition.xy;
    
    // rotate uv coords around +z axis
    uvX.x *= sign(wsNormal.x);
    uvY.x *= -sign(wsNormal.y);
    
    tri_u = float3(uvX.x, uvY.x, uvZ.x);
    tri_v = float3(uvX.y, uvY.y, uvZ.y);
 }

// triplanar blending weights from world space normal
float3 triBlendWeights(float3 wsNormal, float tightness)
{
    //float3 weights = wsNormal*wsNormal;
    float3 weights = pow(abs(wsNormal), tightness);
    weights = max(weights, 0);
    weights /= (weights.x + weights.y + weights.z).xxx;
    return weights;
}
#endif