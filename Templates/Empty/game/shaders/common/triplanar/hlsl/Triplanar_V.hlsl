#include "triplanar.hlsl"

ConnectData main(AppData In, 
                 uniform float4x4 modelview, 
                 uniform float4x4 objTrans,
                 uniform float4x4 worldToObj)
{
    ConnectData Out;
    Out.HPOS = mul(modelview, float4(In.position.xyz, 1));
    
    // world space position and normal for lighting
    Out.wsPosition = mul( objTrans, float4( In.position.xyz, 1 ) ).xyz;
    Out.wsNormal = mul( objTrans, float4( normalize(In.normal.xyz), 0) ).xyz;
    
    // LIGHTING
    float3x3 objToTangentSpace;
    objToTangentSpace[0] = In.T;
    objToTangentSpace[1] = cross(In.T, normalize(In.normal)) * In.tangentW;
    objToTangentSpace[2] = normalize(In.normal);
    Out.worldToTangent = mul(objToTangentSpace, (float3x3)worldToObj);
    
    return Out;
}