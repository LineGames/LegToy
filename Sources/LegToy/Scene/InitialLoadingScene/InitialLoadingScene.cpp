////////////////////////////////////////////////////////////////////////////////
// Scene/InitialLoadingScene/InitialLoadingScene.cpp (LegToy - LegToy)
//
// Game Loading Scene Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "InitialLoadingScene.h"

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IPlatformAppControl.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>

// LegToy
#include "../../Game.h"
#include "../../Common/SharedResources.h"
#include "../../Common/IResource_CommonRendering.h"
#include "../../Common/BackgroundRenderer.h"

// LegToy - Showcase Scene
#include "../ShowcaseScene/ShowcaseScene.h"

// LegToy - Initial Loading Scene
#include "LoadingMarker.h"


namespace LegToy
{
	namespace InitialLoadingSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// InitialLoadingScene

		//------------------------------------------------------------------------------
		InitialLoadingScene::InitialLoadingScene(Leggiero::Toolbox::Scene::IGameSceneContext *ownerContext, Leggiero::IGame *game, SharedResources *sharedResources)
			: Leggiero::Toolbox::Scene::IGameScene(ownerContext)
			, m_game(game), m_sharedResources(sharedResources)
			, m_showcaseScene(nullptr)
		{
			m_renderingResources = dynamic_cast<SharedResource::ICommonRenderingResources *>(sharedResources);
		}

		//------------------------------------------------------------------------------
		InitialLoadingScene::~InitialLoadingScene()
		{
		}

		//------------------------------------------------------------------------------
		// Prepare Scene Just Before Enter
		void InitialLoadingScene::BeforeSceneEnter()
		{
			m_lastFrameTime = Leggiero::GameTimeClockType::now();

			// Prevent sleep during loading (can lead some error)
			m_game->GetComponent<Leggiero::Application::ApplicationComponent>()->PlatformAppControl().SetNoSleepMode(true);

			m_showcaseScene = dynamic_cast<ShowcaseSceneStuffs::ShowcaseScene *>(m_ownerContext->GetSceneObject(SceneIds::kShowcase));

			m_loadingMarker->ResetState();

			m_showcaseScene->StartLoading();
		}

		//------------------------------------------------------------------------------
		// Process Scene Just After Exit
		void InitialLoadingScene::AfterSceneExit()
		{
			m_game->GetComponent<Leggiero::Application::ApplicationComponent>()->PlatformAppControl().SetNoSleepMode(false);
		}

		//------------------------------------------------------------------------------
		// Process a Game Frame
		void InitialLoadingScene::ProcessFrame(Leggiero::GameTimeClockType::time_point frameReferenceTime)
		{
			// Logic Process
			Leggiero::GameTimeClockType::duration frameInterval = frameReferenceTime - m_lastFrameTime;
			m_lastFrameTime = frameReferenceTime;

			m_renderingResources->GetCommonBackgroundRenderer()->UpdateFrame(frameInterval);

			if (m_showcaseScene->IsLoadingFinished())
			{
				m_loadingMarker->NotifyLoadFinished();
			}
			m_loadingMarker->UpdateFrame(frameInterval);
			if (m_loadingMarker->IsMarkerFinished())
			{
				m_renderingResources->GetCommonBackgroundRenderer()->StartToShowcaseScene();
				m_ownerContext->RequestSceneChange(SceneIds::kShowcase);
			}

			// Rendering
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_renderingResources->GetCommonBackgroundRenderer()->Render();
			m_loadingMarker->Render();
		}

		//------------------------------------------------------------------------------
		// Initialize Scene's Graphic Contents
		// Called in Graphic Thread
		void InitialLoadingScene::_InitializeSceneGraphic()
		{
			m_loadingMarker = std::make_shared<LoadingMarker>(
				m_game->GetComponent<Leggiero::Application::ApplicationComponent>(), 
				m_renderingResources->GetCommonShader_ColorSimple()
				);
		}

		//------------------------------------------------------------------------------
		// Finalize Scene's Graphic Contents
		// Called in Graphic Thread
		void InitialLoadingScene::_FinalizeSceneGraphic()
		{
			m_loadingMarker.reset();
		}
	}
}
