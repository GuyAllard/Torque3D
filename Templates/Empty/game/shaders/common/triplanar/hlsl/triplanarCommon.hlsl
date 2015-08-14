#ifndef _TRIPLANAR_COMMON_HLSL_
#define _TRIPLANAR_COMMON_HLSL_

// triplanar UV cordinates from world space position and world space normal
void worldSpaceTriplanarUVs(float3 position, float3 normal, float scale, out float3 u, out float3 v)
{
    float2 uvX;
    float2 uvY;
    float2 uvZ;
    
    uvX = position.yz / scale;
    uvY = position.xz / scale;
    uvZ = position.xy / scale;
    
    // rotate uv coords around +z axis
    uvX.x *= sign(normal.x);
    uvY.x *= -sign(normal.y);
    
    u = float3(uvX.x, uvY.x, uvZ.x);
    v = float3(uvX.y, uvY.y, uvZ.y);
 }

// triplanar blending weights from world space normal
float3 triBlendWeights(float3 normal, float tightness)
{
    float3 weights = pow(abs(normal), tightness);
    weights = max(weights, 0);
    weights /= (weights.x + weights.y + weights.z).xxx;
    return weights;
}
#endif