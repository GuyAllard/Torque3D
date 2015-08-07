#ifndef _TRIPLANAR_HLSL_
#define _TRIPLANAR_HLSL_

struct AppData
{
    float3 position   : POSITION;
    float3 normal     : NORMAL;
    float tangentW    : TEXCOORD3;
    float3 T          : TANGENT;
};

struct ConnectData
{
    float4 HPOS             : POSITION;
    float3 wsPosition       : TEXCOORD0;
    float3x3 worldToTangent : TEXCOORD2;
    float3 tri_u            : TEXCOORD5;
    float3 tri_v            : TEXCOORD6;
    float3 blendWeights     : TEXCOORD7;
    
};

struct FragData
{
    float4 col : COLOR0;
};

struct TriplanarUVs
{
    float2 uvX;
    float2 uvY;
    float2 uvZ;
};

// compute per-face normal for faceted rendering
float3 fragFaceNormal(float3 fragPosition)
{
    return normalize(cross(ddy(fragPosition), ddx(fragPosition)));
}

// triplanar UV cordinates from world space position and world space normal
TriplanarUVs worldSpaceTriplanarUVs(float3 wsPosition, float3 wsNormal)
{
    TriplanarUVs uvs;
    uvs.uvX = wsPosition.yz;
    uvs.uvY = wsPosition.xz;
    uvs.uvZ = wsPosition.xy;
    
    // rotate uv coords around +z axis
    uvs.uvX.x *= sign(wsNormal.x);
    uvs.uvY.x *= -sign(wsNormal.y);
    
    return uvs;
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