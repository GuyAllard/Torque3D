//******************************************************************************
// Triplanar mapping with a single texture
//****************************************************************************** 
new ShaderData( Triplanar_Simple )
{
   DXVertexShaderFile   = "shaders/common/triplanar/hlsl/Triplanar_V.hlsl";
   DXPixelShaderFile    = "shaders/common/triplanar/hlsl/Triplanar_Simple_P.hlsl";
 
   //OGLVertexShaderFile   = "shaders/common/ribbons/gl/basicRibbonShaderV.glsl";
   //OGLPixelShaderFile    = "shaders/common/ribbons/gl/basicRibbonShaderP.glsl";
 
   samplerNames[0] = "$diffuseMap";
 
   pixVersion = 2.1;
};

singleton CustomMaterial(triplanar1)
{
   shader = Triplanar_Simple;
   sampler["diffuseMap"] = "art/materials/granite_diffuse";
};

//******************************************************************************
// Triplanar mapping with a separate texture for the z axis
//******************************************************************************

new ShaderData( Triplanar_z )
{
   DXVertexShaderFile   = "shaders/common/triplanar/hlsl/Triplanar_V.hlsl";
   DXPixelShaderFile    = "shaders/common/triplanar/hlsl/Triplanar_z_P.hlsl";
 
   //OGLVertexShaderFile   = "shaders/common/ribbons/gl/basicRibbonShaderV.glsl";
   //OGLPixelShaderFile    = "shaders/common/ribbons/gl/basicRibbonShaderP.glsl";
 
   samplerNames[0] = "$diffuseMap";
   samplerNames[1] = "$diffuseMapZ";
   samplerNames[2] = "$bumpMap";
   samplerNames[3] = "$bumpMapZ";
 
   pixVersion = 2.1;
};

singleton CustomMaterial(triplanar2)
{
   shader = Triplanar_z;
   
   sampler["diffuseMap"] = "art/materials/granite_diffuse";
   Sampler["diffuseMapZ"] = "art/materials/mossy_diffuse";
   Sampler["bumpMap"] = "art/materials/granite_normal";
   Sampler["bumpMapZ"] = "art/materials/mossy_normal";
   
   forwardLit = true;
};