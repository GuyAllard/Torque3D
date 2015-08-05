#include "triplanar.hlsl"

FragData main(ConnectData In, uniform sampler2D diffuseMap)
{
    FragData Out;

    // calculate normal of the fragment
    float3 norm = In.wsNormalScaled;
    // get blend weights for this fragment
    float3 blendWeights = fragBlendWeights(norm, 3);

    // world space UVs
    float2 uvX = In.wsPosition.yz;
    float2 uvY = In.wsPosition.xz;
    float2 uvZ = In.wsPosition.xy;
    
    // get color for each plane
    float4 colx = tex2D(diffuseMap, uvX);
    float4 coly = tex2D(diffuseMap, uvY);
    float4 colz = tex2D(diffuseMap, uvZ);

    // final color is the sum of the weighted color for each plane
    Out.col = colx.xyzw * blendWeights.xxxx +
              coly.xyzw * blendWeights.yyyy +
              colz.xyzw * blendWeights.zzzz;
    return Out;
}