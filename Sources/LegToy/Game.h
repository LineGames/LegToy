////////////////////////////////////////////////////////////////////////////////
// Game.h (LegToy - LegToy)
//
// Game Class
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__GAME_H
#define __LEGTOY__GAME_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Engine
#include <Engine/Toolbox/ModuledGame/ModuledGame.h>
#include <Engine/Toolbox/Scene/GameSceneHolder.h>


namespace LegToy
{
	// Forward Declarations
	class SharedResources;


	// Real Game Class of LegToy Project
	class Game
		: public Leggiero::Toolbox::Game::ModuledGame
		, public Leggiero::Toolbox::Scene::GameSceneHolder
	{
	public:
		Game();
		virtual ~Game();

	public:	// BaseGame
		virtual bool _UpdateFrame(Leggiero::GameTimeClockType::time_point frameTime) override;

		virtual void _Prepare() override;
		virtual void _GraphicPrepare() override;

		virtual void _GraphicShutdown() override;
		virtual void _Shutdown() override;

	public:	// ModuledGame
		virtual void _InitializeUsingModules(Leggiero::IPlatformApplication *application) override;
		virtual void _RegisterUsingComponents() override;

	protected:	// GameSceneHolder
		virtual void _PrepareGameScenes() override;

	protected:
		void _FinalizeGameScenes();

	protected:
		SharedResources *m_pSharedResources;
	};
}

#endif
