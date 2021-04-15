////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseLogicCenter.cpp (LegToy - LegToy)
//
// Showcase Main Logic Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseLogicCenter.h"

// Leggiero.Utility
#include <Utility/Math/SimpleGeometry.h>

// Leggiero.Application
#include <Application/SystemEventDispatcher.h>

// LegToy - Showcase Scene
#include "CubeSpace.h"
#include "ShowcaseMainMenuController.h"
#include "ShowcaseControllerBase.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// Logic Center Internal Utility

		namespace _ShowcaseLogicInternal
		{
			constexpr float kCubeStartAppearTimeConstant = 3.2f;
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcaseLogicCenter

		//------------------------------------------------------------------------------
		ShowcaseLogicCenter::ShowcaseLogicCenter(Leggiero::Application::SystemEventDispatcher &systemEventCenter)
			: m_systemEventCenter(systemEventCenter)
			, m_showingShowIndex(0), m_requestedShowOpen(0)
			, m_isCommandObserverRegistered(false)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseLogicCenter::~ShowcaseLogicCenter()
		{

		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::LoadLogicNonUIContents()
		{

		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::SetLogicObjects(std::shared_ptr<CubeSpace> cubeSpace)
		{
			m_cubeSpace = cubeSpace;
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::InitializePreUIControllers()
		{

		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::SetUIControllers(std::shared_ptr<ShowcaseMainMenuController> mainMenuController)
		{
			m_mainMenuController = mainMenuController;
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::RegisterShowcaseController(std::shared_ptr<ShowcaseControllerBase> caseController, const Leggiero::GameDataString &caseName, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager)
		{
			int caseIndex = static_cast<int>(m_caseControllers.size()) + 1;
			caseController->SetCaseIndex(caseIndex);

			m_caseControllers.push_back(caseController);

			m_mainMenuController->AddButton(caseIndex, caseName, uiManager, descriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::InitializeAfterUIControllers()
		{
			m_mainMenuController->FitButtons();
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::FinalizeLogicCenter()
		{
			if (m_isCommandObserverRegistered)
			{
				m_systemEventCenter.UnRegisterCommandEventObserver(this);
				m_isCommandObserverRegistered = false;
			}

			for (std::shared_ptr<ShowcaseControllerBase> &controller : m_caseControllers)
			{
				controller->FinalizeController();
			}
			m_caseControllers.clear();

			m_mainMenuController.reset();

			m_cubeSpace.reset();
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::ResetAllStates()
		{
			m_mainMenuController->ResetAllStates();
			for (std::shared_ptr<ShowcaseControllerBase> &controller : m_caseControllers)
			{
				controller->ResetAllStates();
			}

			m_cubeSpace->ForceSetSeeAzimuth(Leggiero::Utility::Mathf::kPiOver2);

			m_showingShowIndex = 0;
			m_requestedShowOpen = 0;
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval)
		{
			if (m_requestedShowOpen != 0)
			{
				if (m_showingShowIndex != m_requestedShowOpen)
				{
					_ForceCloseCurrentShow();
					if (m_requestedShowOpen > 0 && m_requestedShowOpen <= static_cast<int>(m_caseControllers.size()))
					{
						m_caseControllers[m_requestedShowOpen - 1]->StartOpen();
						m_mainMenuController->SetButtonActive(m_requestedShowOpen);
						m_showingShowIndex = m_requestedShowOpen;
					}
				}
			}
			m_requestedShowOpen = 0;

			for (std::shared_ptr<ShowcaseControllerBase> &controller : m_caseControllers)
			{
				controller->ProcessFrameLogic(frameReferenceTime, frameInterval);
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::NotifySceneEnter()
		{
			if (m_isCommandObserverRegistered)
			{
				return;
			}
			m_systemEventCenter.RegisterCommandEventObserver(this);
			m_isCommandObserverRegistered = true;
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::NotifySceneExit()
		{
			m_systemEventCenter.UnRegisterCommandEventObserver(this);
			m_isCommandObserverRegistered = false;
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::GoStartSceneLogic()
		{
			m_cubeSpace->StartToSeeAzimuth(m_cubeSpace->CalculateSeeAzimuth(m_mainMenuController->ContentsAreaCenterX()), _ShowcaseLogicInternal::kCubeStartAppearTimeConstant);
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::RequestCaseStartIntent(int caseIndex)
		{
			m_requestedShowOpen = caseIndex;
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::OnCaseUIClosed(int caseIndex)
		{
			if (m_showingShowIndex == caseIndex)
			{
				m_mainMenuController->SetButtonActive(0);
				m_showingShowIndex = 0;
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::RequestMainMenuOpenIntent()
		{

		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::RequestMainMenuCloseIntent()
		{

		}

		//------------------------------------------------------------------------------
		void ShowcaseLogicCenter::_ForceCloseCurrentShow()
		{
			if (m_showingShowIndex > 0 && m_showingShowIndex <= static_cast<int>(m_caseControllers.size()))
			{
				m_caseControllers[m_showingShowIndex - 1]->ForceClose();
			}
			m_mainMenuController->SetButtonActive(0);
			m_showingShowIndex = 0;
		}

		//------------------------------------------------------------------------------
		// Back-Button
		// return means back button handled by
		bool ShowcaseLogicCenter::OnBackButtonPressed()
		{
			bool isHandled = false;
			if (m_showingShowIndex > 0 && m_showingShowIndex <= static_cast<int>(m_caseControllers.size()))
			{
				ShowcaseControllerBase &handlingController = *m_caseControllers[m_showingShowIndex - 1];
				if (!handlingController.IsClosing())
				{
					isHandled |= m_caseControllers[m_showingShowIndex - 1]->ProcessBackRequest();
				}
			}

			return isHandled;
		}
	}
}
