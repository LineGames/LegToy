////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseHashController.h (LegToy - LegToy)
//
// Hash Test Showcase UI Controller
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_HASH_CONTROLLER_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_HASH_CONTROLLER_H


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


		// Hash Test Showcase Controller
		class ShowcaseHashController
			: public ShowcaseControllerBase
		{
		public:
			ShowcaseHashController(ShowcaseLogicCenter &logicCenter);
			virtual ~ShowcaseHashController();

		public:
			void InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager);

		protected:	// ShowcaseControllerBase
			virtual void _FinalizeCaseController() override;

		protected:
			std::shared_ptr<Leggiero::LUI::Element::UIElementTextInputBox> m_textInputBox;

			std::shared_ptr<Leggiero::LUI::Element::UIElementVariantText> m_textMD5;
			std::shared_ptr<Leggiero::LUI::Element::UIElementVariantText> m_textSHA1;

		protected:
			void _OnTextInput(std::shared_ptr<Leggiero::LUI::Element::UIElementTextInputBox> sender, Leggiero::GameDataString textString);
		};
	}
}

#endif
