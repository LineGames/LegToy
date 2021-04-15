////////////////////////////////////////////////////////////////////////////////
// Common/SharedResources.h (LegToy - LegToy)
//
// LegToy Game Shared Resources
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__COMMON__SHARED_RESOURCES_H
#define __LEGTOY__COMMON__SHARED_RESOURCES_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


namespace LegToy
{
	// Forward Declaration
	class Game;


	// Game Shared Resources
	class SharedResources
	{
	public:
		virtual ~SharedResources() { }

	public:
		virtual void PrepareResources(Game *pGame) = 0;
		virtual void GraphicPrepareResources(Game *pGame) = 0;

		virtual void GraphicShutdownResources(Game *pGame) = 0;
		virtual void ShutdownResources(Game *pGame) = 0;

	public:
		static SharedResources *CreateSharedResources();
	};
}

#endif
