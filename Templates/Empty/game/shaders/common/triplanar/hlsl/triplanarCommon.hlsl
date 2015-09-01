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
float4 triBlendWeights(float3 normal, float tightness)
{
    float4 weights = pow(float4(abs(normal), 0.0f), tightness);
    weights = max(weights, 0.0f);
    weights /= (weights.x + weights.y + weights.z).xxxx;
    weights.w = clamp(sign(normal.z), 0.0f, 1.0f); // store up/down in the last position
    return weights;
}
#endif