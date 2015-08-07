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
    
    float3 wsNormal = mul( objTrans, float4( normalize(In.normal.xyz), 0) ).xyz;
    
    Out.blendWeights = triBlendWeights(wsNormal, 3);
    
    TriplanarUVs uvs = worldSpaceTriplanarUVs(Out.wsPosition, wsNormal);
    Out.tri_u = float3(uvs.uvX.x, uvs.uvY.x, uvs.uvZ.x);
    Out.tri_v = float3(uvs.uvX.y, uvs.uvY.y, uvs.uvZ.y);
    
    
    
    // LIGHTING
    float3x3 objToTangentSpace;
    objToTangentSpace[0] = In.T;
    objToTangentSpace[1] = cross(In.T, normalize(In.normal)) * In.tangentW;
    objToTangentSpace[2] = normalize(In.normal);
    Out.worldToTangent = mul(objToTangentSpace, (float3x3)worldToObj);
    
    return Out;
}