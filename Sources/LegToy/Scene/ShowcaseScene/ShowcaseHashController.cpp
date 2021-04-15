////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseHashController.cpp (LegToy - LegToy)
//
// Hash Test Showcase Controller Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseHashController.h"

// Standard Library
#include <functional>

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>

// Leggiero.Crypto
#include <Crypto/Hash/MD5Util.h>
#include <Crypto/Hash/SHAUtil.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Common/UITransform.h>
#include <LUI/Component/UILayoutComponent.h>
#include <LUI/Component/UISizeSubComponent.h>
#include <LUI/Component/UITransformComponent.h>
#include <LUI/Element/UIElementSingleSimpleButton.h>
#include <LUI/Element/UIElementTextInputBox.h>
#include <LUI/Element/UIElementVariantText.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Description/UIDescriptionUnit.h>
#include <LUI/Prefab/UIFabricatedComponent.h>
#include <LUI/Controller/CommonButtonRenderingController.h>
#include <LUI/Controller/TextInputBoxRenderingController.h>


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// Hash Test Showcase Internal

		namespace _HashTestInternal
		{
			// Mein Herr... Is the cat in the box alive? or dead?
			static const std::string kInitialExampleString("unknown");
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcaseHashController

		//------------------------------------------------------------------------------
		ShowcaseHashController::ShowcaseHashController(ShowcaseLogicCenter &logicCenter)
			: ShowcaseControllerBase(logicCenter)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseHashController::~ShowcaseHashController()
		{
		}

		//------------------------------------------------------------------------------
		void ShowcaseHashController::InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager)
		{
			m_uiMainObject = descriptionManager.FabricateUIObjectByPath("Showcase/HashTest", "ShowcaseUIPrefab", uiManager);
			uiPage->AddPostUIObject(m_uiMainObject);

			std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent(m_uiMainObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));

			m_textInputBox = std::dynamic_pointer_cast<Leggiero::LUI::Element::UIElementTextInputBox>(showcaseUIFabricatedComponent->FindObject("txtInput"));
			m_textInputBox->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::SimpleFabricatedTextInputBoxRenderingController>(m_textInputBox));
			m_textInputBox->SetOnTextInputHandler(std::bind(&ShowcaseHashController::_OnTextInput, this, std::placeholders::_1, std::placeholders::_2));
			m_textInputBox->SetString(_HashTestInternal::kInitialExampleString);
			m_textInputBox->GraphicPrepare();

			m_textMD5 = std::dynamic_pointer_cast<Leggiero::LUI::Element::UIElementVariantText>(showcaseUIFabricatedComponent->FindObject("txtMD5"));
			m_textMD5->SetString(Leggiero::Crypto::Hash::MD5Hash::GetHashString(_HashTestInternal::kInitialExampleString));
			m_textMD5->GraphicPrepare();

			m_textSHA1 = std::dynamic_pointer_cast<Leggiero::LUI::Element::UIElementVariantText>(showcaseUIFabricatedComponent->FindObject("txtSHA1"));
			m_textSHA1->SetString(Leggiero::Crypto::Hash::SHA1Hash::GetHashString(_HashTestInternal::kInitialExampleString));
			m_textSHA1->GraphicPrepare();

			_InitializeMainUI(m_uiMainObject, descriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcaseHashController::_FinalizeCaseController()
		{
			m_textInputBox->DiscardOnTextInputHandler();

			m_textMD5.reset();

			m_textInputBox.reset();
		}

		//------------------------------------------------------------------------------
		void ShowcaseHashController::_OnTextInput(std::shared_ptr<Leggiero::LUI::Element::UIElementTextInputBox> sender, Leggiero::GameDataString textString)
		{
			m_textMD5->SetString(Leggiero::Crypto::Hash::MD5Hash::GetHashString(textString));
			m_textSHA1->SetString(Leggiero::Crypto::Hash::SHA1Hash::GetHashString(textString));
		}
	}
}
