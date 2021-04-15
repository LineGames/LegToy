////////////////////////////////////////////////////////////////////////////////
// LegToyiOSSetting.mm (LegToy - iOS)
//
// Basic Implementation for iOS Application Setting
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "LegToyiOSSetting.h"

// Leggiero.Platform.iOS
#include <iOSPlatformSetting.h>


namespace LegToy
{
    //////////////////////////////////////////////////////////////////////////////// Code Level Settings

    //------------------------------------------------------------------------------
    namespace Setting
    {
        bool kIsLandscapeMode = true;
    }


	//////////////////////////////////////////////////////////////////////////////// LegToyiOSSetting

	//------------------------------------------------------------------------------
    LegToyiOSSetting::LegToyiOSSetting()
	{
	}

	//------------------------------------------------------------------------------
    LegToyiOSSetting::~LegToyiOSSetting()
	{

	}
}


//////////////////////////////////////////////////////////////////////////////// iOS Platform

namespace Leggiero
{
    namespace Platform
    {
        namespace iOS
        {
            //------------------------------------------------------------------------------
            IGameLaunchOption *CreateGameLaunchOption()
            {
                // For now, setting contains nothing
                return new LegToy::LegToyiOSSetting();
            }
        }
    }
}
