#include "triplanar.hlsl"

ConnectData main(AppData In, 
                 uniform float4x4 modelview, 
                 uniform float4x4 objTrans,
                 uniform float4x4 worldToObj,
                 uniform float4x4 worldInvTpose)
{
    ConnectData Out;
    Out.HPOS = mul(modelview, float4(In.position.xyz, 1));
    
    // world space position and normal for lighting
    Out.wsPosition = mul( objTrans, float4( In.position.xyz, 1 ) ).xyz;
    
    // transform the normals to world space, correct for non-uniform scale
    float3 wsNormal = normalize(mul(worldInvTpose, float4(In.normal.xyz, 0) ).xyz);
    
    // determine tri-planar blend weights from world space normal
    Out.blendWeights = triBlendWeights(wsNormal, 3);

    // generate tri-planar uv coordinates
    worldSpaceTriplanarUVs(Out.wsPosition, wsNormal, Out.tri_u, Out.tri_v);
    
    
    // LIGHTING
    float3x3 objToTangentSpace;
    objToTangentSpace[0] = In.T;
    objToTangentSpace[1] = cross(In.T, normalize(In.normal)) * In.tangentW;
    objToTangentSpace[2] = normalize(In.normal);
    Out.worldToTangent = mul(objToTangentSpace, (float3x3)worldToObj);
    
    return Out;
}