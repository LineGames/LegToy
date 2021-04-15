////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseScene.h (LegToy - LegToy)
//
// Game Test Scene
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_SCENE_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_SCENE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include <LUI/Scene/UISceneBase.h>

// LegToy
#include "../SceneIds.h"


// Forward Declarations
namespace Leggiero
{
	namespace Utility
	{
		namespace Object
		{
			class PointerHolder;
		}
	}

	namespace Task
	{
		class ITask;
	}

	namespace LUI
	{
		class UIObject;

		namespace Description
		{
			class DescriptionManager;
		}
	}
}


namespace LegToy
{
	// Forward Declarations
	class SharedResources;

	namespace SharedResource
	{
		class ICommonRenderingResources;
		class IUIResources;
	}


	namespace ShowcaseSceneStuffs
	{
		// Forward Declarations
		class CubeSpace;
		class ShowcaseLogicCenter;
		class ShowcaseMainMenuController;
		class BaseDataLoadingTask;
		class BaseLogicUILoadingTask;
		class ShowcasesLoadingTask;


		// Game Engine Showcase Scene
		class ShowcaseScene
			: public Leggiero::LUI::UISceneBase
		{
		public:	// Loaders have to access scene contents
			friend class BaseDataLoadingTask;
			friend class BaseLogicUILoadingTask;
			friend class ShowcasesLoadingTask;

		public:
			ShowcaseScene(Leggiero::Toolbox::Scene::IGameSceneContext *ownerContext, Leggiero::IGame *game, SharedResources *sharedResources);
			virtual ~ShowcaseScene();

		public:	// IGameScene
			// Get Id of the Scene
			virtual Leggiero::Toolbox::Scene::GameSceneIdType GetSceneId() const override { return SceneIds::kShowcase; }

		protected:	// UISceneBase
			virtual void _InitializeUIManager() override;

			virtual void _FinalizeUIContents() override;

			virtual void _SetUIBeforeEnterFrame() override;
			virtual void _UnSetUIAfterExitFrame() override;

			virtual void _ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval) override;

			virtual void _RenderPreUIContents() override;

		public:
			void StartLoading();
			bool IsLoadingFinished();

		protected:
			SharedResources *m_sharedResources;

			SharedResource::ICommonRenderingResources *m_renderingResources;
			SharedResource::IUIResources *m_uiResources;

			Leggiero::LUI::Description::DescriptionManager *m_uiDescriptionManager;

			Leggiero::GameTimeClockType::time_point m_loadingStartTime;

			std::shared_ptr<ShowcaseLogicCenter>	m_logic;
			std::shared_ptr<CubeSpace>				m_cubeSpace;

			std::shared_ptr<ShowcaseMainMenuController> m_mainMenuController;

			std::shared_ptr<Leggiero::LUI::UIObject> m_sceneUIPage;

			std::shared_ptr<Leggiero::Task::ITask> m_lastTask;
			bool m_isLoadingFinishChecked;

			std::shared_ptr<Leggiero::Utility::Object::PointerHolder> m_myPointerHolder;
		};
	}
}

#endif
