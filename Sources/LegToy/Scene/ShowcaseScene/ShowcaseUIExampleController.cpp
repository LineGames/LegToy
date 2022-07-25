////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseUIExampleController.cpp (LegToy - LegToy)
//
// UI Example Showcase Controller Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseUIExampleController.h"

// Standard Library
#include <functional>
#include <sstream>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IAppInformationProvider.h>
#include <Application/IDeviceInformationProvider.h>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Font
#include <Font/Common/Typeset.h>
#include <Font/Common/SingleFontSet.h>
#include <Font/Common/StyledFontSet.h>
#include <Font/FreeType/FreeTypeFontFace.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Common/UICommonArea.h>
#include <LUI/Component/UILayoutComponent.h>
#include <LUI/Component/UISizeSubComponent.h>
#include <LUI/Touch/ValuedTouchComponent.h>
#include <LUI/Element/UIElementTestButton.h>
#include <LUI/Element/UIElementFixedText.h>
#include <LUI/Element/UIElementBlurredBarrier.h>
#include <LUI/Element/UIElementCancelPanelArea.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Prefab/UIFabricatedComponent.h>
#include <LUI/Controller/CommonButtonRenderingController.h>
#include <LUI/Rendering/UIRenderer.h>
#include <LUI/Rendering/ShapeSetRenderingComponent.h>
#include <LUI/Rendering/UIShapeRect.h>

// LegToy
#include "../../Common/IResource_UI.h"

// LegToy - Showcase Scene
#include "ShowcaseLogicCenter.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// UI Example Showcase Internal

		namespace _UIExampleInternal
		{
			constexpr Leggiero::LUI::UICoordinateType kReferenceScreenWidth = static_cast<Leggiero::LUI::UICoordinateType>(1280.0f);
			constexpr Leggiero::LUI::UICoordinateType kReferenceScreenHeight = static_cast<Leggiero::LUI::UICoordinateType>(720.0f);
			constexpr Leggiero::LUI::UICoordinateRatioType kReferenceScreenAspect = static_cast<Leggiero::LUI::UICoordinateRatioType>(kReferenceScreenWidth / kReferenceScreenHeight);

			constexpr Leggiero::LUI::UICoordinateType kButtonBaseWidth = static_cast<Leggiero::LUI::UICoordinateType>(256.0f);
			constexpr Leggiero::LUI::UICoordinateType kButtonBaseHeight = static_cast<Leggiero::LUI::UICoordinateType>(96.0f);
			
			constexpr Leggiero::LUI::UICoordinateType kButtonBorderBaseWidth = static_cast<Leggiero::LUI::UICoordinateType>(4.0f);

			const static Leggiero::Graphics::GLByteARGB kButtonColor(255, 192, 192, 216);
			const static Leggiero::Graphics::GLByteARGB kButtonBorderColor(255, 128, 144, 216);
			const static Leggiero::Graphics::GLByteARGB kButtonClickedColor(255, 200, 200, 224);
			const static Leggiero::Graphics::GLByteARGB kButtonBorderClickedColor(255, 96, 128, 255);

			const static Leggiero::Graphics::GLByteARGB kSafeAreaColor(128, 128, 216, 128);
			constexpr Leggiero::LUI::UICoordinateType kSafeAreaFontBaseSize = static_cast<Leggiero::LUI::UICoordinateType>(64.0f);

			constexpr int kBlurBarrierBlurIterations = 12;
			constexpr float kBlurBarrierDownsampleRatio = 2.0f;

			constexpr Leggiero::LUI::UICoordinateType kCancelMoveThresholdAmount = static_cast<Leggiero::LUI::UICoordinateType>(16.0f);

			constexpr float kButtonHeightFontSizeRatio = 0.5f;
			constexpr float kButtonTextBaseStrokeWidth = 4.0f;
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcaseUIExampleController

		//------------------------------------------------------------------------------
		ShowcaseUIExampleController::ShowcaseUIExampleController(ShowcaseLogicCenter &logicCenter)
			: ShowcaseControllerBase(logicCenter)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseUIExampleController::~ShowcaseUIExampleController()
		{
		}

		//------------------------------------------------------------------------------
		void ShowcaseUIExampleController::InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, 
			Leggiero::Application::ApplicationComponent &appComponent, Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem, Leggiero::Graphics::GraphicResourceManagerComponent *graphicResourceManager, Leggiero::Font::GlyphManagerComponent *glyphManager, 
			SharedResource::IUIResources *uiSharedResources)
		{
			_LoadFont(bundleFileSystem);
			_ReadSettingsFromDescription(descriptionManager);

			m_uiMainObject = descriptionManager.FabricateUIObjectByPath("Showcase/UIExample", "ShowcaseUIPrefab", uiManager);
			uiPage->AddPostUIObject(m_uiMainObject);

			std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent(m_uiMainObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));
			std::shared_ptr<Leggiero::LUI::UIObject> uiHolderObject = showcaseUIFabricatedComponent->FindObject("uiHolder");

			uiHolderObject->AddPostUIObject(_CreateUIByCode(uiManager, appComponent, graphicResourceManager, glyphManager, uiSharedResources));
			uiPage->AddPostUIObject(m_testPopup);

			_InitializeMainUI(m_uiMainObject, descriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcaseUIExampleController::_FinalizeCaseController()
		{
			m_testButton->DiscardOnClickHandler();
			m_testButton.reset();

			m_testPopupCancel->DiscardOnCancelHandler();
			m_testPopupCancel.reset();
			m_testPopup.reset();

			m_testFontSet.reset();
		}

		//------------------------------------------------------------------------------
		bool ShowcaseUIExampleController::ProcessBackRequest()
		{
			if ((bool)m_testPopup && m_testPopup->IsVisible())
			{
				m_testPopup->SetIsVisible(false);
				return true;
			}
			return ShowcaseControllerBase::ProcessBackRequest();
		}

		//------------------------------------------------------------------------------
		void ShowcaseUIExampleController::_PrepareCloseStart()
		{
			if (m_testPopup)
			{
				m_testPopup->SetIsVisible(false);
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseUIExampleController::_LoadFont(Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem)
		{
			std::stringstream fontDataBuffer;

			bundleFileSystem.ReadBundleFileData("UI/Font/NotoSans-Regular.ttf", 0, fontDataBuffer); fontDataBuffer.seekg(0);
			std::shared_ptr<Leggiero::Font::SingleFontSet> fontNotoSans_Regular(std::make_shared<Leggiero::Font::SingleFontSet>(Leggiero::Font::FreeType::CreateFontFaceFromMemory(fontDataBuffer)));
			fontDataBuffer.str(""); fontDataBuffer.clear();

			bundleFileSystem.ReadBundleFileData("UI/Font/NotoSans-Bold.ttf", 0, fontDataBuffer); fontDataBuffer.seekg(0);
			std::shared_ptr<Leggiero::Font::SingleFontSet> fontNotoSans_Bold(std::make_shared<Leggiero::Font::SingleFontSet>(Leggiero::Font::FreeType::CreateFontFaceFromMemory(fontDataBuffer)));
			fontDataBuffer.str(""); fontDataBuffer.clear();

			m_testFontSet = std::make_shared<Leggiero::Font::StyledFontSet>(fontNotoSans_Regular, fontNotoSans_Bold);
		}

		//------------------------------------------------------------------------------
		void ShowcaseUIExampleController::_ReadSettingsFromDescription(Leggiero::LUI::Description::DescriptionManager &descriptionManager)
		{
			std::shared_ptr<Leggiero::LUI::Description::DescriptionUnit> showcaseUnit = descriptionManager.GetUnit("Showcase/UIExample");

			m_contentsWidth = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "showcaseWidth"));
			m_contentsHeight = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "contentsHeight"));
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Leggiero::LUI::UIObject> ShowcaseUIExampleController::_CreateUIByCode(std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::Application::ApplicationComponent &appComponent, Leggiero::Graphics::GraphicResourceManagerComponent *graphicResourceManager, Leggiero::Font::GlyphManagerComponent *glyphManager, SharedResource::IUIResources *uiSharedResources)
		{
			std::shared_ptr<Leggiero::LUI::UIObject> contentsObject = std::make_shared<Leggiero::LUI::UIObject>(uiManager, 
				std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIValuedSizeSubComponent>(m_contentsWidth, m_contentsHeight)));

			// Calculate Values
			Leggiero::LUI::UICoordinateType screenWidth = static_cast<Leggiero::LUI::UICoordinateType>(appComponent.AppInformation().GetIntegerPixelWidth());
			Leggiero::LUI::UICoordinateType screenHeight = static_cast<Leggiero::LUI::UICoordinateType>(appComponent.AppInformation().GetIntegerPixelHeight());
			Leggiero::LUI::UICoordinateRatioType screenAspect = static_cast<Leggiero::LUI::UICoordinateRatioType>(screenWidth / screenHeight);

			float uiBaseScale = 1.0f;
			if (screenAspect > _UIExampleInternal::kReferenceScreenAspect)
			{
				uiBaseScale = static_cast<float>(screenHeight / _UIExampleInternal::kReferenceScreenHeight);
			}
			else
			{
				uiBaseScale = static_cast<float>(screenWidth / _UIExampleInternal::kReferenceScreenWidth);
			}

			// Test Button
			Leggiero::LUI::UICoordinateType buttonWidth = static_cast<Leggiero::LUI::UICoordinateType>(_UIExampleInternal::kButtonBaseWidth * uiBaseScale);
			Leggiero::LUI::UICoordinateType buttonHeight = static_cast<Leggiero::LUI::UICoordinateType>(_UIExampleInternal::kButtonBaseHeight * uiBaseScale);

			std::shared_ptr<Leggiero::LUI::Element::UIElementFixedText> buttonText(std::make_shared<Leggiero::LUI::Element::UIElementFixedText>(uiManager, Leggiero::Font::TypesetCachedGlyphesHorizontalSingleLine(glyphManager,
				(const char *)u8"Test",
				m_testFontSet, static_cast<float>(buttonHeight * _UIExampleInternal::kButtonHeightFontSizeRatio), Leggiero::Font::GlyphStyleType::kBold, Leggiero::Graphics::HorizontalAlignType::kLeft, Leggiero::Graphics::VerticalAlignType::kTop,
				Leggiero::Graphics::GLByteARGB::kWhite, _UIExampleInternal::kButtonTextBaseStrokeWidth, Leggiero::Graphics::GLByteARGB::kBlack)));
			buttonText->ChangeLayoutComponent(std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIVisualSizeSubComponent>(buttonText)));
			buttonText->GetLayoutComponent()->parentReferenceRatioX = Leggiero::LUI::kUICoordinateRatio_Half; buttonText->GetLayoutComponent()->parentReferenceRatioY = Leggiero::LUI::kUICoordinateRatio_Half;
			buttonText->GetLayoutComponent()->anchorReferenceRatioX = Leggiero::LUI::kUICoordinateRatio_Half; buttonText->GetLayoutComponent()->anchorReferenceRatioY = static_cast<Leggiero::LUI::UICoordinateRatioType>(0.46f);

			m_testButton = std::make_shared<Leggiero::LUI::Element::UIElementTestButton>(uiManager,
				buttonWidth, buttonHeight, _UIExampleInternal::kButtonColor,
				static_cast<Leggiero::LUI::UICoordinateType>(_UIExampleInternal::kButtonBorderBaseWidth * uiBaseScale), _UIExampleInternal::kButtonBorderColor,
				buttonText, std::bind(&ShowcaseUIExampleController::_OnTestButtonClicked, this, std::placeholders::_1),
				true, _UIExampleInternal::kButtonClickedColor, _UIExampleInternal::kButtonBorderClickedColor
				);
			m_testButton->ChangeLayoutComponent(std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIValuedSizeSubComponent>(buttonWidth, buttonHeight)));
			m_testButton->GetLayoutComponent()->parentReferenceRatioX = Leggiero::LUI::kUICoordinateRatio_Half; m_testButton->GetLayoutComponent()->parentReferenceRatioY = Leggiero::LUI::kUICoordinateRatio_Half;
			m_testButton->GetLayoutComponent()->anchorReferenceRatioX = Leggiero::LUI::kUICoordinateRatio_Half; m_testButton->GetLayoutComponent()->anchorReferenceRatioY = Leggiero::LUI::kUICoordinateRatio_Half;

			contentsObject->AddPostUIObject(m_testButton);

			// Safe Area Popup
			m_testPopup = std::make_shared<Leggiero::LUI::Element::UIElementBlurredBarrier>(uiManager, graphicResourceManager, Leggiero::Graphics::GLByteARGB::kBlack,
				true, _UIExampleInternal::kBlurBarrierBlurIterations, uiSharedResources->GetCommonTextureBlurShader(), _UIExampleInternal::kBlurBarrierDownsampleRatio, uiSharedResources->GetCommonTextureColorShader());
			m_testPopup->ChangeLayoutComponent(std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIObjectSizeSubComponent>(m_testPopup)));
			m_testPopup->SetSize(screenWidth, screenHeight);
			m_testPopup->SetIsDynamic(false);
			m_testPopup->SetColor(Leggiero::Graphics::GLByteARGB::kWhite);
			
			Leggiero::LUI::UICoordinateType safeAreaWidth = static_cast<Leggiero::LUI::UICoordinateType>(appComponent.DeviceInformation().GetSafeArea().GetWidth());
			Leggiero::LUI::UICoordinateType safeAreaHeight = static_cast<Leggiero::LUI::UICoordinateType>(appComponent.DeviceInformation().GetSafeArea().GetHeight());

			std::shared_ptr<Leggiero::LUI::UIShapeRect> safeAreaRectShape(std::make_shared<Leggiero::LUI::UIShapeRect>(safeAreaWidth, safeAreaHeight, _UIExampleInternal::kSafeAreaColor));
			std::shared_ptr<Leggiero::LUI::UIObject> safeAreaUIObject = std::make_shared<Leggiero::LUI::UIObject>(uiManager,
				std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIValuedSizeSubComponent>(safeAreaWidth, safeAreaHeight)));
			safeAreaUIObject->RegisterComponent(std::make_shared<Leggiero::LUI::Rendering::ShapeSetRenderingComponent>(safeAreaRectShape));
			safeAreaUIObject->GetLayoutComponent()->parentOffsetX = static_cast<Leggiero::LUI::UICoordinateType>(appComponent.DeviceInformation().GetSafeArea().left);
			safeAreaUIObject->GetLayoutComponent()->parentOffsetY = static_cast<Leggiero::LUI::UICoordinateType>(appComponent.DeviceInformation().GetSafeArea().top);
			m_testPopup->AddPostUIObject(safeAreaUIObject);

			std::shared_ptr<Leggiero::LUI::Element::UIElementFixedText> safeAreaText(std::make_shared<Leggiero::LUI::Element::UIElementFixedText>(uiManager, Leggiero::Font::TypesetCachedGlyphesHorizontalSingleLine(glyphManager,
				(const char *)u8"Safe Area",
				m_testFontSet, static_cast<float>(_UIExampleInternal::kSafeAreaFontBaseSize * uiBaseScale), Leggiero::Font::GlyphStyleType::kBold, Leggiero::Graphics::HorizontalAlignType::kLeft, Leggiero::Graphics::VerticalAlignType::kTop,
				Leggiero::Graphics::GLByteARGB::kWhite)));
			safeAreaText->ChangeLayoutComponent(std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIVisualSizeSubComponent>(safeAreaText)));
			safeAreaText->GetLayoutComponent()->parentReferenceRatioX = Leggiero::LUI::kUICoordinateRatio_Half; safeAreaText->GetLayoutComponent()->parentReferenceRatioY = Leggiero::LUI::kUICoordinateRatio_Half;
			safeAreaText->GetLayoutComponent()->anchorReferenceRatioX = Leggiero::LUI::kUICoordinateRatio_Half; safeAreaText->GetLayoutComponent()->anchorReferenceRatioY = Leggiero::LUI::kUICoordinateRatio_Half;
			safeAreaUIObject->AddPostUIObject(safeAreaText);

			m_testPopupCancel = std::make_shared<Leggiero::LUI::Element::CancelPanelArea>(uiManager, Leggiero::LUI::IUIArea::kEverywhereArea, _UIExampleInternal::kCancelMoveThresholdAmount, Leggiero::LUI::IUIArea::kEverywhereArea, false, false,
				std::bind(&ShowcaseUIExampleController::_OnPopupCancel, this, std::placeholders::_1)
				);
			m_testPopup->AddPostUIObject(m_testPopupCancel);

			m_testPopup->SetIsVisible(false);

			return contentsObject;
		}

		//------------------------------------------------------------------------------
		void ShowcaseUIExampleController::_OnTestButtonClicked(std::shared_ptr<Leggiero::LUI::Element::UIElementTestButton> sender)
		{
			m_testPopup->MakeDirty();
			m_testPopup->SetAlpha(1.0f);
			m_testPopup->SetIsVisible(true);
		}

		//------------------------------------------------------------------------------
		void ShowcaseUIExampleController::_OnPopupCancel(std::shared_ptr<Leggiero::LUI::Element::CancelPanelArea> sender)
		{
			m_testPopup->SetIsVisible(false);
		}
	}
}
