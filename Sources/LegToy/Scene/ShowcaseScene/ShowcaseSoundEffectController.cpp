////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseSoundEffectController.cpp (LegToy - LegToy)
//
// Hash Test Showcase Controller Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseSoundEffectController.h"

// Standard Library
#include <functional>
#include <sstream>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>
#include <Utility/Sugar/Finally.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/SystemEventDispatcher.h>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Sound
#include <Sound/SoundMixerComponent.h>
#include <Sound/Common/SoundTypes.h>
#include <Sound/Provider/BakedSoundProvider.h>
#include <Sound/Provider/WavSoundProvider.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Common/UICommonArea.h>
#include <LUI/Touch/UITouch.h>
#include <LUI/Touch/ValuedTouchComponent.h>
#include <LUI/Element/UIElementSingleSimpleButton.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Prefab/UIFabricatedComponent.h>
#include <LUI/Controller/CommonButtonRenderingController.h>

// LegToy - Showcase Scene
#include "ShowcaseLogicCenter.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// Sound Effect Showcase Internal

		namespace _SoundEffectInternal
		{
			//------------------------------------------------------------------------------
			const static std::string kSoundPath_SE1("Showcase/SE/sound_01.wav");
			const static std::string kSoundPath_SE2("Showcase/SE/sound_02.wav");
			const static std::string kSoundPath_SE3("Showcase/SE/sound_03.wav");
			const static std::string kSoundPath_SE4("Showcase/SE/sound_04.wav");
			const static std::string kSoundPath_SE5("Showcase/SE/sound_05.wav");

			const static std::string kSoundPath_Long("Showcase/SE/loop_01.wav");
			constexpr Leggiero::Sound::SampleNumberType kLongSoundLoopStart = 6450;
			constexpr Leggiero::Sound::SampleNumberType kLongSoundLoopFinish = 10077;

			constexpr Leggiero::LUI::UICoordinateRatioType kPlayButtonTouchAreaWidthRatio = static_cast<Leggiero::LUI::UICoordinateRatioType>(1.48f);
			constexpr Leggiero::LUI::UICoordinateRatioType kPlayButtonTouchAreaHeightRatio = static_cast<Leggiero::LUI::UICoordinateRatioType>(1.2f);


			//////////////////////////////////////////////////////////////////////////////// PlayButtonController

			//------------------------------------------------------------------------------
			// Sound Effect Play Button Controller
			class PlayButtonController
			{
			public:
				PlayButtonController(std::shared_ptr<Leggiero::LUI::UIObject> buttonObject);
				virtual ~PlayButtonController();

			public:
				void UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval);
				void OnPause();
				void ResetState();

				bool HasPlayingHandle() const { return (bool)m_playingHandle; }
				void SetPlayingHandle(std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext> handle);

			protected:
				void _OnTouchDown(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time, bool isCoveredByDescendant);

			protected:
				std::weak_ptr< Leggiero::LUI::UIObject>		m_buttonObjectRef;
				std::shared_ptr<Leggiero::LUI::UIObject>	m_normalObject;
				std::shared_ptr<Leggiero::LUI::UIObject>	m_playingObject;

				std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext>	m_playingHandle;
				Leggiero::Utility::Threading::SafePthreadLock					m_playingHandleLock;
			};

			//------------------------------------------------------------------------------
			PlayButtonController::PlayButtonController(std::shared_ptr<Leggiero::LUI::UIObject> buttonObject)
				: m_buttonObjectRef(buttonObject)
			{
				std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> fabricatedComponent(buttonObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));
				m_normalObject = fabricatedComponent->FindObject("normal");
				m_playingObject = fabricatedComponent->FindObject("playing");
				m_normalObject->SetIsVisible(true);
				m_playingObject->SetIsVisible(false);

				std::shared_ptr<Leggiero::LUI::ValuedTouchComponent> touchComponent(std::make_shared<Leggiero::LUI::ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<Leggiero::LUI::CommonArea::ObjectBasedArea>(buttonObject, kPlayButtonTouchAreaWidthRatio, kPlayButtonTouchAreaHeightRatio));
				touchComponent->SetCoverArea(touchComponent->GetInteractionArea());
				touchComponent->SetTouchDownFunc(std::bind(&PlayButtonController::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				buttonObject->RegisterComponent(touchComponent);
			}

			//------------------------------------------------------------------------------
			PlayButtonController::~PlayButtonController()
			{
				std::shared_ptr<Leggiero::LUI::UIObject> buttonObject(m_buttonObjectRef.lock());
				if (buttonObject)
				{
					buttonObject->UnRegisterComponent(Leggiero::LUI::UIComponentType::kTouchInteraction);
				}
				buttonObject.reset();

				if (m_playingHandle && !m_playingHandle->IsFinished())
				{
					m_playingHandle->Stop();
				}
			}

			//------------------------------------------------------------------------------
			void PlayButtonController::UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval)
			{
				std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext> playingHandle(m_playingHandle);

				if (playingHandle && playingHandle->IsPaused())
				{
					m_normalObject->SetIsVisible(true);
					m_playingObject->SetIsVisible(false);
				}
			}

			//------------------------------------------------------------------------------
			void PlayButtonController::OnPause()
			{
				std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext> playingHandle(m_playingHandle);

				if (playingHandle)
				{
					if (!playingHandle->IsFinished())
					{
						playingHandle->Stop();
					}
					playingHandle.reset();
					m_playingHandle.reset();

					m_normalObject->SetIsVisible(true);
					m_playingObject->SetIsVisible(false);
				}
			}

			//------------------------------------------------------------------------------
			void PlayButtonController::ResetState()
			{
				std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext> playingHandle(m_playingHandle);
				if (playingHandle)
				{
					playingHandle->Pause();
				}

				m_normalObject->SetIsVisible(true);
				m_playingObject->SetIsVisible(false);
			}

			//------------------------------------------------------------------------------
			void PlayButtonController::SetPlayingHandle(std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext> handle)
			{
				std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext> oldHandle;

				// Only do in lock context
				{
					auto lockContext = m_playingHandleLock.Lock();
					if (lockContext)
					{
						std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext> oldHandle = m_playingHandle;
						m_playingHandle = handle;
					}
				}

				if (oldHandle)
				{
					if (!oldHandle->IsFinished())
					{
						oldHandle->Stop();
					}
				}
			}

			//------------------------------------------------------------------------------
			void PlayButtonController::_OnTouchDown(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time, bool isCoveredByDescendant)
			{
				std::shared_ptr<Leggiero::Sound::BufferedSoundPlayingContext> playingHandle(m_playingHandle);

				if (playingHandle)
				{
					if (!playingHandle->IsPaused())
					{
						playingHandle->Pause();
					}
					playingHandle->Rewind();
					playingHandle->Resume();
					m_normalObject->SetIsVisible(false);
					m_playingObject->SetIsVisible(true);
				}
				else
				{
					m_normalObject->SetIsVisible(false);
					m_playingObject->SetIsVisible(true);
				}
			}


			//////////////////////////////////////////////////////////////////////////////// LongPlayButtonController
			
			//------------------------------------------------------------------------------
			// Long Sound Play Button Controller
			class LongPlayButtonController
			{
			public:
				LongPlayButtonController(std::shared_ptr<Leggiero::LUI::UIObject> buttonObject, 
					Leggiero::Sound::SoundMixerComponent *soundMixer, std::shared_ptr<Leggiero::Sound::ISoundProvider> m_soundProvider, Leggiero::Sound::SampleNumberType loopStartSample, Leggiero::Sound::SampleNumberType loopFinishSample);
				virtual ~LongPlayButtonController();

			public:
				void UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval);
				void OnPause();
				void ResetState();

			protected:
				void _OnTouchDown(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time, bool isCoveredByDescendant);

				void _OnTouchOut(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time);
				void _OnTouchCovered(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time);
				void _OnTouchCancel(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::GameTimeClockType::time_point time);
				void _OnTouchUp(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time);

				void _OnPrimaryTouchLose(std::shared_ptr<Leggiero::LUI::UITouch> touch);

				void _FinishPlaying();

			protected:
				std::shared_ptr<Leggiero::LUI::UIObject>	m_buttonObject;
				std::shared_ptr<Leggiero::LUI::UIObject>	m_normalObject;
				std::shared_ptr<Leggiero::LUI::UIObject>	m_playingObject;

				Leggiero::Sound::SoundMixerComponent				*m_soundMixer;
				std::shared_ptr<Leggiero::Sound::ISoundProvider>	m_soundProvider;
				Leggiero::Sound::SampleNumberType					m_loopStartSample;
				Leggiero::Sound::SampleNumberType					m_loopFinishSample;

				std::shared_ptr<Leggiero::Sound::LoopingSoundPlayingContext>	m_playingContext;

				std::shared_ptr<Leggiero::LUI::UITouch> m_processingTouch;
			};

			//------------------------------------------------------------------------------
			LongPlayButtonController::LongPlayButtonController(std::shared_ptr<Leggiero::LUI::UIObject> buttonObject, Leggiero::Sound::SoundMixerComponent *soundMixer, std::shared_ptr<Leggiero::Sound::ISoundProvider> m_soundProvider, Leggiero::Sound::SampleNumberType loopStartSample, Leggiero::Sound::SampleNumberType loopFinishSample)
				: m_buttonObject(buttonObject)
				, m_soundMixer(soundMixer), m_soundProvider(m_soundProvider), m_loopStartSample(loopStartSample), m_loopFinishSample(loopFinishSample)
			{
				std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> fabricatedComponent(buttonObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));
				m_normalObject = fabricatedComponent->FindObject("normal");
				m_playingObject = fabricatedComponent->FindObject("playing");
				m_normalObject->SetIsVisible(true);
				m_playingObject->SetIsVisible(false);

				std::shared_ptr<Leggiero::LUI::ValuedTouchComponent> touchComponent(std::make_shared<Leggiero::LUI::ValuedTouchComponent>());
				touchComponent->SetInteractionArea(std::make_shared<Leggiero::LUI::CommonArea::ObjectBasedArea>(buttonObject, kPlayButtonTouchAreaWidthRatio, kPlayButtonTouchAreaHeightRatio));
				touchComponent->SetCoverArea(touchComponent->GetInteractionArea());
				touchComponent->SetTouchDownFunc(std::bind(&LongPlayButtonController::_OnTouchDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
				touchComponent->SetTouchOutFunc(std::bind(&LongPlayButtonController::_OnTouchOut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCoveredFunc(std::bind(&LongPlayButtonController::_OnTouchCovered, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetTouchCancelFunc(std::bind(&LongPlayButtonController::_OnTouchCancel, this, std::placeholders::_1, std::placeholders::_2));
				touchComponent->SetTouchUpFunc(std::bind(&LongPlayButtonController::_OnTouchUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				touchComponent->SetPrimaryTouchLoseFunc(std::bind(&LongPlayButtonController::_OnPrimaryTouchLose, this, std::placeholders::_1));
				buttonObject->RegisterComponent(touchComponent);
			}

			//------------------------------------------------------------------------------
			LongPlayButtonController::~LongPlayButtonController()
			{
				if (m_playingContext)
				{
					if (!m_playingContext->IsFinished())
					{
						m_playingContext->StopLooping();
						m_playingContext->Stop();
					}
					m_playingContext.reset();
				}
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval)
			{
				if (!m_playingContext)
				{
					return;
				}
				if (m_playingContext->IsFinished())
				{
					m_playingContext.reset();
					m_normalObject->SetIsVisible(true);
					m_playingObject->SetIsVisible(false);
					return;
				}
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::OnPause()
			{
				if (m_playingContext)
				{
					if (!m_playingContext->IsFinished())
					{
						m_playingContext->Stop();
					}
					m_playingContext.reset();
					m_normalObject->SetIsVisible(true);
					m_playingObject->SetIsVisible(false);
				}
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::ResetState()
			{
				if (m_playingContext)
				{
					if (!m_playingContext->IsFinished())
					{
						m_playingContext->Stop();
					}
					m_playingContext.reset();
				}
				m_normalObject->SetIsVisible(true);
				m_playingObject->SetIsVisible(false);
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::_OnTouchDown(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time, bool isCoveredByDescendant)
			{
				if (m_processingTouch)
				{
					return;
				}
				if (!m_soundProvider)
				{
					return;
				}
				if (m_playingContext)
				{
					return;
				}

				touch->Subscribe(m_buttonObject, true);
				if (!touch->ClaimPrimary(m_buttonObject))
				{
					touch->UnSubscribe(m_buttonObject);
					return;
				}
				m_processingTouch = touch;

				m_playingContext = m_soundMixer->PlayLoopingSound(m_soundProvider, m_loopStartSample, m_loopFinishSample, true);
				if (m_playingContext)
				{
					m_normalObject->SetIsVisible(false);
					m_playingObject->SetIsVisible(true);
				}
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::_OnTouchOut(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time)
			{
				touch->UnSubscribe(m_buttonObject);
				if (!m_processingTouch || m_processingTouch != touch)
				{
					return;
				}
				m_processingTouch.reset();
				_FinishPlaying();
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::_OnTouchCovered(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time)
			{
				touch->UnSubscribe(m_buttonObject);
				if (!m_processingTouch || m_processingTouch != touch)
				{
					return;
				}
				m_processingTouch.reset();
				_FinishPlaying();
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::_OnTouchCancel(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::GameTimeClockType::time_point time)
			{
				touch->UnSubscribe(m_buttonObject);
				if (!m_processingTouch || m_processingTouch != touch)
				{
					return;
				}
				m_processingTouch.reset();
				_FinishPlaying();
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::_OnTouchUp(std::shared_ptr<Leggiero::LUI::UITouch> touch, Leggiero::LUI::UICoordinateType x, Leggiero::LUI::UICoordinateType y, Leggiero::GameTimeClockType::time_point time)
			{
				touch->UnSubscribe(m_buttonObject);
				if (!m_processingTouch || m_processingTouch != touch)
				{
					return;
				}
				m_processingTouch.reset();
				_FinishPlaying();
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::_OnPrimaryTouchLose(std::shared_ptr<Leggiero::LUI::UITouch> touch)
			{
				touch->UnSubscribe(m_buttonObject);
				if (!m_processingTouch || m_processingTouch != touch)
				{
					return;
				}
				m_processingTouch.reset();
				_FinishPlaying();
			}

			//------------------------------------------------------------------------------
			void LongPlayButtonController::_FinishPlaying()
			{
				if (m_playingContext)
				{
					m_playingContext->StopLooping();
				}
			}
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcaseSoundEffectController

		//------------------------------------------------------------------------------
		ShowcaseSoundEffectController::ShowcaseSoundEffectController(ShowcaseLogicCenter &logicCenter)
			: ShowcaseControllerBase(logicCenter)
			, m_appComponent(nullptr), m_soundMixer(nullptr)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseSoundEffectController::~ShowcaseSoundEffectController()
		{
			if (m_appComponent != nullptr)
			{
				m_appComponent->SystemEventCenter().UnRegisterApplicationEventObserver(this);
				m_appComponent = nullptr;
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseSoundEffectController::InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
			Leggiero::Application::ApplicationComponent *appComponent, Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem, Leggiero::Sound::SoundMixerComponent *soundMixer)
		{
			m_appComponent = appComponent;
			m_soundMixer = soundMixer;

			_InitializeSoundSources(bundleFileSystem);

			m_uiMainObject = descriptionManager.FabricateUIObjectByPath("Showcase/SoundEffect", "ShowcaseUIPrefab", uiManager);
			uiPage->AddPostUIObject(m_uiMainObject);

			std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent(m_uiMainObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));

			std::shared_ptr<Leggiero::LUI::UIObject> buttonPlaySE1 = showcaseUIFabricatedComponent->FindObject("btnPlaySE1");
			m_playButtonControllers.push_back(std::make_shared<_SoundEffectInternal::PlayButtonController>(buttonPlaySE1));

			std::shared_ptr<Leggiero::LUI::UIObject> buttonPlaySE2 = showcaseUIFabricatedComponent->FindObject("btnPlaySE2");
			m_playButtonControllers.push_back(std::make_shared<_SoundEffectInternal::PlayButtonController>(buttonPlaySE2));

			std::shared_ptr<Leggiero::LUI::UIObject> buttonPlaySE3 = showcaseUIFabricatedComponent->FindObject("btnPlaySE3");
			m_playButtonControllers.push_back(std::make_shared<_SoundEffectInternal::PlayButtonController>(buttonPlaySE3));

			std::shared_ptr<Leggiero::LUI::UIObject> buttonPlaySE4 = showcaseUIFabricatedComponent->FindObject("btnPlaySE4");
			m_playButtonControllers.push_back(std::make_shared<_SoundEffectInternal::PlayButtonController>(buttonPlaySE4));

			std::shared_ptr<Leggiero::LUI::UIObject> buttonPlaySE5 = showcaseUIFabricatedComponent->FindObject("btnPlaySE5");
			m_playButtonControllers.push_back(std::make_shared<_SoundEffectInternal::PlayButtonController>(buttonPlaySE5));

			std::shared_ptr<Leggiero::LUI::UIObject> buttonPlayLoop = showcaseUIFabricatedComponent->FindObject("btnPlayLong");
			m_longPlayButtonController = std::make_shared<_SoundEffectInternal::LongPlayButtonController>(buttonPlayLoop, 
				soundMixer, m_loopPlayButtonSoundSources[1], _SoundEffectInternal::kLongSoundLoopStart, _SoundEffectInternal::kLongSoundLoopFinish);

			_CheckAndGiveSoundHandles();

			_InitializeMainUI(m_uiMainObject, descriptionManager);

			m_appComponent->SystemEventCenter().RegisterApplicationEventObserver(this);
		}

		//------------------------------------------------------------------------------
		void ShowcaseSoundEffectController::OnPause()
		{
			m_longPlayButtonController->OnPause();
			for (std::shared_ptr<_SoundEffectInternal::PlayButtonController> &currentButtonController : m_playButtonControllers)
			{
				currentButtonController->OnPause();
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseSoundEffectController::OnResume()
		{
			_CheckAndGiveSoundHandles();
		}

		//------------------------------------------------------------------------------
		void ShowcaseSoundEffectController::_ResetCaseStates()
		{
			for (std::shared_ptr<_SoundEffectInternal::PlayButtonController> &currentButtonController : m_playButtonControllers)
			{
				currentButtonController->ResetState();
			}
			m_longPlayButtonController->ResetState();
		}

		//------------------------------------------------------------------------------
		void ShowcaseSoundEffectController::_FinalizeCaseController()
		{
			if (m_appComponent != nullptr)
			{
				m_appComponent->SystemEventCenter().UnRegisterApplicationEventObserver(this);
				m_appComponent = nullptr;
			}

			m_playButtonControllers.clear();
			m_longPlayButtonController.reset();

			m_playButtonSoundSources.clear();
			m_loopPlayButtonSoundSources.clear();

			m_soundMixer = nullptr;
		}

		//------------------------------------------------------------------------------
		void ShowcaseSoundEffectController::_ProcessCaseFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval)
		{
			for (std::shared_ptr<_SoundEffectInternal::PlayButtonController> &currentButtonController : m_playButtonControllers)
			{
				currentButtonController->UpdateFrame(frameInterval);
			}
			m_longPlayButtonController->UpdateFrame(frameInterval);

			_CheckAndGiveSoundHandles();
		}

		//------------------------------------------------------------------------------
		void ShowcaseSoundEffectController::_InitializeSoundSources(Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem)
		{
			std::shared_ptr<std::stringstream> soundFileData;

			soundFileData = std::make_shared<std::stringstream>();
			bundleFileSystem.ReadBundleFileData(_SoundEffectInternal::kSoundPath_SE1, 0, *soundFileData); soundFileData->seekg(0);
			m_playButtonSoundSources.push_back(std::make_shared<Leggiero::Sound::WavSoundProvider>(soundFileData));

			soundFileData = std::make_shared<std::stringstream>();
			bundleFileSystem.ReadBundleFileData(_SoundEffectInternal::kSoundPath_SE2, 0, *soundFileData); soundFileData->seekg(0);
			m_playButtonSoundSources.push_back(std::make_shared<Leggiero::Sound::WavSoundProvider>(soundFileData));

			soundFileData = std::make_shared<std::stringstream>();
			bundleFileSystem.ReadBundleFileData(_SoundEffectInternal::kSoundPath_SE3, 0, *soundFileData); soundFileData->seekg(0);
			m_playButtonSoundSources.push_back(std::make_shared<Leggiero::Sound::WavSoundProvider>(soundFileData));

			soundFileData = std::make_shared<std::stringstream>();
			bundleFileSystem.ReadBundleFileData(_SoundEffectInternal::kSoundPath_SE4, 0, *soundFileData); soundFileData->seekg(0);
			m_playButtonSoundSources.push_back(std::make_shared<Leggiero::Sound::WavSoundProvider>(soundFileData));

			soundFileData = std::make_shared<std::stringstream>();
			bundleFileSystem.ReadBundleFileData(_SoundEffectInternal::kSoundPath_SE5, 0, *soundFileData); soundFileData->seekg(0);
			m_playButtonSoundSources.push_back(std::make_shared<Leggiero::Sound::WavSoundProvider>(soundFileData));

			soundFileData = std::make_shared<std::stringstream>();
			bundleFileSystem.ReadBundleFileData(_SoundEffectInternal::kSoundPath_Long, 0, *soundFileData); soundFileData->seekg(0);
			m_loopPlayButtonSoundSources.push_back(std::make_shared<Leggiero::Sound::WavSoundProvider>(soundFileData));
			m_loopPlayButtonSoundSources.push_back(std::make_shared<Leggiero::Sound::BakedSoundProvider>(*m_loopPlayButtonSoundSources[0]));
		}

		//------------------------------------------------------------------------------
		void ShowcaseSoundEffectController::_CheckAndGiveSoundHandles()
		{
			size_t soundCount = m_playButtonControllers.size();
			if (m_playButtonSoundSources.size() < soundCount)
			{
				soundCount = m_playButtonSoundSources.size();
			}
			for (size_t i = 0; i < soundCount; ++i)
			{
				if (!m_playButtonControllers[i]->HasPlayingHandle())
				{
					m_playButtonControllers[i]->SetPlayingHandle(m_soundMixer->PlayBufferedSound(m_playButtonSoundSources[i], false, 1.0f, false, true));
				}
			}
		}
	}
}
