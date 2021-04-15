////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseUIExampleController.h (LegToy - LegToy)
//
// UI Example Showcase UI Controller
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_UI_EXAMPLE_CONTROLLER_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__SHOWCASE_UI_EXAMPLE_CONTROLLER_H


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

	namespace Graphics
	{
		class GraphicResourceManagerComponent;
	}

	namespace FileSystem
	{
		class BundleFileResourceComponent;
	}

	namespace Font
	{
		class GlyphManagerComponent;
		class IFontSet;
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
			class UIElementTestButton;
			class UIElementBlurredBarrier;
			class CancelPanelArea;
		}
	}
}


namespace LegToy
{
	// Forward Declarations
	namespace SharedResource
	{
		class IUIResources;
	}


	namespace ShowcaseSceneStuffs
	{
		// Forward Declarations
		class ShowcaseLogicCenter;


		// UI Example Showcase Controller
		class ShowcaseUIExampleController
			: public ShowcaseControllerBase
		{
		public:
			ShowcaseUIExampleController(ShowcaseLogicCenter &logicCenter);
			virtual ~ShowcaseUIExampleController();

		public:
			void InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
				Leggiero::Application::ApplicationComponent &appComponent, Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem, Leggiero::Graphics::GraphicResourceManagerComponent *graphicResourceManager, Leggiero::Font::GlyphManagerComponent *glyphManager, 
				SharedResource::IUIResources *uiSharedResources);

		public:	// ShowcaseControllerBase
			virtual bool ProcessBackRequest() override;

		protected:	// ShowcaseControllerBase
			virtual void _FinalizeCaseController() override;
			virtual void _PrepareCloseStart() override;

		protected:
			void _LoadFont(Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem);
			void _ReadSettingsFromDescription(Leggiero::LUI::Description::DescriptionManager &descriptionManager);
			std::shared_ptr<Leggiero::LUI::UIObject> _CreateUIByCode(std::shared_ptr<Leggiero::LUI::UIManager> uiManager, 
				Leggiero::Application::ApplicationComponent &appComponent, Leggiero::Graphics::GraphicResourceManagerComponent *graphicResourceManager, Leggiero::Font::GlyphManagerComponent *glyphManager, 
				SharedResource::IUIResources *uiSharedResources);

		protected:
			std::shared_ptr<Leggiero::Font::IFontSet> m_testFontSet;

			Leggiero::LUI::UICoordinateType m_contentsWidth;
			Leggiero::LUI::UICoordinateType m_contentsHeight;

			std::shared_ptr<Leggiero::LUI::Element::UIElementTestButton> m_testButton;

			std::shared_ptr<Leggiero::LUI::Element::UIElementBlurredBarrier> m_testPopup;
			std::shared_ptr<Leggiero::LUI::Element::CancelPanelArea> m_testPopupCancel;

		protected:
			void _OnTestButtonClicked(std::shared_ptr<Leggiero::LUI::Element::UIElementTestButton> sender);
			void _OnPopupCancel(std::shared_ptr<Leggiero::LUI::Element::CancelPanelArea> sender);
		};
	}
}

#endif
