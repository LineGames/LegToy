////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseWebViewController.cpp (LegToy - LegToy)
//
// Web View Showcase Controller Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseWebViewController.h"

// Standard Library
#include <functional>

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IAppWebViewController.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Component/UILayoutComponent.h>
#include <LUI/Component/UISizeSubComponent.h>
#include <LUI/Element/UIElementSingleSimpleButton.h>
#include <LUI/Element/UIElementTextInputBox.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Prefab/UIFabricatedComponent.h>
#include <LUI/Controller/CommonButtonRenderingController.h>
#include <LUI/Controller/TextInputBoxRenderingController.h>


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// Web View Showcase Internal

		namespace _WebViewInternal
		{
			static const std::string kInitialExampleString("https://www.line.games/");
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcaseHashController

		//------------------------------------------------------------------------------
		ShowcaseWebViewController::ShowcaseWebViewController(ShowcaseLogicCenter &logicCenter)
			: ShowcaseControllerBase(logicCenter)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseWebViewController::~ShowcaseWebViewController()
		{
		}

		//------------------------------------------------------------------------------
		void ShowcaseWebViewController::InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
			Leggiero::Application::ApplicationComponent *appComponent)
		{
			m_appComponent = appComponent;

			m_uiMainObject = descriptionManager.FabricateUIObjectByPath("Showcase/WebView", "ShowcaseUIPrefab", uiManager);
			uiPage->AddPostUIObject(m_uiMainObject);

			std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent(m_uiMainObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));

			m_textInputBox = std::dynamic_pointer_cast<Leggiero::LUI::Element::UIElementTextInputBox>(showcaseUIFabricatedComponent->FindObject("txtInput"));
			m_textInputBox->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::SimpleFabricatedTextInputBoxRenderingController>(m_textInputBox));
			m_textInputBox->SetString(_WebViewInternal::kInitialExampleString);
			m_textInputBox->GraphicPrepare();

			m_goButton = std::dynamic_pointer_cast<Leggiero::LUI::Element::SingleSimpleButton>(showcaseUIFabricatedComponent->FindObject("btnGo"));
			m_goButton->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::SimpleButtonRenderingController>(*m_goButton));
			m_goButton->SetOnClickHandler(std::bind(&ShowcaseWebViewController::_OnGoButtonClicked, this, std::placeholders::_1));

			_InitializeMainUI(m_uiMainObject, descriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcaseWebViewController::_FinalizeCaseController()
		{
			m_goButton->DiscardOnClickHandler();
			m_goButton.reset();

			m_textInputBox.reset();
		}

		//------------------------------------------------------------------------------
		bool ShowcaseWebViewController::ProcessBackRequest()
		{
			if (m_appComponent->WebViewController().IsURLPageOpened())
			{
				return true;
			}
			return ShowcaseControllerBase::ProcessBackRequest();
		}

		//------------------------------------------------------------------------------
		void ShowcaseWebViewController::_OnGoButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender)
		{
			m_appComponent->WebViewController().OpenURLByPage("Web View Test", "Close", m_textInputBox->GetCurrentValue().c_str());
		}
	}
}
