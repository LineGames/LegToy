////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseLoadingTasks.h (LegToy - LegToy)
//
// Showcase Scene Loading Async Tasks
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_LOADING_TASKS_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_LOADING_TASKS_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Task
#include <Task/Tasks/ITask.h>
#include <Task/Tasks/DependentTask.h>

// LegToy - Showcase Scene
//#include "ShowcaseControllerBase.h"


// Forward Declaration
namespace Leggiero
{
	namespace Utility
	{
		namespace Object
		{
			class PointerHolder;
		}
	}
}


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		// Forward Declarations
		class ShowcaseScene;


		// Base Non-Graphic Data Loading Task
		class BaseDataLoadingTask
			: public Leggiero::Task::ITask
		{
		public:
			BaseDataLoadingTask(std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder);
			virtual ~BaseDataLoadingTask();

		public:	// ITask
			// Do Real Task Works
			virtual Leggiero::Task::TaskDoneResult Do() override;

		protected:
			std::weak_ptr<Leggiero::Utility::Object::PointerHolder> m_sceneHolder;
		};


		// Showcase Scene Base Logic UI Loading Task
		class BaseLogicUILoadingTask
			: public Leggiero::Task::DependentTask
		{
		public:
			BaseLogicUILoadingTask(std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder, std::shared_ptr<BaseDataLoadingTask> baseDataLoader);
			virtual ~BaseLogicUILoadingTask();

		public:	// ITask
			virtual Leggiero::Task::TaskCapabilityType	GetRequiredCapabilties() override { return Leggiero::Task::TaskCapabilities::kGraphics; }

		public:	// DependentTask
			virtual Leggiero::Task::TaskDoneResult _DoWork() override;

		protected:
			std::weak_ptr<Leggiero::Utility::Object::PointerHolder> m_sceneHolder;
		};


		// Showcase Scene Showcases Loading Task
		class ShowcasesLoadingTask
			: public Leggiero::Task::DependentTask
		{
		public:
			ShowcasesLoadingTask(std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerSceneHolder, std::shared_ptr<BaseLogicUILoadingTask> baseUILoader);
			virtual ~ShowcasesLoadingTask();

		public:	// ITask
			virtual Leggiero::Task::TaskCapabilityType	GetRequiredCapabilties() override { return Leggiero::Task::TaskCapabilities::kGraphics; }

		public:	// DependentTask
			virtual Leggiero::Task::TaskDoneResult _DoWork() override;

		protected:
			void _LoadUIExampleShowcase(ShowcaseScene *sceneObject);
			void _LoadSoundEffectShowcase(ShowcaseScene *sceneObject);
			void _LoadBGMPlayerShowcase(ShowcaseScene *sceneObject);
			void _LoadTypesettingShowcase(ShowcaseScene *sceneObject);
			void _LoadHashTesterShowcase(ShowcaseScene *sceneObject);
			void _LoadWebViewShowcase(ShowcaseScene *sceneObject);

		protected:
			std::weak_ptr<Leggiero::Utility::Object::PointerHolder> m_sceneHolder;

			int m_loadingStep;
		};
	}
}

#endif
