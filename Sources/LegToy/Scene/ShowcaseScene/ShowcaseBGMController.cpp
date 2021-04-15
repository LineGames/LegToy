////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseBGMController.cpp (LegToy - LegToy)
//
// BGM Player Showcase Controller Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseBGMController.h"

// Standard Library
#include <cmath>
#include <functional>

// External Library
#include <fmt/format.h>

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>

// Leggiero.Sound
#include <Sound/BGMPlayerComponent.h>
#include <Sound/Common/IBGMPlayingHandle.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Element/UIElementSingleSimpleButton.h>
#include <LUI/Element/UIElementSimpleSlideBar.h>
#include <LUI/Element/UIElementCachedVariantText.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Prefab/UIFabricatedComponent.h>
#include <LUI/Controller/CommonButtonRenderingController.h>
#include <LUI/Controller/SimpleSliderRenderingController.h>
#include <LUI/Controller/OnOffButtonController.h>

// LegToy - Showcase Scene
#include "ShowcaseLogicCenter.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// BGM Player Showcase Internal

		namespace _BGMPlayerInternal
		{
			const static std::string kBGMFilePath("Showcase/BGM/Nineteen.mp3");

			Leggiero::GameDataString TimeToString(float timeInSec)
			{
				int integerTimeInSec = (int)round(timeInSec);
				int lengthMinutePart = integerTimeInSec / 60;
				int lengthSecondPart = integerTimeInSec % 60;
				return fmt::format("{0:#02}:{1:#02}", lengthMinutePart, lengthSecondPart);
			}
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcaseBGMController

		//------------------------------------------------------------------------------
		ShowcaseBGMController::ShowcaseBGMController(ShowcaseLogicCenter &logicCenter)
			: ShowcaseControllerBase(logicCenter)
			, m_bgmPlayer(nullptr)
			, m_isRepeating(false), m_isPlaying(false), m_isSeeking(false)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseBGMController::~ShowcaseBGMController()
		{
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
			Leggiero::Sound::BGMPlayerComponent *bgmPlayer)
		{
			m_bgmPlayer = bgmPlayer;
			m_playingHandle = m_bgmPlayer->PlayInBundle(_BGMPlayerInternal::kBGMFilePath, false, 1.0f, false, 
				std::bind(&ShowcaseBGMController::_OnBGMPlayFinished, this, std::placeholders::_1, std::placeholders::_2));
			m_playingHandle->Prepare();

			std::shared_ptr<Leggiero::LUI::Description::DescriptionUnit> showcaseUnit = descriptionManager.GetUnit("Showcase/BGM");

			m_uiMainObject = descriptionManager.FabricateUIObjectByPath(showcaseUnit, "ShowcaseUIPrefab", uiManager);
			uiPage->AddPostUIObject(m_uiMainObject);

			std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent(m_uiMainObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));

			std::shared_ptr<Leggiero::LUI::UIObject> contentsObject = showcaseUIFabricatedComponent->FindObject("playerHolder");

			// Initialize Buttons
			m_playButton = std::dynamic_pointer_cast<Leggiero::LUI::Element::SingleSimpleButton>(showcaseUIFabricatedComponent->FindObject("btnPlay"));
			m_playButton->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::ActivatableButtonRenderingController>(*m_playButton));
			m_playButton->SetOnClickHandler(std::bind(&ShowcaseBGMController::_OnPlayButtonClicked, this, std::placeholders::_1));

			m_rewindButton = std::dynamic_pointer_cast<Leggiero::LUI::Element::SingleSimpleButton>(showcaseUIFabricatedComponent->FindObject("btnRewind"));
			m_rewindButton->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::ActivatableButtonRenderingController>(*m_rewindButton));
			m_rewindButton->SetOnClickHandler(std::bind(&ShowcaseBGMController::_OnRewindButtonClicked, this, std::placeholders::_1));

			m_pauseButton = std::dynamic_pointer_cast<Leggiero::LUI::Element::SingleSimpleButton>(showcaseUIFabricatedComponent->FindObject("btnPause"));
			m_pauseButton->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::ActivatableButtonRenderingController>(*m_pauseButton));
			m_pauseButton->SetOnClickHandler(std::bind(&ShowcaseBGMController::_OnPauseButtonClicked, this, std::placeholders::_1));

			// Repeat On/Off Button
			Leggiero::LUI::UICoordinateRatioType onOffButtonTouchHeightRatio = static_cast<Leggiero::LUI::UICoordinateRatioType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "onOffButtonTouchHeightRatio"));
			Leggiero::LUI::UICoordinateType onOffButtonOffLeft = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "onOffButtonOffLeft"));
			m_repeatOnOffController = std::make_shared<Leggiero::LUI::Controller::OnOffButtonController>(showcaseUIFabricatedComponent->FindObject("toggleRepeat"), onOffButtonTouchHeightRatio, onOffButtonOffLeft,
				std::bind(&ShowcaseBGMController::_OnRepeatValueChanged, this, std::placeholders::_1));
			m_repeatOnOffController->SetState(false);
			m_repeatOnOffController->StopAnimationEffect();

			// Initialize Scroll Bar
			m_slider = std::dynamic_pointer_cast<Leggiero::LUI::Element::UIElementSimpleSlideBar>(descriptionManager.FabricateUIObjectByPath("Showcase/BGM", "ScrollBarPrefab", uiManager));
			m_slider->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::SimpleSliderBasicRenderingController>(m_slider->GetMarkerObject(), nullptr));
			m_slider->SetOnTouchStateChangedHandler(std::bind(&ShowcaseBGMController::_OnSliderTouchStateChanged, this, std::placeholders::_1, std::placeholders::_2));
			contentsObject->AddPostUIObject(m_slider);

			// Time UI
			m_totalTimeText = std::dynamic_pointer_cast<Leggiero::LUI::Element::UIElementCachedVariantText>(showcaseUIFabricatedComponent->FindObject("txtTimeTotal"));
			m_totalTimeText->SetString(_BGMPlayerInternal::TimeToString(m_playingHandle->GetLength()));
			m_totalTimeText->GraphicPrepare();

			m_currentTimeText = std::dynamic_pointer_cast<Leggiero::LUI::Element::UIElementCachedVariantText>(showcaseUIFabricatedComponent->FindObject("txtTimeCurrent"));
			m_currentTimeText->SetString("00:00");
			m_currentTimeText->GraphicPrepare();

			_InitializeMainUI(m_uiMainObject, descriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_ResetCaseStates()
		{
			m_playingHandle->Seek(0.0f);
			m_playingHandle->Pause();
			m_isPlaying = false;

			m_currentTimeText->SetString("00:00");
			m_slider->ForceSetValue(0.0, true);

			m_playButton->SetIsActive(false);
			m_rewindButton->SetIsActive(false);
			m_pauseButton->SetIsActive(true);

			m_isRepeating = false;
			m_repeatOnOffController->SetState(false);
			m_repeatOnOffController->StopAnimationEffect();

			m_isSeeking = false;
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_FinalizeCaseController()
		{
			m_repeatOnOffController.reset();

			m_playButton->DiscardOnClickHandler();
			m_rewindButton->DiscardOnClickHandler();
			m_pauseButton->DiscardOnClickHandler();
			m_playButton.reset();
			m_rewindButton.reset();
			m_pauseButton.reset();

			m_slider->DiscardOnTouchStateChangedHandler();
			m_slider.reset();

			m_totalTimeText.reset();
			m_currentTimeText.reset();

			m_playingHandle->Stop();
			m_playingHandle.reset();
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_ProcessCaseFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval)
		{
			m_repeatOnOffController->UpdateFrame(frameInterval);

			if (m_isSeeking)
			{
				double settedValue = Leggiero::Utility::Math::ClipMinMax(m_slider->GetValue(), 0.0, 1.0);
				float targetPosition = static_cast<float>(settedValue * m_playingHandle->GetLength());

				m_currentTimeText->SetString(_BGMPlayerInternal::TimeToString(targetPosition));
			}
			else
			{
				m_currentTimeText->SetString(_BGMPlayerInternal::TimeToString(m_playingHandle->GetCurrentPosition()));

				float currentTimeRatio = Leggiero::Utility::Math::ClipMinMax(m_playingHandle->GetCurrentPosition() / m_playingHandle->GetLength(), 0.0f, 1.0f);
				m_slider->ForceSetValue(currentTimeRatio, true);
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_OnBGMPlayFinished(Leggiero::Sound::IBGMPlayingHandle *sender, bool isFinishedSuccessfully)
		{
			if (isFinishedSuccessfully)
			{
				if (m_isRepeating)
				{
					m_isPlaying = true;
					m_playButton->SetIsActive(true);
					m_pauseButton->SetIsActive(false);

					sender->Seek(0.0f);
					sender->Play();
				}
				else
				{
					m_isPlaying = false;
					m_playButton->SetIsActive(false);
					m_pauseButton->SetIsActive(true);
				}
			}
			else
			{
				m_playingHandle = m_bgmPlayer->PlayInBundle(_BGMPlayerInternal::kBGMFilePath, false, 1.0f, false,
					std::bind(&ShowcaseBGMController::_OnBGMPlayFinished, this, std::placeholders::_1, std::placeholders::_2));
				if (m_isPlaying)
				{
					m_playingHandle->Play();
					m_playButton->SetIsActive(true);
					m_pauseButton->SetIsActive(false);
				}
				else
				{
					m_playButton->SetIsActive(false);
					m_pauseButton->SetIsActive(true);
				}
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_OnPlayButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender)
		{
			if (m_isPlaying || m_isSeeking)
			{
				return;
			}

			m_isPlaying = true;
			m_playButton->SetIsActive(true);
			m_pauseButton->SetIsActive(false);

			m_playingHandle->Play();
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_OnRewindButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender)
		{
			if (m_isSeeking)
			{
				return;
			}

			m_playingHandle->Seek(0.0f);
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_OnPauseButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender)
		{
			if (!m_isPlaying || m_isSeeking)
			{
				return;
			}

			m_isPlaying = false;
			m_playButton->SetIsActive(false);
			m_pauseButton->SetIsActive(true);

			m_playingHandle->Pause();
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_OnRepeatValueChanged(bool isRepeat)
		{
			m_isRepeating = isRepeat;
		}

		//------------------------------------------------------------------------------
		void ShowcaseBGMController::_OnSliderTouchStateChanged(std::shared_ptr<Leggiero::LUI::Element::UIElementSimpleSlideBar> sender, bool isTouched)
		{
			if (isTouched)
			{
				m_isSeeking = true;
				if (m_isPlaying)
				{
					m_playingHandle->Pause();
				}
			}
			else
			{
				if (m_isSeeking)
				{
					m_isSeeking = false;

					double settedValue = Leggiero::Utility::Math::ClipMinMax(sender->GetValue(), 0.0, 1.0);

					float targetPosition = static_cast<float>(settedValue * m_playingHandle->GetLength());
					m_playingHandle->Seek(targetPosition);
					
					if (m_isPlaying)
					{
						m_playingHandle->Play();
					}
				}
			}
		}
	}
}
