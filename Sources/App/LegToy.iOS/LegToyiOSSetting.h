////////////////////////////////////////////////////////////////////////////////
// LegToyiOSSetting.h (LegToy - iOS)
//
// iOS Application Setting
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY_APP_IOS__LEGTOY_IOS_SETTING_H
#define __LEGTOY_APP_IOS__LEGTOY_IOS_SETTING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Application
#include <Engine/Application/IGameLaunchOption.h>


namespace LegToy
{
    // Code Level Settings
    namespace Setting
    {
        extern bool kIsLandscapeMode;
    }


	// iOS Application Launch Setting
	class LegToyiOSSetting
		: public Leggiero::IGameLaunchOption
	{
	public:
        LegToyiOSSetting();
		virtual ~LegToyiOSSetting();
	};
}

#endif
