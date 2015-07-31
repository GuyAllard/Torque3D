#include "backdropShape.h"
#include "scene/sceneRenderState.h"
#include "renderInstance/renderPassManager.h"
#include "ts/tsShapeInstance.h"
#include "gfx/gfxTransformSaver.h"
#include "lighting/lightManager.h"

IMPLEMENT_CO_NETOBJECT_V1(BackdropShape);

BackdropShape::BackdropShape()
{
   // backdrop shapes are not really part of the universe that the player
   // can interact with, so disable collision
   mCollisionType = None;
   mDecalType = None;
   mTypeMask = BackdropObjectType;
}


bool BackdropShape::onAdd()
{
   if(!Parent::onAdd())
      return false;

   setGlobalBounds();
   resetWorldBox();
	
   return true;
}


void BackdropShape::prepRenderImage(SceneRenderState* state)
{
   if( !mShapeInstance )
      return;

   // the position of a backdrop shape is always relative to the camera,
   // so we use the objects position for LOD selection
   F32 dist = getRenderPosition().len();
   if (dist < 0.01f)
      dist = 0.01f;

   F32 invScale = (1.0f/getMax(getMax(mObjScale.x,mObjScale.y),mObjScale.z));   

   if ( mForceDetail == -1 )
      mShapeInstance->setDetailFromDistance( state, dist * invScale );
   else
      mShapeInstance->setCurrentDetail( mForceDetail );

   if ( mShapeInstance->getCurrentDetail() < 0 )
      return;

   GFXTransformSaver saver;
   
   // Set up our TS render state.
   TSRenderState rdata;

   rdata.setSceneState( state );
   rdata.setFadeOverride( 1.0f );
   rdata.setOriginSort( mUseOriginSort );

   // We might have some forward lit materials
   // so pass down a query to gather lights.
   LightQuery query;
   query.init( getWorldSphere() );
   rdata.setLightQuery( &query );

   MatrixF mat = getRenderTransform();
   // our position is always relative to the camera position
   mat.setPosition(getRenderPosition() + state->getDiffuseCameraPosition());
   mat.scale( mObjScale );
   GFX->setWorldMatrix( mat );

   mShapeInstance->animate();

   // pass a hint that we should use the backdrop bins which cause the shape
   // to render after the sky and before the rest of the scene
   rdata.setBackdropBinHint( true );
   
   mShapeInstance->render( rdata );
}
