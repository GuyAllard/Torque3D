#include "triplanarFeatureHLSL.h"
#include "shaderGen/shaderOp.h"

TriplanarFeatureHLSL::TriplanarFeatureHLSL()
   : mDep("shaders/common/triplanar/hlsl/triplanarCommon.hlsl")
{
   addDependency(&mDep);
}

void TriplanarFeatureHLSL::processVert(Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd)
{
   MultiLine *meta = new MultiLine;
   output = meta;

   addWorldSpaceNormal(componentList, meta);
   addUvs(componentList, meta);
}

void TriplanarFeatureHLSL::processPix(Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd)
{
   MultiLine *meta = new MultiLine;
   output = meta;

   get_blendWeights(componentList, meta);
   get_uvX(componentList, meta);
   get_uvY(componentList, meta);
   get_uvZ(componentList, meta);
}

void TriplanarFeatureHLSL::addWorldSpaceNormal(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   // search for vert normal
   Var *inNormal = (Var*)LangElement::find("normal");
   if (inNormal)
   {
      Var *worldInvTpose = (Var*)LangElement::find("worldInvTpose");

      if (!worldInvTpose)
      {     
         worldInvTpose = new Var;
         worldInvTpose->setType("float4x4");
         worldInvTpose->setName("worldInvTpose");
         worldInvTpose->uniform = true;
         worldInvTpose->constSortPos = cspPass;
      }

      Var *outNormal = (Var*)LangElement::find("tpNormal");
      if (!outNormal)
      {
         // Setup the connector.
         ShaderConnector *connectComp = dynamic_cast<ShaderConnector *>(componentList[C_CONNECTOR]);
         outNormal = connectComp->getElement(RT_TEXCOORD);
         outNormal->setName("tpNormal");
         outNormal->setStructName("OUT");
         outNormal->setType("float3");
         outNormal->mapsToSampler = true;
      }

      // Transform the normal to world space.
      //OUT.tpNormal = normalize(mul(worldInvTpose, float4(In.normal.xyz, 0)).xyz);
      meta->addStatement(new GenOp("   @ = normalize(mul( @, float4( @.xyz, 0.0 ) ).xyz);\r\n", outNormal, worldInvTpose, inNormal));
   }
}

Var* TriplanarFeatureHLSL::get_blendWeights(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *inNormal = getInTexCoord("tpNormal", "float3", true, componentList);
   if (!inNormal)
      return NULL;

   Var *triplanarTightness = (Var*)LangElement::find("triplanarTightness");
   if (!triplanarTightness)
   {
      triplanarTightness = new Var;
      triplanarTightness->setType("float");
      triplanarTightness->setName("triplanarTightness");
      triplanarTightness->uniform = true;
      triplanarTightness->constSortPos = cspPass;
   }

   Var *blendWeights = (Var*)LangElement::find("blendWeights");
   if (!blendWeights)
   {
      blendWeights = new Var;
      blendWeights->setType("float4");
      blendWeights->setName("blendWeights");
      LangElement *blendWeightsDecl = new DecOp(blendWeights);

      //float4 blendWeights = triBlendWeights(tpNormal, triplanarTightness);
      meta->addStatement(new GenOp("   @ = triBlendWeights(@, @);\r\n", blendWeightsDecl, inNormal, triplanarTightness));
   } 
   return blendWeights;
}

void TriplanarFeatureHLSL::addUvs(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *worldNormal = (Var*)LangElement::find("tpNormal");
   if (!worldNormal)
      return;

   // Grab the input position.
   Var *inPos = (Var*)LangElement::find("inPosition");
   if (!inPos)
      inPos = (Var*)LangElement::find("position");

   Var *wsPosition = (Var*)LangElement::find("wsPosition");
   if (!wsPosition)
   {
      wsPosition = new Var;
      wsPosition->setType("float3");
      wsPosition->setName("wsPosition");
   }
   LangElement *wsPositionDecl = new DecOp(wsPosition);

   // Get the transform to world space.
   Var *objTrans = (Var*)LangElement::find("objTrans");
   if (!objTrans)
   {
      objTrans = new Var;
      objTrans->setType("float4x4");
      objTrans->setName("objTrans");
      objTrans->uniform = true;
      objTrans->constSortPos = cspPrimitive;
   }
   // wsPosition = mul(objTrans, float4(In.position.xyz, 1)).xyz;
   meta->addStatement(new GenOp("   @ = mul(@, float4(@.xyz, 1)).xyz;\r\n", wsPositionDecl, objTrans, inPos));

   Var *outTri_u = (Var*)LangElement::find("tri_u");
   if (!outTri_u)
   {
      // Setup the connector.
      ShaderConnector *connectComp = dynamic_cast<ShaderConnector *>(componentList[C_CONNECTOR]);
      outTri_u = connectComp->getElement(RT_TEXCOORD);
      outTri_u->setName("tri_u");
      outTri_u->setStructName("OUT");
      outTri_u->setType("float3");
      outTri_u->mapsToSampler = true;
   }

   Var *outTri_v = (Var*)LangElement::find("tri_v");
   if (!outTri_v)
   {
      // Setup the connector.
      ShaderConnector *connectComp = dynamic_cast<ShaderConnector *>(componentList[C_CONNECTOR]);
      outTri_v = connectComp->getElement(RT_TEXCOORD);
      outTri_v->setName("tri_v");
      outTri_v->setStructName("OUT");
      outTri_v->setType("float3");
      outTri_v->mapsToSampler = true;
   }

   Var *triplanarScale = (Var*)LangElement::find("triplanarScale");
   if (!triplanarScale)
   {
      triplanarScale = new Var;
      triplanarScale->setType("float");
      triplanarScale->setName("triplanarScale");
      triplanarScale->uniform = true;
      triplanarScale->constSortPos = cspPass;
   }

   //worldSpaceTriplanarUVs(wsPosition, wsNormal, Out.tri_u, Out.tri_v);
   meta->addStatement(new GenOp("   worldSpaceTriplanarUVs(@, @, @, @, @);\r\n", 
                                    wsPosition, worldNormal, triplanarScale, outTri_u, outTri_v));
}

Var* TriplanarFeatureHLSL::get_uvX(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *inTri_u = getInTexCoord("tri_u", "float3", true, componentList);
   Var *inTri_v = getInTexCoord("tri_v", "float3", true, componentList);

   Var *uvX = (Var*)LangElement::find("uvX");
   if (!uvX)
   {
      uvX = new Var;
      uvX->setType("float2");
      uvX->setName("uvX");
      LangElement *uvXDecl = new DecOp(uvX);
      meta->addStatement(new GenOp("   @ = float2( @[0], @[0] );\r\n", uvXDecl, inTri_u, inTri_v));
   }

   return uvX;
}

Var* TriplanarFeatureHLSL::get_uvY(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *inTri_u = getInTexCoord("tri_u", "float3", true, componentList);
   Var *inTri_v = getInTexCoord("tri_v", "float3", true, componentList);

   Var *uvY = (Var*)LangElement::find("uvY");
   if (!uvY)
   {
      uvY = new Var;
      uvY->setType("float2");
      uvY->setName("uvY");
      LangElement *uvYDecl = new DecOp(uvY);
      meta->addStatement(new GenOp("   @ = float2( @[1], @[1] );\r\n", uvYDecl, inTri_u, inTri_v));
   }

   return uvY;
}

Var* TriplanarFeatureHLSL::get_uvZ(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *inTri_u = getInTexCoord("tri_u", "float3", true, componentList);
   Var *inTri_v = getInTexCoord("tri_v", "float3", true, componentList);

   Var *uvZ = (Var*)LangElement::find("uvZ");
   if (!uvZ)
   {
      uvZ = new Var;
      uvZ->setType("float2");
      uvZ->setName("uvZ");
      LangElement *uvZDecl = new DecOp(uvZ);
      meta->addStatement(new GenOp("   @ = float2( @[2], @[2] );\r\n", uvZDecl, inTri_u, inTri_v));
   }

   return uvZ;
}

LangElement* TriplanarFeatureHLSL::getDiffuseOp(Vector<ShaderComponent*> &componentList, MultiLine *meta, const MaterialFeatureData &fd)
{
   Var *blendWeights = get_blendWeights(componentList, meta);
   Var *uvX = get_uvX(componentList, meta);
   Var *uvY = get_uvY(componentList, meta);
   Var *uvZ = get_uvZ(componentList, meta);

   Var *diffuseMap = (Var*)LangElement::find("diffuseMap");
   if (!diffuseMap)
   {
      diffuseMap = new Var;
      diffuseMap->setType("sampler2D");
      diffuseMap->setName("diffuseMap");
      diffuseMap->uniform = true;
      diffuseMap->sampler = true;
      diffuseMap->constNum = Var::getTexUnitNum();
   }

   Var *diffuseMapZ;
   if (fd.features[MFT_TriplanarDiffuseMapZ])
   {
      diffuseMapZ = (Var*)LangElement::find("diffuseMapZ");
      if (!diffuseMapZ)
      {
         diffuseMapZ = new Var;
         diffuseMapZ->setType("sampler2D");
         diffuseMapZ->setName("diffuseMapZ");
         diffuseMapZ->uniform = true;
         diffuseMapZ->sampler = true;
         diffuseMapZ->constNum = Var::getTexUnitNum();
      }
   }
   else
   {
      diffuseMapZ = diffuseMap;
   }

   if (fd.features[MFT_TriplanarZUp])
   {
      return new GenOp("(tex2D(@, @) * @.x + tex2D(@, @) * @.y + tex2D(@, @) * @.z * @.w + tex2D(@, @) * @.z * (1 - @.w))",
                        diffuseMap, uvX, blendWeights,
                        diffuseMap, uvY, blendWeights,
                        diffuseMapZ, uvZ, blendWeights, blendWeights,
                        diffuseMap, uvZ, blendWeights, blendWeights);
   }
   else
   {
      return new GenOp("(tex2D(@, @) * @.x + tex2D(@, @) * @.y + tex2D(@, @) * @.z)",
                           diffuseMap, uvX, blendWeights,
                           diffuseMap, uvY, blendWeights,
                           diffuseMapZ, uvZ, blendWeights);
   }
}

LangElement* TriplanarFeatureHLSL::getBumpOp(Vector<ShaderComponent*> &componentList, MultiLine *meta, const MaterialFeatureData &fd)
{
   Var *blendWeights = get_blendWeights(componentList, meta);
   Var *uvX = get_uvX(componentList, meta);
   Var *uvY = get_uvY(componentList, meta);
   Var *uvZ = get_uvZ(componentList, meta);

   Var *bumpMap = (Var*)LangElement::find("bumpMap");
   if (!bumpMap)
   {
      bumpMap = new Var;
      bumpMap->setType("sampler2D");
      bumpMap->setName("bumpMap");
      bumpMap->uniform = true;
      bumpMap->sampler = true;
      bumpMap->constNum = Var::getTexUnitNum();     // used as texture unit num here
   }

   Var *bumpMapZ;
   if (fd.features[MFT_TriplanarBumpMapZ])
   {
      bumpMapZ = (Var*)LangElement::find("bumpMapZ");
      if (!bumpMapZ)
      {
         bumpMapZ = new Var;
         bumpMapZ->setType("sampler2D");
         bumpMapZ->setName("bumpMapZ");
         bumpMapZ->uniform = true;
         bumpMapZ->sampler = true;
         bumpMapZ->constNum = Var::getTexUnitNum();
      }
   }
   else
   {
      bumpMapZ = bumpMap;
   }

   if (fd.features[MFT_TriplanarZUp])
   {
      return new GenOp("(tex2D(@, @) * @.x + tex2D(@, @) * @.y + tex2D(@, @) * @.z * @.w + tex2D(@, @) * @.z * (1 - @.w))",
                        bumpMap, uvX, blendWeights,
                        bumpMap, uvY, blendWeights,
                        bumpMapZ, uvZ, blendWeights, blendWeights,
                        bumpMap, uvZ, blendWeights, blendWeights);
   }
   else
   {
      return new GenOp("(tex2D(@, @) * @.x + tex2D(@, @) * @.y + tex2D(@, @) * @.z)",
                        bumpMap, uvX, blendWeights,
                        bumpMap, uvY, blendWeights,
                        bumpMapZ, uvZ, blendWeights);
   }
}