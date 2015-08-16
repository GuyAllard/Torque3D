#ifndef _TRIPLANAR_FEATURE_HLSL_H_
#define _TRIPLANAR_FEATURE_HLSL_H_

#ifndef _SHADERGEN_HLSL_SHADERFEATUREHLSL_H_
#include "shaderGen/HLSL/shaderFeatureHLSL.h"
#endif
#ifndef _LANG_ELEMENT_H_
#include "shaderGen/langElement.h"
#endif
#ifndef _GFXDEVICE_H_
#include "gfx/gfxDevice.h"
#endif
#ifndef _FEATUREMGR_H_
#include "shaderGen/featureMgr.h"
#endif
#ifndef _MATERIALFEATURETYPES_H_
#include "materials/materialFeatureTypes.h"
#endif
#ifndef _MATERIALFEATUREDATA_H_
#include "materials/materialFeatureData.h"
#endif
#ifndef _PROCESSEDMATERIAL_H_
#include "materials/processedMaterial.h"
#endif

class TriplanarFeatureHLSL : public ShaderFeatureHLSL
{
protected:
   ShaderIncludeDependency mDep;

public:
   TriplanarFeatureHLSL();

   virtual void processVert(Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd);

   virtual void processPix(Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd);
   
   void addWorldSpaceNormal(MultiLine *meta);
   void addBlendWeights(Vector<ShaderComponent*> &componentList, MultiLine *meta);
   void addUvs(Vector<ShaderComponent*> &componentList, MultiLine *meta);

   // tri planar uvs
   static Var* get_uvX(Vector<ShaderComponent*> &componentList, MultiLine *meta);
   static Var* get_uvY(Vector<ShaderComponent*> &componentList, MultiLine *meta);
   static Var* get_uvZ(Vector<ShaderComponent*> &componentList, MultiLine *meta);
   static Var* get_blendWeights(Vector<ShaderComponent*> &componentList, MultiLine *meta);
   
   // sampler lookup and blend
   static LangElement* getDiffuseOp(Vector<ShaderComponent*> &componentList, MultiLine *meta, const MaterialFeatureData &fd);
   static LangElement* getBumpOp(Vector<ShaderComponent*> &componentList, MultiLine *meta, const MaterialFeatureData &fd);

   virtual String getName()
   {
      return "Triplanar Mapping";
   }

   virtual Resources getResources(const MaterialFeatureData &fd)
   {
      Resources res;

      res.numTex = 0;    // no textures
      res.numTexReg = 3; // 3 registers: blendWeights, tri_u, tri_v

      return res;
   }
};

class TriplanarDiffuseMapZFeatureHLSL : public ShaderFeatureHLSL
{
public:
   virtual String getName() { return "Triplanar Diffuse Z"; }
};

class TriplanarBumpMapZFeatureHLSL : public ShaderFeatureHLSL
{
public:
   virtual String getName() { return "Triplanar Bump Z"; }
};
#endif