////////////////////////////////////////////////////////////////////////////////
// LegToyWinPCSetting.h (LegToy - LegToy.WinPC)
//
// WinPC Application Setting
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Standard Library
#include <string>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Platform.WinPC
#include <WindowsCommon.h>

// Leggiero.Application
#include <Engine/Application/IGameLaunchOption.h>


namespace LegToy
{
	// WinPC Application Setting
	class LegToyWinPCSetting
		: public Leggiero::IGameLaunchOption
	{
	public:
		LegToyWinPCSetting();
		virtual ~LegToyWinPCSetting();

	public:
		void ParseCommandLine(LPTSTR cmdLine);

	public:
		LPCTSTR windowName;

		int screenWidth;
		int screenHeight;

		float screenXPPI;
		float screenYPPI;

		bool isTablet;

		std::string localeString;
		std::string referrer;

	public:
		float simulatedSafePaddingLeft;
		float simulatedSafePaddingTop;
		float simulatedSafePaddingRight;
		float simulatedSafePaddingBottom;

	protected:
		void _InitializeDefaultValues();
	};
}
