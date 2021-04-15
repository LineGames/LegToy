////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseTypesetController.h (LegToy - LegToy)
//
// Typesetting Showcase UI Controller
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_TYPESET_CONTROLLER_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_TYPESET_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include <LUI/Common/UICommonTypes.h>
#include <LUI/Element/UIElementSimpleVerticalScrollPanel.h>

// LegToy - Showcase Scene
#include "ShowcaseControllerBase.h"


// Forward Declarations
namespace Leggiero
{
	namespace FileSystem
	{
		class BundleFileResourceComponent;
	}

	namespace Font
	{
		class GlyphManagerComponent;
	}

	namespace LUI
	{
		class UIObject;
		class UIManager;
		class OffsetTransform;
		class UIFabricatedComponent;

		namespace Description
		{
			class DescriptionManager;
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


		// Typesetting Showcase Controller
		class ShowcaseTypesetController
			: public ShowcaseControllerBase
		{
		public:
			ShowcaseTypesetController(ShowcaseLogicCenter &logicCenter);
			virtual ~ShowcaseTypesetController();

		public:
			void InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
				Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem, Leggiero::Font::GlyphManagerComponent *glyphManager);

		protected:	// ShowcaseControllerBase
			virtual void _ResetCaseStates() override;
			virtual void _FinalizeCaseController() override;

		protected:
			std::shared_ptr<Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel> m_scrollPanel;
			std::shared_ptr<Leggiero::LUI::UIObject> m_scrollMarker;

			bool m_isScrollEnabled;
			std::shared_ptr<Leggiero::LUI::OffsetTransform> m_scrollMarkerOffset;
			Leggiero::LUI::UICoordinateType m_markerPositionMultiplier;

		protected:
			std::shared_ptr<Leggiero::LUI::UIObject> _CreateArticle(Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem, Leggiero::Font::GlyphManagerComponent *glyphManager, 
				std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager);

			void _InitializeScrollIndicator(std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent, Leggiero::LUI::Description::DescriptionManager &descriptionManager);

			void _OnScrollPanelPositionChanged(std::shared_ptr<Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel> sender, Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel::PositionValueType position);
		};
	}
}

#endif
