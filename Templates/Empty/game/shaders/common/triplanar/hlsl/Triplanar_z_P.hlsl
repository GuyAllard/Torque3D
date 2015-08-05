#include "triplanar.hlsl"
#include "../../lighting.hlsl"
#include "../../torque.hlsl"

FragData main(ConnectData In,
              uniform sampler2D diffuseMap,
              uniform sampler2D diffuseMapZ,
              uniform sampler2D bumpMap,
              uniform sampler2D bumpMapZ,
              uniform float3 eyePosWorld)
{
    FragData Out;

    // calculate normal of the fragment
    float3 norm = normalize(In.wsNormalScaled);
    // get blend weights for this fragment
    float3 blendWeights = fragBlendWeights(norm, 3);
    
    // world space UVs
    float2 uvX = In.wsPosition.yz;
    float2 uvY = In.wsPosition.xz;
    float2 uvZ = In.wsPosition.xy;

    // get color for each plane
    float4 colx = tex2D(diffuseMap, uvX);
    float4 coly = tex2D(diffuseMap, uvY);
    float4 colz = tex2D(diffuseMapZ, uvZ);

    // final color is the sum of the weighted color for each plane
    float4 diffuseCol = colx.xyzw * blendWeights.xxxx +
                        coly.xyzw * blendWeights.yyyy +
                        colz.xyzw * blendWeights.zzzz;
    
    // Bumpmap
    float4 bumpx = tex2D(bumpMap, uvX);
    float4 bumpy = tex2D(bumpMap, uvY);
    float4 bumpz = tex2D(bumpMapZ, uvZ);
    float4 bumpNormal = bumpx * blendWeights.xxxx +
                        bumpy * blendWeights.yyyy +
                        bumpz * blendWeights.zzzz;
                        
    bumpNormal.xyz = bumpNormal.xyz * 2.0 - 1.0;
    bumpNormal.xyz *= bumpNormal.a;
    float3 wsNormal = normalize( mul( bumpNormal.xyz, In.worldToTangent ) );
    
    // LIGHTING
    float3 wsView = normalize( eyePosWorld - In.wsPosition );
    float4 diffuse; 
    float4 specular;
    compute4Lights( wsView, In.wsPosition, wsNormal, float4( 1, 1, 1, 1 ), diffuse, specular);
    float4 lightCol = float4( diffuse.rgb + ambient.rgb, 1 );
    
    // Pixel Specular
    float4 specCol = lightCol * diffuseCol.a * specular;
    
    // Final color
    Out.col = diffuseCol * lightCol + specCol;
    hdrEncode( Out.col );
    return Out;
}