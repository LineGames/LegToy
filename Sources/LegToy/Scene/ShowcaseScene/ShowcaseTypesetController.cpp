////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseTypesetController.cpp (LegToy - LegToy)
//
// Typesetting Showcase Controller Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseTypesetController.h"

// Standard Library
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <limits>
#include <vector>

// External Library
#include <utfcpp/utf8.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Font
#include <Font/Common/Typeset.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Common/UITransform.h>
#include <LUI/Component/UILayoutComponent.h>
#include <LUI/Component/UISizeSubComponent.h>
#include <LUI/Component/UITransformComponent.h>
#include <LUI/Rendering/ShapeSetRenderingComponent.h>
#include <LUI/Rendering/UIShapeRect.h>
#include <LUI/Element/UIElementFixedText.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Description/UIDescriptionUnit.h>
#include <LUI/Description/UIDescriptionFont.h>
#include <LUI/Prefab/UIFabricatedComponent.h>

// LegToy - Showcase Scene
#include "ShowcaseLogicCenter.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// Typeset Showcase Internal

		namespace _TypesetInternal
		{
			const static std::string kArticleDataFilePath("Showcase/Typeset/Article.dat");
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcaseTypesetController

		//------------------------------------------------------------------------------
		ShowcaseTypesetController::ShowcaseTypesetController(ShowcaseLogicCenter &logicCenter)
			: ShowcaseControllerBase(logicCenter)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseTypesetController::~ShowcaseTypesetController()
		{
		}

		//------------------------------------------------------------------------------
		void ShowcaseTypesetController::InitializeShowcaseUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager, Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem, Leggiero::Font::GlyphManagerComponent *glyphManager)
		{
			m_uiMainObject = descriptionManager.FabricateUIObjectByPath("Showcase/Typesetting", "ShowcaseUIPrefab", uiManager);
			uiPage->AddPostUIObject(m_uiMainObject);

			std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent(m_uiMainObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));

			m_scrollPanel = std::dynamic_pointer_cast<Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel>(showcaseUIFabricatedComponent->FindObject("articleScrollPanel"));

			std::shared_ptr<Leggiero::LUI::UIObject> articleObject = _CreateArticle(bundleFileSystem, glyphManager, uiManager, descriptionManager);

			m_scrollPanel->GetContentsUIObject()->AddPostUIObject(articleObject);
			std::shared_ptr<Leggiero::LUI::UIValuedSizeSubComponent> scrollContentsSizeSubComponent(std::dynamic_pointer_cast<Leggiero::LUI::UIValuedSizeSubComponent>(m_scrollPanel->GetContentsUIObject()->GetLayoutComponent()->GetSizeSubComponent()));
			scrollContentsSizeSubComponent->SetWidth(articleObject->CalculateWidth());
			scrollContentsSizeSubComponent->SetHeight(articleObject->CalculateHeight());
			m_scrollPanel->SetContentsHeight(scrollContentsSizeSubComponent->GetHeight());

			_InitializeScrollIndicator(showcaseUIFabricatedComponent, descriptionManager);

			_InitializeMainUI(m_uiMainObject, descriptionManager);
		}

		//------------------------------------------------------------------------------
		void ShowcaseTypesetController::_ResetCaseStates()
		{
			m_scrollPanel->SetPosition(0.0f);
			if (m_isScrollEnabled)
			{
				m_scrollMarkerOffset->SetOffsetY(static_cast<Leggiero::LUI::UICoordinateType>(0.0f));
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseTypesetController::_FinalizeCaseController()
		{
			m_scrollPanel->DiscardOnPositionChangedHandler();
			m_scrollPanel.reset();

			m_scrollMarker.reset();
			m_scrollMarkerOffset.reset();
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<Leggiero::LUI::UIObject> ShowcaseTypesetController::_CreateArticle(Leggiero::FileSystem::BundleFileResourceComponent &bundleFileSystem, Leggiero::Font::GlyphManagerComponent *glyphManager, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager)
		{
			size_t articleDataSize = bundleFileSystem.GetBundleFileLength(_TypesetInternal::kArticleDataFilePath);
			if (articleDataSize == 0)
			{
				// Something Wrong...
				return nullptr;
			}

			// Read Article Strings
			std::vector<uint8_t> titleBuffer;
			titleBuffer.reserve(256);
			std::vector<uint8_t> filteredTextBuffer;
			filteredTextBuffer.reserve(articleDataSize + 1);

			char *dataBuffer = (char *)malloc(articleDataSize + 1);
			{
				auto bufferReleaseFunc = [dataBuffer]() mutable { free(dataBuffer); };
				FINALLY_OF_BLOCK(_RELEASE_BUFFER, bufferReleaseFunc);

				bundleFileSystem.ReadBundleFileData("Showcase/Typeset/Article.dat", 0, dataBuffer, articleDataSize);
				dataBuffer[articleDataSize] = '\0';

				bool isReadingTitle = true;
				char *byteIt = dataBuffer;
				char *byteItEnd = (byteIt + articleDataSize);
				while (dataBuffer < byteItEnd)
				{
					uint32_t codepoint = 0u;
					try
					{
						codepoint = utf8::next(byteIt, byteItEnd);
					}
					catch (utf8::not_enough_room ex)
					{
						// String End
						break;
					}

					if (isReadingTitle)
					{
						if (codepoint == 10u)
						{
							isReadingTitle = false;
						}
						else
						{
							bool isValidChar = true;
							if (codepoint < 32u)
							{
								switch (codepoint)
								{
									case 9u:	// '\t'
										break;

									default:
										isValidChar = false;
										break;
								}
							}
							else if (codepoint == 127u)
							{
								isValidChar = false;
							}
							if (isValidChar)
							{
								utf8::append(codepoint, std::back_inserter(titleBuffer));
							}
						}
					}
					else
					{
						uint32_t effectiveCodepoint = codepoint;
						if (effectiveCodepoint < 32u)
						{
							switch (effectiveCodepoint)
							{
								case 9u:	// '\t'
								case 10u:	// '\n'
								case 11u:	// '\v'
									break;

								case 12u:	// '\f'
									effectiveCodepoint = 10u;
									break;

								default:
									effectiveCodepoint = 0u;
									break;
							}
						}
						else if (effectiveCodepoint == 127u)
						{
							effectiveCodepoint = 0u;
						}
						if (effectiveCodepoint != 0u)
						{
							utf8::append(effectiveCodepoint, std::back_inserter(filteredTextBuffer));
						}
					}
				}
			}

			Leggiero::GameDataString titleString((char *)&titleBuffer[0], titleBuffer.size());
			Leggiero::GameDataString articleString((char *)&filteredTextBuffer[0], filteredTextBuffer.size());

			// Typeset article
			std::shared_ptr<Leggiero::LUI::Description::DescriptionUnit> showcaseUnit = descriptionManager.GetUnit("Showcase/Typesetting");

			Leggiero::Graphics::BasicCoordType articleWidth = static_cast<Leggiero::Graphics::BasicCoordType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articleWidth"));
			float lineSpaceRatio = static_cast<float>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articleLineSpacing"));

			Leggiero::LUI::Description::ObjectEnvironmentPair titleFontClassDescriptionFound(showcaseUnit->SelectDescriptionByName(descriptionManager, "articleTitleFontClass"));
			std::shared_ptr<Leggiero::LUI::Description::FontClass> titleFontClassDescription(std::dynamic_pointer_cast<Leggiero::LUI::Description::FontClass>(std::get<0>(titleFontClassDescriptionFound)));
			std::shared_ptr<Leggiero::Font::MultiPageFontSetting> titleFontSetting(titleFontClassDescription->GetFontSetting(std::get<1>(titleFontClassDescriptionFound)));
			Leggiero::Font::GlyphSizeType titleFontSize = static_cast<Leggiero::Font::GlyphSizeType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articleTitleFontSize"));
			Leggiero::Graphics::GLByteARGB titleFontColor = descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::ColorARGBValueType>(showcaseUnit, "articleTitleFontColor");

			std::shared_ptr<Leggiero::Font::CachedGlyphTypesetting> titleTypesetting = Leggiero::Font::TypesetCachedGlyphesHorizontalBlock(glyphManager,
				titleString,
				titleFontSetting, titleFontSize,
				articleWidth, Leggiero::Font::TextAlignType::kCenter, Leggiero::Font::TextAlignType::kCenter, lineSpaceRatio,
				Leggiero::Graphics::HorizontalAlignType::kLeft, Leggiero::Graphics::VerticalAlignType::kTop,
				titleFontColor);

			Leggiero::LUI::Description::ObjectEnvironmentPair articleFontClassDescriptionFound(showcaseUnit->SelectDescriptionByName(descriptionManager, "articleFontClass"));
			std::shared_ptr<Leggiero::LUI::Description::FontClass> articleFontClassDescription(std::dynamic_pointer_cast<Leggiero::LUI::Description::FontClass>(std::get<0>(articleFontClassDescriptionFound)));
			std::shared_ptr<Leggiero::Font::MultiPageFontSetting> articleFontSetting(articleFontClassDescription->GetFontSetting(std::get<1>(articleFontClassDescriptionFound)));
			Leggiero::Font::GlyphSizeType articleFontSize = static_cast<Leggiero::Font::GlyphSizeType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articleFontSize"));
			Leggiero::Graphics::GLByteARGB articleFontColor = descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::ColorARGBValueType>(showcaseUnit, "articleFontColor");
			
			std::shared_ptr<Leggiero::Font::CachedGlyphTypesetting> articleTypesetting = Leggiero::Font::TypesetCachedGlyphesHorizontalBlock(glyphManager,
				articleString,
				articleFontSetting, articleFontSize,
				articleWidth, Leggiero::Font::TextAlignType::kJustifyWord, Leggiero::Font::TextAlignType::kLeft, lineSpaceRatio,
				Leggiero::Graphics::HorizontalAlignType::kLeft, Leggiero::Graphics::VerticalAlignType::kTop,
				articleFontColor);

			// Create Article Text UI Object
			Leggiero::LUI::UICoordinateType titleShadowOffsetX = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articleTitleShadowOffsetX"));
			Leggiero::LUI::UICoordinateType titleShadowOffsetY = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articleTitleShadowOffsetY"));
			bool hasTitleShadow = ((titleShadowOffsetX * titleShadowOffsetX + titleShadowOffsetY * titleShadowOffsetY) > Leggiero::LUI::kFloatEpsilon);
			Leggiero::Graphics::GLByteARGB titleShadowColor(Leggiero::Graphics::GLByteARGB::kTransparent);
			if (hasTitleShadow)
			{
				titleShadowColor = descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::ColorARGBValueType>(showcaseUnit, "articleTitleShadowColor");
			}
			std::shared_ptr<Leggiero::LUI::Element::UIElementFixedText> titleTextElement = std::make_shared<Leggiero::LUI::Element::UIElementFixedText>(uiManager, titleTypesetting, hasTitleShadow, titleShadowColor, Leggiero::LUI::UIVector2D(titleShadowOffsetX, titleShadowOffsetY));
			titleTextElement->ChangeLayoutComponent(std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIObjectSizeSubComponent>(titleTextElement)));

			std::shared_ptr<Leggiero::LUI::Element::UIElementFixedText> articleTextElement = std::make_shared<Leggiero::LUI::Element::UIElementFixedText>(uiManager, articleTypesetting);

			// Create Article UI Object
			Leggiero::LUI::UICoordinateType articlePadding = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articlePadding"));
			Leggiero::LUI::UICoordinateType articleTitleHeadMargin = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articleTitleHeadMargin"));
			Leggiero::LUI::UICoordinateType articleTitleMargin = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseUnit, "articleTitleMargin"));
			Leggiero::LUI::UICoordinateType articleHeight = articlePadding + articleTitleHeadMargin + titleTextElement->CalculateHeight() + articleTitleMargin + articleTextElement->CalculateHeight() + articlePadding;

			titleTextElement->GetLayoutComponent()->parentReferenceRatioX = static_cast<Leggiero::LUI::UICoordinateRatioType>(0.5f);
			titleTextElement->GetLayoutComponent()->anchorReferenceRatioX = static_cast<Leggiero::LUI::UICoordinateRatioType>(0.5f);
			titleTextElement->GetLayoutComponent()->parentOffsetY = articlePadding + articleTitleHeadMargin;

			articleTextElement->GetLayoutComponent()->parentOffsetX = articlePadding;
			articleTextElement->GetLayoutComponent()->parentOffsetY = articlePadding + articleTitleHeadMargin + titleTextElement->CalculateHeight() + articleTitleMargin;

			titleTextElement->GraphicPrepare();
			articleTextElement->GraphicPrepare();

			std::shared_ptr<Leggiero::LUI::UIObject> articleObject = std::make_shared<Leggiero::LUI::UIObject>(uiManager,
				std::make_shared<Leggiero::LUI::UILayoutComponent>(std::make_shared<Leggiero::LUI::UIValuedSizeSubComponent>(static_cast<Leggiero::LUI::UICoordinateType>(articleWidth), articleHeight)));
			articleObject->AddPostUIObject(titleTextElement);
			articleObject->AddPostUIObject(articleTextElement);

			return articleObject;
		}

		//------------------------------------------------------------------------------
		void ShowcaseTypesetController::_InitializeScrollIndicator(std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent, Leggiero::LUI::Description::DescriptionManager &descriptionManager)
		{
			m_scrollMarker = showcaseUIFabricatedComponent->FindObject("scrollMarker");

			float scrollMax = static_cast<float>(m_scrollPanel->GetMaximumValue());
			if (scrollMax <= 0.0f)
			{
				m_isScrollEnabled = false;
				m_scrollMarker->SetIsVisible(false);
				return;
			}

			m_isScrollEnabled = true;

			float contentsTotalHeight = static_cast<float>(m_scrollPanel->GetCurrentContentsHeight());
			float boundedHeight = static_cast<float>(m_scrollPanel->GetBoundedHeight());

			float showingContentsRatio = boundedHeight / contentsTotalHeight;

			Leggiero::LUI::UICoordinateType panelVisualHeight = m_scrollPanel->CalculateHeight();
			Leggiero::LUI::UICoordinateType markerHeight = static_cast<Leggiero::LUI::UICoordinateType>(std::ceil(showingContentsRatio * panelVisualHeight));

			Leggiero::LUI::UICoordinateType scrollMarkerMinLength = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>("Showcase/Typesetting", "scrollMarkerMinLength"));
			if (markerHeight < scrollMarkerMinLength)
			{
				markerHeight = scrollMarkerMinLength;
			}
			
			std::shared_ptr<Leggiero::LUI::UIShapeRect> markerShapeRect = std::dynamic_pointer_cast<Leggiero::LUI::UIShapeRect>(m_scrollMarker->GetComponentObject<Leggiero::LUI::Rendering::ShapeSetRenderingComponent>(Leggiero::LUI::UIComponentType::kRendering)->GetShapes()[0]);
			markerShapeRect->SetSize(markerShapeRect->GetWidth(), markerHeight);
			std::dynamic_pointer_cast<Leggiero::LUI::UIValuedSizeSubComponent>(m_scrollMarker->GetLayoutComponent()->GetSizeSubComponent())->SetHeight(markerHeight);

			m_scrollMarkerOffset = std::make_shared<Leggiero::LUI::OffsetTransform>(static_cast<Leggiero::LUI::UICoordinateType>(0.0f), static_cast<Leggiero::LUI::UICoordinateType>(0.0f));
			m_scrollMarker->RegisterComponent(std::make_shared<Leggiero::LUI::UIFixedTransformComponent>(m_scrollMarkerOffset));

			Leggiero::LUI::UICoordinateType markerMovingHeight = panelVisualHeight - markerHeight;
			m_markerPositionMultiplier = markerMovingHeight / scrollMax;

			m_scrollPanel->SetOnPositionChangedHandler(std::bind(&ShowcaseTypesetController::_OnScrollPanelPositionChanged, this, std::placeholders::_1, std::placeholders::_2));
		}

		//------------------------------------------------------------------------------
		void ShowcaseTypesetController::_OnScrollPanelPositionChanged(std::shared_ptr<Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel> sender, Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel::PositionValueType position)
		{
			if (!m_isScrollEnabled)
			{
				return;
			}

			Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel::PositionValueType clippedPosition = position;
			if (clippedPosition < static_cast<Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel::PositionValueType>(0.0f))
			{
				clippedPosition = static_cast<Leggiero::LUI::Element::UIElementSimpleVerticalScrollPanel::PositionValueType>(0.0f);
			}
			else if (clippedPosition > sender->GetMaximumValue())
			{
				clippedPosition = sender->GetMaximumValue();
			}

			Leggiero::LUI::UICoordinateType markerOffset = static_cast<Leggiero::LUI::UICoordinateType>(clippedPosition * m_markerPositionMultiplier);
			m_scrollMarkerOffset->SetOffsetY(markerOffset);
		}
	}
}
