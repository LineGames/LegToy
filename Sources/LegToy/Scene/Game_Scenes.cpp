////////////////////////////////////////////////////////////////////////////////
// Scene/Game_Scenes.cpp (LegToy - LegToy)
//
// Implementation of Game Class's Scene related parts
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "../Game.h"

// Leggiero.Engine
#include <Engine/Toolbox/Scene/IGameScene.h>

// LegToy
#include "SceneIds.h"

#include "InitialLoadingScene/InitialLoadingScene.h"
#include "ShowcaseScene/ShowcaseScene.h"


namespace LegToy
{
	//////////////////////////////////////////////////////////////////////////////// Game - Scenes

	//------------------------------------------------------------------------------
	void Game::_PrepareGameScenes()
	{
		InitialLoadingSceneStuffs::InitialLoadingScene *initialLoadingScene = new InitialLoadingSceneStuffs::InitialLoadingScene(this, this, m_pSharedResources);
		RegisterGameScene(initialLoadingScene);

		ShowcaseSceneStuffs::ShowcaseScene *showcaseScene = new ShowcaseSceneStuffs::ShowcaseScene(this, this, m_pSharedResources);
		RegisterGameScene(showcaseScene);

		// Start Scene
		RequestSceneChange(SceneIds::kInitialLoading);
	}

	//------------------------------------------------------------------------------
	void Game::_FinalizeGameScenes()
	{
		std::vector<Leggiero::Toolbox::Scene::IGameScene *> allScenes = GetAllRegisteredScenes();
		for (Leggiero::Toolbox::Scene::IGameScene *deletingScene : allScenes)
		{
			UnRegisterGameScene(deletingScene->GetSceneId());
			delete deletingScene;
		}
	}
}
