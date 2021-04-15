////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseControllerBase.h (LegToy - LegToy)
//
// Base Class of Showcase UI Controllers
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_CONTROLLER_BASE_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_CONTROLLER_BASE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include <LUI/Common/UICommonTypes.h>


// Forward Declarations
namespace Leggiero
{
	namespace LUI
	{
		class UIObject;
		class UIManager;
		class OffsetTransform;

		namespace Description
		{
			class DescriptionManager;
		}

		namespace Element
		{
			class SingleSimpleButton;
			class UIElementTextInputBox;
			class UIElementVariantText;
		}
	}
}


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		// Forward Declarations
		class ShowcaseLogicCenter;


		// Base Showcase Controller
		class ShowcaseControllerBase
		{
		public:
			ShowcaseControllerBase(ShowcaseLogicCenter &logicCenter);
			virtual ~ShowcaseControllerBase();

		public:
			void FinalizeController();

		public:
			void ResetAllStates();
			void ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval);

			void StartOpen();
			void StartClose();
			void ForceClose();

			void SetCaseIndex(int index) { m_caseIndex = index; }
			int GetCaseIndex() const { return m_caseIndex; }

			virtual bool IsOpening() { return m_state == 1; }
			virtual bool IsClosing() { return m_state == 2; }

			virtual bool ProcessBackRequest();

		protected:
			void _InitializeMainUI(std::shared_ptr<Leggiero::LUI::UIObject> showcaseMainUIObject, Leggiero::LUI::Description::DescriptionManager &descriptionManager);

		protected:
			virtual void _ResetCaseStates() { }
			virtual void _FinalizeCaseController() { }

			virtual void _ProcessCaseFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval) { }

			virtual void _PrepareCloseStart() { }

		protected:
			ShowcaseLogicCenter &m_logicCenter;

			std::shared_ptr<Leggiero::LUI::UIObject> m_uiMainObject;
			std::shared_ptr<Leggiero::LUI::OffsetTransform> m_mainUIOffset;

			std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> m_closeButton;

			int m_state;	// 0 for normal, 1 for opening, 2 for closing
			float m_inStateTime;

			Leggiero::LUI::UICoordinateType m_animOpeningMoving;
			Leggiero::LUI::UICoordinateType m_animClosingMoving;

			int m_caseIndex;

		protected:
			void _OnCloseButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender);
		};
	}
}

#endif
