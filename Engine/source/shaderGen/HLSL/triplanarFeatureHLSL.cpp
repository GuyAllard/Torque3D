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

   addWorldSpaceNormal(meta);
   addBlendWeights(componentList, meta);
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

void TriplanarFeatureHLSL::addWorldSpaceNormal(MultiLine *meta)
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

      Var *worldNormal = new Var;
      worldNormal->setType("float3");
      worldNormal->setName("worldNormal");
      LangElement *worldNormalDecl = new DecOp(worldNormal);

      // Transform the normal to world space.
      //float3 wsNormal = normalize(mul(worldInvTpose, float4(In.normal.xyz, 0)).xyz);
      meta->addStatement(new GenOp("   @ = normalize(mul( @, float4( @.xyz, 0.0 ) ).xyz);\r\n", worldNormalDecl, worldInvTpose, inNormal));
   }
}

void TriplanarFeatureHLSL::addBlendWeights(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *worldNormal = (Var*)LangElement::find("worldNormal");
   if (!worldNormal)
      return;

   Var *outBlendWeights = (Var*)LangElement::find("blendWeights");
   if (!outBlendWeights)
   {
      // Setup the connector.
      ShaderConnector *connectComp = dynamic_cast<ShaderConnector *>(componentList[C_CONNECTOR]);
      outBlendWeights = connectComp->getElement(RT_TEXCOORD);
      outBlendWeights->setName("blendWeights");
      outBlendWeights->setStructName("OUT");
      outBlendWeights->setType("float3");
      outBlendWeights->mapsToSampler = false;
   }
   //Out.blendWeights = triBlendWeights(wsNormal, 3);
   meta->addStatement(new GenOp("   @ = triBlendWeights(@, 3);\r\n", outBlendWeights, worldNormal));
}

void TriplanarFeatureHLSL::addUvs(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *worldNormal = (Var*)LangElement::find("worldNormal");
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

   //worldSpaceTriplanarUVs(wsPosition, wsNormal, Out.tri_u, Out.tri_v);
   meta->addStatement(new GenOp("   worldSpaceTriplanarUVs(@, @, @, @);\r\n", wsPosition, worldNormal, outTri_u, outTri_v));
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

Var* TriplanarFeatureHLSL::get_blendWeights(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *inBlendWeights = getInTexCoord("blendWeights", "float3", true, componentList);
   
   return inBlendWeights;
}

LangElement* TriplanarFeatureHLSL::getBumpOp(Vector<ShaderComponent*> &componentList, MultiLine *meta, Var *bumpMap)
{
   Var *blendWeights = get_blendWeights(componentList, meta);
   Var *uvX = get_uvX(componentList, meta);
   Var *uvY = get_uvY(componentList, meta);
   Var *uvZ = get_uvZ(componentList, meta);

   return new GenOp("tex2D(@, @) * @.x + tex2D(@, @) * @.y + tex2D(@, @) * @.z",
                        bumpMap, uvX, blendWeights,
                        bumpMap, uvY, blendWeights,
                        bumpMap, uvZ, blendWeights);
}