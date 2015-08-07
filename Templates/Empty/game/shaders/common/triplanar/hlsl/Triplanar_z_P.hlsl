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

    // unpack uvs
    float2 uvX = float2(In.tri_u[0], In.tri_v[0]);
    float2 uvY = float2(In.tri_u[1], In.tri_v[1]);
    float2 uvZ = float2(In.tri_u[2], In.tri_v[2]);
    
    // diffuse color
    // get color for each plane
    float4 colx = tex2D(diffuseMap, uvX);
    float4 coly = tex2D(diffuseMap, uvY);
    float4 colz = tex2D(diffuseMapZ, uvZ);

    // final color is the sum of the weighted color for each plane
    float4 diffuseCol = colx.xyzw * In.blendWeights.xxxx +
                        coly.xyzw * In.blendWeights.yyyy +
                        colz.xyzw * In.blendWeights.zzzz;
    
    // vizualize the blend weights
    //diffuseCol *= float4(In.blendWeights, 1);
    
    // Bumpmap
    float4 bumpx = tex2D(bumpMap, uvX);
    float4 bumpy = tex2D(bumpMap, uvY);
    float4 bumpz = tex2D(bumpMapZ, uvZ);
    float4 bumpNormal = bumpx * In.blendWeights.xxxx +
                        bumpy * In.blendWeights.yyyy +
                        bumpz * In.blendWeights.zzzz;
                        
    bumpNormal.xyz = bumpNormal.xyz * 2.0 - 1.0;
    float3 wsNormal = normalize( mul( bumpNormal.xyz, In.worldToTangent ) );
    
    // LIGHTING
    float3 wsView = normalize( eyePosWorld - In.wsPosition );
    float4 diffuse; 
    float4 specular;
    compute4Lights( wsView, In.wsPosition, wsNormal, float4( 1, 1, 1, 1 ), diffuse, specular);
    float4 lightCol = float4( diffuse.rgb + ambient.rgb, 1 );
    
    // Pixel Specular
    float4 specCol = lightCol * diffuseCol.a * specular * 0.5;
    
    // Final color
    Out.col = diffuseCol * lightCol + specCol;
    hdrEncode( Out.col );
    return Out;
}