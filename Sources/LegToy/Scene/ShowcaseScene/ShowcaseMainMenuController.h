////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseMainMenuController.h (LegToy - LegToy)
//
// Main Menu UI Controller
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_MAIN_MENU_CONTROLLER_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_MAIN_MENU_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <unordered_map>

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
			class DescriptionUnit;
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


		// Main Menu Controller
		class ShowcaseMainMenuController
		{
		public:
			ShowcaseMainMenuController(ShowcaseLogicCenter &logicCenter);
			virtual ~ShowcaseMainMenuController();

		public:
			void InitializeMainMenuUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager);
			void FinalizeController();

			void AddButton(int creatingIndex, const Leggiero::GameDataString &buttonCaption, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager);
			void FitButtons();

		public:
			void ResetAllStates();
			void ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval);

			Leggiero::LUI::UICoordinateType ContentsAreaCenterX() const { return m_contentsAreaCenterX; }

			void SetButtonActive(int activeIndex);

		protected:
			ShowcaseLogicCenter &m_logicCenter;

			std::shared_ptr<Leggiero::LUI::Description::DescriptionUnit> m_menuUnit;

			std::shared_ptr<Leggiero::LUI::UIObject> m_menuObject;
			std::shared_ptr<Leggiero::LUI::UIObject> m_menuBGObject;
			std::shared_ptr<Leggiero::LUI::UIObject> m_buttonPanelObject;

			Leggiero::LUI::UICoordinateType m_contentsAreaCenterX;
			Leggiero::LUI::UICoordinateType m_buttonUnitHeight;
			Leggiero::LUI::UICoordinateType m_menuBGHeaderHeight;
			Leggiero::LUI::UICoordinateType m_menuBGPadding;
			Leggiero::LUI::UICoordinateType m_menuBGWidth;

			std::shared_ptr<Leggiero::LUI::OffsetTransform> m_menuOffset;

			std::unordered_map<int, std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> > m_caseButtons;

			int buttonVerticalCount;

			bool m_isMenuOpening;
			float m_menuOpeningDuration;

		protected:
			void _OnCaseButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender, int index);
		};
	}
}

#endif
