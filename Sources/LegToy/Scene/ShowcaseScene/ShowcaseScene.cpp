////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseScene.cpp (LegToy - LegToy)
//
// Game Test Scene Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseScene.h"

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>

// Leggiero.Task
#include <Task/TaskManagerComponent.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>

// LegToy
#include "../../Game.h"
#include "../../Common/SharedResources.h"
#include "../../Common/IResource_CommonRendering.h"
#include "../../Common/IResource_UI.h"
#include "../../Common/BackgroundRenderer.h"

// LegToy - Showcase Scene
#include "ShowcaseLogicCenter.h"
#include "CubeSpace.h"
#include "ShowcaseMainMenuController.h"
#include "ShowcaseLoadingTasks.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// ShowcaseScene

		//------------------------------------------------------------------------------
		ShowcaseScene::ShowcaseScene(Leggiero::Toolbox::Scene::IGameSceneContext *ownerContext, Leggiero::IGame *game, SharedResources *sharedResources)
			: Leggiero::LUI::UISceneBase(ownerContext, game)
			, m_sharedResources(sharedResources)
			, m_myPointerHolder(std::make_shared<Leggiero::Utility::Object::PointerHolder>(this))
			, m_isLoadingFinishChecked(false)
		{
			m_renderingResources = dynamic_cast<SharedResource::ICommonRenderingResources *>(sharedResources);
			m_uiResources = dynamic_cast<SharedResource::IUIResources *>(sharedResources);

			m_logic = std::make_shared<ShowcaseLogicCenter>(game->GetComponent<Leggiero::Application::ApplicationComponent>()->SystemEventCenter());
			m_mainMenuController = std::make_shared<ShowcaseMainMenuController>(*m_logic);

			m_logic->SetUIControllers(m_mainMenuController);
		}

		//------------------------------------------------------------------------------
		ShowcaseScene::~ShowcaseScene()
		{
		}

		//------------------------------------------------------------------------------
		void ShowcaseScene::_InitializeUIManager()
		{
			m_uiManager = m_uiResources->GetSharedUIManager();

			auto tempDescriptionHolder = m_uiResources->GetBaseUIDescriptionManager();
			m_uiDescriptionManager = tempDescriptionHolder.get();
		}

		//------------------------------------------------------------------------------
		// Called in Graphic Thread
		void ShowcaseScene::_FinalizeUIContents()
		{
			m_myPointerHolder->NotifyTargetDeleted();
			m_lastTask.reset();

			if (m_uiManager)
			{
				m_uiManager->RemoveTopUIObject(m_sceneUIPage);
			}
			m_sceneUIPage.reset();

			m_mainMenuController->FinalizeController();
			m_logic->FinalizeLogicCenter();

			m_mainMenuController.reset();

			m_cubeSpace.reset();

			m_logic.reset();
		}

		//------------------------------------------------------------------------------
		// Called in Graphic Thread
		void ShowcaseScene::_SetUIBeforeEnterFrame()
		{
			// Not actually UIs, but this is a calling point
			m_logic->ResetAllStates();

			m_uiManager->AddTopUIObject(m_sceneUIPage);

			m_logic->NotifySceneEnter();
			m_logic->GoStartSceneLogic();
		}

		//------------------------------------------------------------------------------
		// Called in Graphic Thread
		void ShowcaseScene::_UnSetUIAfterExitFrame()
		{
			if (m_logic)
			{
				m_logic->NotifySceneExit();
			}
			m_uiManager->RemoveTopUIObject(m_sceneUIPage);
		}

		//------------------------------------------------------------------------------
		// Frame Logic Process Function
		void ShowcaseScene::_ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval)
		{
			m_renderingResources->GetCommonBackgroundRenderer()->UpdateFrame(frameInterval);

			m_logic->ProcessFrameLogic(frameReferenceTime, frameInterval);
			m_mainMenuController->ProcessFrameLogic(frameReferenceTime, frameInterval);

			m_cubeSpace->UpdateFrame(frameInterval);
		}

		//------------------------------------------------------------------------------
		void ShowcaseScene::_RenderPreUIContents()
		{
			m_renderingResources->GetCommonBackgroundRenderer()->Render();

			m_cubeSpace->Render();
		}

		//------------------------------------------------------------------------------
		void ShowcaseScene::StartLoading()
		{
			m_loadingStartTime = Leggiero::GameTimeClockType::now();

			Leggiero::Task::TaskManagerComponent *taskManager = m_game->GetComponent<Leggiero::Task::TaskManagerComponent>();

			std::shared_ptr<BaseDataLoadingTask> baseLoadingTask(std::make_shared<BaseDataLoadingTask>(m_myPointerHolder));
			taskManager->ExecuteTask(baseLoadingTask);

			std::shared_ptr<BaseLogicUILoadingTask> uiLoadingTask(std::make_shared<BaseLogicUILoadingTask>(m_myPointerHolder, baseLoadingTask));
			taskManager->ExecuteTask(uiLoadingTask);

			std::shared_ptr<ShowcasesLoadingTask> showcaseLoadingTask(std::make_shared<ShowcasesLoadingTask>(m_myPointerHolder, uiLoadingTask));
			taskManager->ExecuteTask(showcaseLoadingTask);

			m_lastTask = showcaseLoadingTask;
		}

		//------------------------------------------------------------------------------
		bool ShowcaseScene::IsLoadingFinished()
		{
			if (m_isLoadingFinishChecked)
			{
				return true;
			}
			if (m_lastTask && m_lastTask->IsFinished())
			{
				m_isLoadingFinishChecked = true;
				return true;
			}
			return false;
		}
	}
}
