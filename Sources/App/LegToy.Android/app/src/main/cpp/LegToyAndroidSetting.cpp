////////////////////////////////////////////////////////////////////////////////
// LegToyAndroidSetting.cpp (LegToy - LegToy.Android)
//
// Basic Implementation for Android Application Setting
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "LegToyAndroidSetting.h"

// Leggiero.Platform.Android
#include <AndroidPlatformSetting.h>


namespace LegToy
{
	//////////////////////////////////////////////////////////////////////////////// LegToyAndroidSetting

	//------------------------------------------------------------------------------
	LegToyAndroidSetting::LegToyAndroidSetting()
	{
	}

	//------------------------------------------------------------------------------
	LegToyAndroidSetting::~LegToyAndroidSetting()
	{

	}
}


//////////////////////////////////////////////////////////////////////////////// Android Platform

namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//------------------------------------------------------------------------------
			IGameLaunchOption *CreateGameLaunchOption()
			{
				// For now, setting contains nothing
				return new LegToy::LegToyAndroidSetting();
			}
		}
	}
}
