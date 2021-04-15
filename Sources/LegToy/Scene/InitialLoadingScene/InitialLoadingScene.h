////////////////////////////////////////////////////////////////////////////////
// Scene/InitialLoadingScene/InitialLoadingScene.h (LegToy - LegToy)
//
// LegToy app loading scene
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__INITIAL_LOADING_SCENE__INITIAL_LOADING_SCENE_H
#define __LEGTOY__SCENE__INITIAL_LOADING_SCENE__INITIAL_LOADING_SCENE_H


// Standard Library
#include <memory>

// External Library
#include <GLES3.h>
#include <glm/glm.hpp>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Engine
#include <Engine/Application/IGame.h>
#include <Engine/Toolbox/Scene/IGameScene.h>

// Leggiero.Graphics
#include <Graphics/Texture/GLTextureResource.h>
#include <Graphics/Shader/Basic/TexturedColorSimpleShader.h>

// LegToy
#include "../SceneIds.h"


namespace LegToy
{
	// Forward Declarations
	class SharedResources;

	namespace SharedResource
	{
		class ICommonRenderingResources;
	}

	namespace ShowcaseSceneStuffs
	{
		class ShowcaseScene;
	}


	namespace InitialLoadingSceneStuffs
	{
		// Forward Declaration
		class LoadingMarker;


		// Application Loading Scene
		class InitialLoadingScene
			: public Leggiero::Toolbox::Scene::IGameScene
		{
		public:
			InitialLoadingScene(Leggiero::Toolbox::Scene::IGameSceneContext *ownerContext, Leggiero::IGame *game, SharedResources *sharedResources);
			virtual ~InitialLoadingScene();

		public:	// IGameScene
			// Get Id of the Scene
			virtual Leggiero::Toolbox::Scene::GameSceneIdType GetSceneId() const override { return SceneIds::kInitialLoading; }

			// Prepare Scene Just Before Enter
			virtual void BeforeSceneEnter() override;

			// Process Scene Just After Exit
			virtual void AfterSceneExit() override;

			// Process a Game Frame
			virtual void ProcessFrame(Leggiero::GameTimeClockType::time_point frameReferenceTime) override;

		protected:	// IGameScene
			// Initialize Scene's Graphic Contents
			// Called in Graphic Thread
			virtual void _InitializeSceneGraphic() override;

			// Finalize Scene's Graphic Contents
			// Called in Graphic Thread
			virtual void _FinalizeSceneGraphic() override;

		protected:
			Leggiero::IGame *m_game;
			SharedResources *m_sharedResources;
			SharedResource::ICommonRenderingResources *m_renderingResources;

			Leggiero::GameTimeClockType::time_point m_lastFrameTime;

			ShowcaseSceneStuffs::ShowcaseScene *m_showcaseScene;

			std::shared_ptr<LoadingMarker> m_loadingMarker;
		};
	}
}

#endif
