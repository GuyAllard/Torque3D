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
    float3 wsNormal         : TEXCOORD1;
    float3 wsNormalScaled   : TEXCOORD2;
    float3x3 worldToTangent : TEXCOORD3;
};

struct FragData
{
    float4 col : COLOR0;
};

// compute per-face normal for faceted rendering
float3 fragFaceNormal(float3 fragPosition)
{
    return normalize(cross(ddy(fragPosition), ddx(fragPosition)));
}

// compute triplanar blending weights from normal
float3 fragBlendWeights(float3 wsNormal, float tightness)
{
    //float3 weights = wsNormal*wsNormal;
    float3 weights = pow(abs(wsNormal), tightness);
    weights = max(weights, 0);
    weights /= (weights.x + weights.y + weights.z).xxx;
    return weights;
}
#endif