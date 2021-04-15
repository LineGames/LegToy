////////////////////////////////////////////////////////////////////////////////
// LegToyWinPCSetting.h (LegToy - LegToy.WinPC)
//
// Implementation for WinPC Application Setting
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "LegToyWinPCSetting.h"


namespace LegToy
{
	//////////////////////////////////////////////////////////////////////////////// LegToyWinPCSetting

	//------------------------------------------------------------------------------
	LegToyWinPCSetting::LegToyWinPCSetting()
	{
		_InitializeDefaultValues();
	}

	//------------------------------------------------------------------------------
	LegToyWinPCSetting::~LegToyWinPCSetting()
	{

	}

	//------------------------------------------------------------------------------
	void LegToyWinPCSetting::ParseCommandLine(LPTSTR cmdLine)
	{
		//TODO
	}

	//------------------------------------------------------------------------------
	void LegToyWinPCSetting::_InitializeDefaultValues()
	{
#ifdef _DEBUG
		windowName = L"LegToy - x64 (Debug)";
#else
		windowName = L"LegToy";
#endif

		screenWidth = 1280;
		screenHeight = 720;

		screenXPPI = 326.0f;
		screenYPPI = 326.0f;

		isTablet = false;

		localeString = "en_US";

		simulatedSafePaddingLeft = 0.0f;
		simulatedSafePaddingTop = 0.0f;
		simulatedSafePaddingRight = 0.0f;
		simulatedSafePaddingBottom = 0.0f;
	}
}
