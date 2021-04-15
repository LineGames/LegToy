////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseWebViewController.h (LegToy - LegToy)
//
// Web View Showcase UI Controller
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_WEB_VIEW_CONTROLLER_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_WEB_VIEW_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

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
			class UIElementTextInputBox;
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


		// Web View Showcase Controller
		class ShowcaseWebViewController
			: public ShowcaseControllerBase
		{
		public:
			ShowcaseWebViewController(ShowcaseLogicCenter &logicCenter);
			virtual ~ShowcaseWebViewController();

		public:
			void InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
				Leggiero::Application::ApplicationComponent *appComponent);

		public:	// ShowcaseControllerBase
			virtual bool ProcessBackRequest() override;

		protected:	// ShowcaseControllerBase
			virtual void _FinalizeCaseController() override;

		protected:
			Leggiero::Application::ApplicationComponent *m_appComponent;

			std::shared_ptr<Leggiero::LUI::Element::UIElementTextInputBox>	m_textInputBox;
			std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton>		m_goButton;

		protected:
			void _OnGoButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender);
		};
	}
}

#endif
