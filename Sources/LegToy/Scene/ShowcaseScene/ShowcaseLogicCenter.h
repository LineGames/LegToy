////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseLogicCenter.h (LegToy - LegToy)
//
// Monolithic Showcase Logic Processing Object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_LOGIC_CENTER_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_LOGIC_CENTER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <vector>

// Leggiero.Application
#include <Application/SystemEventObserver.h>


// Forward Declarations
namespace Leggiero
{
	namespace Application
	{
		class SystemEventDispatcher;
	}

	namespace LUI
	{
		class UIManager;

		namespace Description
		{
			class DescriptionManager;
		}
	}
}



namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		// Forward Declarations
		class CubeSpace;
		class ShowcaseMainMenuController;
		class ShowcaseControllerBase;


		// Showcase Logic
		class ShowcaseLogicCenter
			: public Leggiero::Application::ISystemCommandEventObserver
		{
		public:
			ShowcaseLogicCenter(Leggiero::Application::SystemEventDispatcher &systemEventCenter);
			virtual ~ShowcaseLogicCenter();

		public:
			void LoadLogicNonUIContents();

			void SetLogicObjects(std::shared_ptr<CubeSpace> cubeSpace);

			void InitializePreUIControllers();
			void SetUIControllers(std::shared_ptr<ShowcaseMainMenuController> mainMenuController);
			void RegisterShowcaseController(std::shared_ptr<ShowcaseControllerBase> caseController, const Leggiero::GameDataString &caseName, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager);
			
			void InitializeAfterUIControllers();
			
			void FinalizeLogicCenter();

		public:
			void ResetAllStates();
			void ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval);

			void NotifySceneEnter();
			void NotifySceneExit();

			void GoStartSceneLogic();

		public:
			void RequestCaseStartIntent(int caseIndex);
			void OnCaseUIClosed(int caseIndex);

			void RequestMainMenuOpenIntent();
			void RequestMainMenuCloseIntent();

		public:
			// Back-Button
			// return means back button handled by
			virtual bool OnBackButtonPressed() override;

		protected:
			void _ForceCloseCurrentShow();

		protected:
			Leggiero::Application::SystemEventDispatcher &m_systemEventCenter;

			std::shared_ptr<CubeSpace> m_cubeSpace;

			std::shared_ptr<ShowcaseMainMenuController>				m_mainMenuController;
			std::vector<std::shared_ptr<ShowcaseControllerBase> >	m_caseControllers;

			int m_showingShowIndex;
			int m_requestedShowOpen;

			bool m_isCommandObserverRegistered;
		};
	}
}

#endif
