////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseLoadingTasks.cpp (LegToy - LegToy)
//
// Showcase Loader Tasks Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseLoadingTasks.h"

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IAppInformationProvider.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Font
#include <Font/GlyphManagerComponent.h>

// Leggiero.Sound
#include <Sound/SoundMixerComponent.h>
#include <Sound/BGMPlayerComponent.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Prefab/UIFabricatedComponent.h>
#include <LUI/Component/UILayoutComponent.h>
#include <LUI/Component/UISizeSubComponent.h>

// LegToy
#include "../../Game.h"
#include "../../Common/SharedResources.h"
#include "../../Common/IResource_CommonRendering.h"
#include "../../Common/IResource_UI.h"
#include "../../Common/BackgroundRenderer.h"

// LegToy - Showcase Scene
#include "ShowcaseScene.h"
#include "ShowcaseLogicCenter.h"
#include "CubeSpace.h"
#include "ShowcaseMainMenuController.h"
#include "ShowcaseUIExampleController.h"
#include "ShowcaseSoundEffectController.h"
#include "ShowcaseBGMController.h"
#include "ShowcaseTypesetController.h"
#include "ShowcaseHashController.h"
#include "ShowcaseWebViewController.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// BaseDataLoadingTask

		//------------------------------------------------------------------------------
		BaseDataLoadingTask::BaseDataLoadingTask(std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder)
			: m_sceneHolder(ownerSceneHolder)
		{

		}

		//------------------------------------------------------------------------------
		BaseDataLoadingTask::~BaseDataLoadingTask()
		{
		}

	
		//------------------------------------------------------------------------------
		// Do Real Task Works
		Leggiero::Task::TaskDoneResult BaseDataLoadingTask::Do()
		{
			ShowcaseScene *sceneObject = nullptr;
			std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder(m_sceneHolder.lock());
			std::unique_ptr<Leggiero::Utility::Object::PointerHolder::IHoldContext> sceneLockObject;
			if (ownerSceneHolder)
			{
				sceneLockObject = ownerSceneHolder->LockTarget();
				if (sceneLockObject)
				{
					sceneObject = sceneLockObject->GetTypedObject<ShowcaseScene>();
				}
			}
			if (sceneObject == nullptr)
			{
				// Scene already finalized
				return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kFinished);
			}

			sceneObject->m_logic->LoadLogicNonUIContents();

			return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kFinished);
		}


		//////////////////////////////////////////////////////////////////////////////// BaseLogicUILoadingTask

		//------------------------------------------------------------------------------
		BaseLogicUILoadingTask::BaseLogicUILoadingTask(std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder, std::shared_ptr<BaseDataLoadingTask> baseDataLoader)
			: Leggiero::Task::DependentTask(baseDataLoader)
			, m_sceneHolder(ownerSceneHolder)
		{
		}

		//------------------------------------------------------------------------------
		BaseLogicUILoadingTask::~BaseLogicUILoadingTask()
		{
		}

		//------------------------------------------------------------------------------
		Leggiero::Task::TaskDoneResult BaseLogicUILoadingTask::_DoWork()
		{
			ShowcaseScene *sceneObject = nullptr;
			std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder(m_sceneHolder.lock());
			std::unique_ptr<Leggiero::Utility::Object::PointerHolder::IHoldContext> sceneLockObject;
			if (ownerSceneHolder)
			{
				sceneLockObject = ownerSceneHolder->LockTarget();
				if (sceneLockObject)
				{
					sceneObject = sceneLockObject->GetTypedObject<ShowcaseScene>();
				}
			}
			if (sceneObject == nullptr)
			{
				// Scene already finalized
				return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kFinished);
			}

			sceneObject->m_cubeSpace = std::make_shared<CubeSpace>(
				sceneObject->m_game->GetComponent<Leggiero::Application::ApplicationComponent>(),
				sceneObject->m_game->GetComponent<Leggiero::Graphics::GraphicResourceManagerComponent>(),
				sceneObject->m_renderingResources->GetCommonShader_ColorSimple(), sceneObject->m_renderingResources->GetCommonShader_TextureSimple()
				);

			sceneObject->m_logic->SetLogicObjects(sceneObject->m_cubeSpace);

			sceneObject->m_logic->InitializePreUIControllers();

			Leggiero::Application::ApplicationComponent *appComponent = sceneObject->m_game->GetComponent<Leggiero::Application::ApplicationComponent>();
			Leggiero::LUI::UICoordinateType uiScreenWidth = static_cast<Leggiero::LUI::UICoordinateType>(appComponent->AppInformation().GetPixelWidth());
			Leggiero::LUI::UICoordinateType uiScreenHeight = static_cast<Leggiero::LUI::UICoordinateType>(appComponent->AppInformation().GetPixelHeight());

			sceneObject->m_sceneUIPage = std::make_shared<Leggiero::LUI::UIObject>(sceneObject->m_uiManager,
				std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIValuedSizeSubComponent>(uiScreenWidth, uiScreenHeight)));

			sceneObject->m_mainMenuController->InitializeMainMenuUI(sceneObject->m_sceneUIPage, sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager);

			return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kFinished);
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcasesLoadingTask

		//------------------------------------------------------------------------------
		ShowcasesLoadingTask::ShowcasesLoadingTask(std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder, std::shared_ptr<BaseLogicUILoadingTask> baseUILoader)
			: Leggiero::Task::DependentTask(baseUILoader)
			, m_sceneHolder(ownerSceneHolder)
			, m_loadingStep(0)
		{
		}

		//------------------------------------------------------------------------------
		ShowcasesLoadingTask::~ShowcasesLoadingTask()
		{
		}

		//------------------------------------------------------------------------------
		Leggiero::Task::TaskDoneResult ShowcasesLoadingTask::_DoWork()
		{
			ShowcaseScene *sceneObject = nullptr;
			std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder(m_sceneHolder.lock());
			std::unique_ptr<Leggiero::Utility::Object::PointerHolder::IHoldContext> sceneLockObject;
			if (ownerSceneHolder)
			{
				sceneLockObject = ownerSceneHolder->LockTarget();
				if (sceneLockObject)
				{
					sceneObject = sceneLockObject->GetTypedObject<ShowcaseScene>();
				}
			}
			if (sceneObject == nullptr)
			{
				// Scene already finalized
				return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kFinished);
			}

			// Load step by step
			switch (m_loadingStep)
			{
				case 0:
					{
						_LoadUIExampleShowcase(sceneObject);
						++m_loadingStep;
						return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kYield);
					}
					break;

				case 1:
					{
						_LoadSoundEffectShowcase(sceneObject);
						++m_loadingStep;
						return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kYield);
					}
					break;

				case 2:
					{
						_LoadBGMPlayerShowcase(sceneObject);
						++m_loadingStep;
						return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kYield);
					}
					break;

				case 3:
					{
						_LoadTypesettingShowcase(sceneObject);
						++m_loadingStep;
						return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kYield);
					}
					break;

				case 4:
					{
						_LoadHashTesterShowcase(sceneObject);
						++m_loadingStep;
						return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kYield);
					}
					break;

				case 5:
					{
						_LoadWebViewShowcase(sceneObject);
						++m_loadingStep;
						return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kYield);
					}
					break;

				default:
					{
						sceneObject->m_logic->InitializeAfterUIControllers();
					}
					break;
			}

			return Leggiero::Task::TaskDoneResult(Leggiero::Task::TaskDoneResult::ResultType::kFinished);
		}

		//------------------------------------------------------------------------------
		void ShowcasesLoadingTask::_LoadUIExampleShowcase(ShowcaseScene *sceneObject)
		{
			std::shared_ptr<ShowcaseUIExampleController> showcaseUIExampleController = std::make_shared<ShowcaseUIExampleController>(*sceneObject->m_logic);
			showcaseUIExampleController->InitializeShowcaseUI(sceneObject->m_sceneUIPage, sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager,
				*sceneObject->m_game->GetComponent<Leggiero::Application::ApplicationComponent>(),
				*sceneObject->m_game->GetComponent<Leggiero::FileSystem::BundleFileResourceComponent>(),
				sceneObject->m_game->GetComponent<Leggiero::Graphics::GraphicResourceManagerComponent>(),
				sceneObject->m_game->GetComponent<Leggiero::Font::GlyphManagerComponent>(),
				sceneObject->m_uiResources);
			sceneObject->m_logic->RegisterShowcaseController(showcaseUIExampleController, "UI Example", sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcasesLoadingTask::_LoadSoundEffectShowcase(ShowcaseScene *sceneObject)
		{
			std::shared_ptr<ShowcaseSoundEffectController> showcaseSoundEffectController = std::make_shared<ShowcaseSoundEffectController>(*sceneObject->m_logic);
			showcaseSoundEffectController->InitializeShowcaseUI(sceneObject->m_sceneUIPage, sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager,
				sceneObject->m_game->GetComponent<Leggiero::Application::ApplicationComponent>(),
				*sceneObject->m_game->GetComponent<Leggiero::FileSystem::BundleFileResourceComponent>(),
				sceneObject->m_game->GetComponent<Leggiero::Sound::SoundMixerComponent>());
			sceneObject->m_logic->RegisterShowcaseController(showcaseSoundEffectController, "Sound Effect", sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager);

		}

		//------------------------------------------------------------------------------
		void ShowcasesLoadingTask::_LoadBGMPlayerShowcase(ShowcaseScene *sceneObject)
		{
			std::shared_ptr<ShowcaseBGMController> showcaseBGMController = std::make_shared<ShowcaseBGMController>(*sceneObject->m_logic);
			showcaseBGMController->InitializeShowcaseUI(sceneObject->m_sceneUIPage, sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager,
				sceneObject->m_game->GetComponent<Leggiero::Sound::BGMPlayerComponent>());
			sceneObject->m_logic->RegisterShowcaseController(showcaseBGMController, "BGM Player", sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcasesLoadingTask::_LoadTypesettingShowcase(ShowcaseScene *sceneObject)
		{
			std::shared_ptr<ShowcaseTypesetController> showcaseTypesetController = std::make_shared<ShowcaseTypesetController>(*sceneObject->m_logic);
			showcaseTypesetController->InitializeShowcaseUI(sceneObject->m_sceneUIPage, sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager,
				*sceneObject->m_game->GetComponent<Leggiero::FileSystem::BundleFileResourceComponent>(),
				sceneObject->m_game->GetComponent<Leggiero::Font::GlyphManagerComponent>());
			sceneObject->m_logic->RegisterShowcaseController(showcaseTypesetController, "Typesetting", sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcasesLoadingTask::_LoadHashTesterShowcase(ShowcaseScene *sceneObject)
		{
			std::shared_ptr<ShowcaseHashController> showcaseHashController = std::make_shared<ShowcaseHashController>(*sceneObject->m_logic);
			showcaseHashController->InitializeShowcaseUI(sceneObject->m_sceneUIPage, sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager);
			sceneObject->m_logic->RegisterShowcaseController(showcaseHashController, "Hash Tester", sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcasesLoadingTask::_LoadWebViewShowcase(ShowcaseScene *sceneObject)
		{
			std::shared_ptr<ShowcaseWebViewController> showcaseWebViewController = std::make_shared<ShowcaseWebViewController>(*sceneObject->m_logic);
			showcaseWebViewController->InitializeShowcaseUI(sceneObject->m_sceneUIPage, sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager,
				sceneObject->m_game->GetComponent<Leggiero::Application::ApplicationComponent>());
			sceneObject->m_logic->RegisterShowcaseController(showcaseWebViewController, "Web View", sceneObject->m_uiManager, *sceneObject->m_uiDescriptionManager);
		}
	}
}
