////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseBGMController.h (LegToy - LegToy)
//
// BGM Player Showcase UI Controller
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_BGM_CONTROLLER_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_BGM_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include <LUI/Common/UICommonTypes.h>

// LegToy - Showcase Scene
#include "ShowcaseControllerBase.h"


// Forward Declarations
namespace Leggiero
{
	namespace Sound
	{
		class BGMPlayerComponent;
		class IBGMPlayingHandle;
	}

	namespace LUI
	{
		class UIObject;
		class UIManager;

		namespace Description
		{
			class DescriptionManager;
		}

		namespace Element
		{
			class SingleSimpleButton;
			class UIElementSimpleSlideBar;
			class UIElementCachedVariantText;
		}

		namespace Controller
		{
			class OnOffButtonController;
		}
	}
}


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		// Forward Declarations
		class ShowcaseLogicCenter;


		// BGM Player Showcase Controller
		class ShowcaseBGMController
			: public ShowcaseControllerBase
		{
		public:
			ShowcaseBGMController(ShowcaseLogicCenter &logicCenter);
			virtual ~ShowcaseBGMController();

		public:
			void InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
				Leggiero::Sound::BGMPlayerComponent *bgmPlayer);

		protected:	// ShowcaseControllerBase
			virtual void _ResetCaseStates() override;
			virtual void _FinalizeCaseController() override;

			virtual void _ProcessCaseFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval) override;

		protected:
			Leggiero::Sound::BGMPlayerComponent *m_bgmPlayer;

			bool m_isRepeating;
			bool m_isPlaying;
			bool m_isSeeking;

			std::shared_ptr<Leggiero::Sound::IBGMPlayingHandle> m_playingHandle;

			std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> m_playButton;
			std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> m_rewindButton;
			std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> m_pauseButton;

			std::shared_ptr<Leggiero::LUI::Element::UIElementSimpleSlideBar> m_slider;

			std::shared_ptr<Leggiero::LUI::Element::UIElementCachedVariantText> m_totalTimeText;
			std::shared_ptr<Leggiero::LUI::Element::UIElementCachedVariantText> m_currentTimeText;

			std::shared_ptr<Leggiero::LUI::Controller::OnOffButtonController> m_repeatOnOffController;

		protected:
			void _OnBGMPlayFinished(Leggiero::Sound::IBGMPlayingHandle *sender, bool isFinishedSuccessfully);

			void _OnPlayButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender);
			void _OnRewindButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender);
			void _OnPauseButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender);

			void _OnRepeatValueChanged(bool isRepeat);

			void _OnSliderTouchStateChanged(std::shared_ptr<Leggiero::LUI::Element::UIElementSimpleSlideBar> sender, bool isTouched);
		};
	}
}

#endif
