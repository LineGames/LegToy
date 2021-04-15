////////////////////////////////////////////////////////////////////////////////
// Game.cpp (LegToy - LegToy)
//
// Implementation of Game Class
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "Game.h"

// LegToy
#include "Common/SharedResources.h"


namespace LegToy
{
	//////////////////////////////////////////////////////////////////////////////// Game

	//------------------------------------------------------------------------------
	Game::Game()
		: m_pSharedResources(SharedResources::CreateSharedResources())
	{
	}

	//------------------------------------------------------------------------------
	Game::~Game()
	{
		if (m_pSharedResources != nullptr)
		{
			delete m_pSharedResources;
		}
	}

	//------------------------------------------------------------------------------
	bool Game::_UpdateFrame(Leggiero::GameTimeClockType::time_point frameTime)
	{
		if (!GameSceneProcessFrame(frameTime))
		{
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------
	void Game::_Prepare()
	{
		m_pSharedResources->PrepareResources(this);
		GameSceneProcessPrepare();
	}

	//------------------------------------------------------------------------------
	void Game::_GraphicPrepare()
	{
		m_pSharedResources->GraphicPrepareResources(this);
		GameSceneProcessGraphicPrepare();
	}

	//------------------------------------------------------------------------------
	void Game::_GraphicShutdown()
	{
		GameSceneProcessGraphicShutdown();
		m_pSharedResources->GraphicShutdownResources(this);
	}

	//------------------------------------------------------------------------------
	void Game::_Shutdown()
	{
		GameSceneProcessShutdown();
		_FinalizeGameScenes();
		m_pSharedResources->ShutdownResources(this);

		delete m_pSharedResources;
		m_pSharedResources = nullptr;
	}
}
