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

   /*getOutTexCoord("texCoord",
      "float2",
      true,
      false,
      meta,
      componentList);*/

   addWorldSpaceNormal(meta);
   addBlendWeights(componentList, meta);
   addUvs(componentList, meta);
}

void TriplanarFeatureHLSL::processPix(Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd)
{
   Var *inBlendWeights = getInTexCoord("blendWeights", "float3", true, componentList);
   Var *inTri_u = getInTexCoord("tri_u", "float3", true, componentList);
   Var *inTri_v = getInTexCoord("tri_v", "float3", true, componentList);

   output = NULL;
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

      if (worldInvTpose)
      {
         Var *wsNormal = new Var;
         wsNormal->setType("float3");
         wsNormal->setName("wsNormal");
         LangElement *wsNormalDecl = new DecOp(wsNormal);

         // Transform the normal to world space.
         //float3 wsNormal = normalize(mul(worldInvTpose, float4(In.normal.xyz, 0)).xyz);
         meta->addStatement(new GenOp("   @ = normalize(mul( @, float4( @.xyz, 0.0 ) ).xyz);\r\n", wsNormalDecl, worldInvTpose, inNormal));
      }
   }
}

void TriplanarFeatureHLSL::addBlendWeights(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *wsNormal = (Var*)LangElement::find("wsNormal");
   if (!wsNormal)
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
   meta->addStatement(new GenOp("   @ = triBlendWeights(@, 3);\r\n", outBlendWeights, wsNormal));
}

void TriplanarFeatureHLSL::addUvs(Vector<ShaderComponent*> &componentList, MultiLine *meta)
{
   Var *wsNormal = (Var*)LangElement::find("wsNormal");
   if (!wsNormal)
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
      outTri_u->mapsToSampler = false;
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
      outTri_v->mapsToSampler = false;
   }

   //worldSpaceTriplanarUVs(wsPosition, wsNormal, Out.tri_u, Out.tri_v);
   meta->addStatement(new GenOp("   worldSpaceTriplanarUVs(@, @, @, @);\r\n", wsPosition, wsNormal, outTri_u, outTri_v));
}