////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseSoundEffectController.h (LegToy - LegToy)
//
// Sound Effect Showcase UI Controller
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_SOUND_EFFECT_CONTROLLER_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_SOUND_EFFECT_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <vector>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.LegacyUI
#include <LUI/Common/UICommonTypes.h>

// LegToy - Showcase Scene
#include "ShowcaseControllerBase.h"


// Forward Declarations
namespace Leggiero
{
	namespace Application
	{
		class ApplicationComponent;
	}

	namespace FileSystem
	{
		class BundleFileResourceComponent;
	}

	namespace Sound
	{
		class SoundMixerComponent;
		class ISoundProvider;
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
		}
	}
}


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		// Forward Declarations
		class ShowcaseLogicCenter;

		namespace _SoundEffectInternal
		{
			class PlayButtonController;
			class LongPlayButtonController;
		}


		// Sound Effect Showcase Controller
		class ShowcaseSoundEffectController
			: public ShowcaseControllerBase
			, public Leggiero::Application::IApplicationEventObserver
		{
		public:
			ShowcaseSoundEffectController(ShowcaseLogicCenter &logicCenter);
			virtual ~ShowcaseSoundEffectController();

		public:
			void InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
				Leggiero::Application::ApplicationComponent *appComponent, Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem, Leggiero::Sound::SoundMixerComponent *soundMixer);

		public:	// IApplicationEventObserver
			virtual void OnPause() override;
			virtual void OnResume() override;

		protected:	// ShowcaseControllerBase
			virtual void _ResetCaseStates() override;
			virtual void _FinalizeCaseController() override;

			virtual void _ProcessCaseFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval) override;

		protected:
			void _InitializeSoundSources(Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem);
			void _CheckAndGiveSoundHandles();

		protected:
			Leggiero::Application::ApplicationComponent *m_appComponent;
			Leggiero::Sound::SoundMixerComponent		*m_soundMixer;

			std::vector<std::shared_ptr<_SoundEffectInternal::PlayButtonController> >	m_playButtonControllers;
			std::shared_ptr<_SoundEffectInternal::LongPlayButtonController>				m_longPlayButtonController;

			std::vector<std::shared_ptr<Leggiero::Sound::ISoundProvider> >	m_playButtonSoundSources;
			std::vector<std::shared_ptr<Leggiero::Sound::ISoundProvider> >	m_loopPlayButtonSoundSources;
		};
	}
}

#endif
