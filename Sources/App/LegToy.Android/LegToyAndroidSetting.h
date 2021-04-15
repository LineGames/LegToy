////////////////////////////////////////////////////////////////////////////////
// LegToyAndroidSetting.h (LegToy - LegToy.Android)
//
// Android Application Setting
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY_APP_ANDROID__LEGTOY_ANDROID_SETTING_H
#define __LEGTOY_APP_ANDROID__LEGTOY_ANDROID_SETTING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Application
#include <Engine/Application/IGameLaunchOption.h>


namespace LegToy
{
	// Android Application Launch Setting
	class LegToyAndroidSetting
		: public Leggiero::IGameLaunchOption
	{
	public:
		LegToyAndroidSetting();
		virtual ~LegToyAndroidSetting();
	};
}

#endif
