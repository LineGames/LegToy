////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseMainMenuController.cpp (LegToy - LegToy)
//
// Main Menu Controller Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseMainMenuController.h"

// Standard Library
#include <functional>
#include <limits>

// External Library
#include <AHEasing/easing.h>

// Leggiero.Utility
#include <Utility/Math/Easing.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Description/UIDescriptionUnit.h>
#include <LUI/Component/UILayoutComponent.h>
#include <LUI/Component/UISizeSubComponent.h>
#include <LUI/Component/UITransformComponent.h>
#include <LUI/Rendering/ShapeSetRenderingComponent.h>
#include <LUI/Rendering/UIShapeRect.h>
#include <LUI/Prefab/UIFabricatedComponent.h>
#include <LUI/Prefab/UIPrefabPlaceProcessor.h>
#include <LUI/Controller/CommonButtonRenderingController.h>
#include <LUI/Element/UIElementSingleSimpleButton.h>

// LegToy - Showcase Scene
#include "ShowcaseLogicCenter.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// Main Menu Internal

		namespace _MainMenuInternal
		{
			constexpr float kMenuOpenInterval = 0.4f;
		}

		//////////////////////////////////////////////////////////////////////////////// ShowcaseMainMenuController

		//------------------------------------------------------------------------------
		ShowcaseMainMenuController::ShowcaseMainMenuController(ShowcaseLogicCenter &logicCenter)
			: m_logicCenter(logicCenter)
			, m_contentsAreaCenterX(std::numeric_limits<Leggiero::LUI::UICoordinateType>::quiet_NaN())
			, buttonVerticalCount(0)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseMainMenuController::~ShowcaseMainMenuController()
		{
		}

		//------------------------------------------------------------------------------
		void ShowcaseMainMenuController::InitializeMainMenuUI(std::shared_ptr<Leggiero::LUI::UIObject> uiPage, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager)
		{
			m_menuUnit = descriptionManager.GetUnit("Showcase/MainMenu");

			m_menuObject = descriptionManager.FabricateUIObjectByPath(m_menuUnit, "MainMenuPrefab", uiManager);
			uiPage->AddPostUIObject(m_menuObject);

			m_menuOffset = std::make_shared<Leggiero::LUI::OffsetTransform>(static_cast<Leggiero::LUI::UICoordinateType>(0.0f), static_cast<Leggiero::LUI::UICoordinateType>(0.0f));
			m_menuObject->RegisterComponent(std::make_shared<Leggiero::LUI::UIFixedTransformComponent>(m_menuOffset));

			m_contentsAreaCenterX = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(m_menuUnit, "contentsAreaCenterX"));
			m_buttonUnitHeight = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(m_menuUnit, "buttonUnitHeight"));
			m_menuBGHeaderHeight = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(m_menuUnit, "menuBGHeaderHeight"));
			m_menuBGPadding = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(m_menuUnit, "menuBGPadding"));
			m_menuBGWidth = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(m_menuUnit, "menuBGWidth"));

			std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> mainMenuFabricatedComponent(m_menuObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));

			m_buttonPanelObject = mainMenuFabricatedComponent->FindObject("menuContents");
			m_menuBGObject = mainMenuFabricatedComponent->FindObject("menuBG");
		}

		//------------------------------------------------------------------------------
		void ShowcaseMainMenuController::FinalizeController()
		{
			for (std::unordered_map<int, std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> >::iterator it = m_caseButtons.begin(); it != m_caseButtons.end(); ++it)
			{
				it->second->DiscardOnClickHandler();
			}
			m_caseButtons.clear();

			m_menuOffset.reset();

			m_buttonPanelObject.reset();
			m_menuBGObject.reset();
			m_menuObject.reset();

			m_menuUnit.reset();
		}

		//------------------------------------------------------------------------------
		void ShowcaseMainMenuController::AddButton(int creatingIndex, const Leggiero::GameDataString &buttonCaption, std::shared_ptr<Leggiero::LUI::UIManager> uiManager, Leggiero::LUI::Description::DescriptionManager &descriptionManager)
		{
			class InlineButtonTextProvider
				: public Leggiero::LUI::Description::Prefab::IUIPrefabPlaceProcessor
			{
			public:
				InlineButtonTextProvider(const Leggiero::GameDataString &btnCpation)
					: m_buttonCaption(btnCpation)
				{ }

			public:
				virtual Leggiero::GameDataString ProcessStringGeneration(const Leggiero::LUI::Description::SourcedStringEntry::StringGeneratingKeyType &generateKey) override
				{
					if (generateKey == "buttonCaption")
					{
						return m_buttonCaption;
					}
					return Leggiero::g_EmptyGameDataString;
				};

			protected:
				const Leggiero::GameDataString &m_buttonCaption;
			};
			InlineButtonTextProvider buttonTextProvider(buttonCaption);

			std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> createdButton = std::dynamic_pointer_cast<Leggiero::LUI::Element::SingleSimpleButton>(descriptionManager.FabricateUIObjectByPath(m_menuUnit, "ButtonPrefab", uiManager, &buttonTextProvider));
			createdButton->GetLayoutComponent()->parentOffsetY = static_cast<Leggiero::LUI::UICoordinateType>(buttonVerticalCount * m_buttonUnitHeight);
			m_buttonPanelObject->AddPostUIObject(createdButton);

			++buttonVerticalCount;

			createdButton->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::ActivatableButtonRenderingController>(*createdButton));
			createdButton->SetOnClickHandler(std::bind(&ShowcaseMainMenuController::_OnCaseButtonClicked, this, std::placeholders::_1, creatingIndex));

			m_caseButtons.insert(std::make_pair(creatingIndex, createdButton));
		}

		//------------------------------------------------------------------------------
		void ShowcaseMainMenuController::FitButtons()
		{
			Leggiero::LUI::UICoordinateType buttonListHeight = static_cast<Leggiero::LUI::UICoordinateType>(buttonVerticalCount * m_buttonUnitHeight);
			std::dynamic_pointer_cast<Leggiero::LUI::UIValuedSizeSubComponent>(m_buttonPanelObject->GetLayoutComponent()->GetSizeSubComponent())->SetHeight(buttonListHeight);

			Leggiero::LUI::UICoordinateType menuBGHeight = m_menuBGPadding + m_menuBGHeaderHeight + buttonListHeight + m_menuBGPadding;
			std::dynamic_pointer_cast<Leggiero::LUI::UIValuedSizeSubComponent>(m_menuObject->GetLayoutComponent()->GetSizeSubComponent())->SetHeight(menuBGHeight);
			
			std::shared_ptr<Leggiero::LUI::UIShapeRect> bgRect = std::dynamic_pointer_cast<Leggiero::LUI::UIShapeRect>(m_menuBGObject->GetComponentObject<Leggiero::LUI::Rendering::ShapeSetRenderingComponent>(Leggiero::LUI::UIComponentType::kRendering)->GetShapes()[0]);
			bgRect->SetSize(bgRect->GetWidth(), menuBGHeight);
			std::dynamic_pointer_cast<Leggiero::LUI::UIValuedSizeSubComponent>(m_menuBGObject->GetLayoutComponent()->GetSizeSubComponent())->SetHeight(menuBGHeight);

		}

		//------------------------------------------------------------------------------
		void ShowcaseMainMenuController::ResetAllStates()
		{
			for (std::unordered_map<int, std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> >::iterator it = m_caseButtons.begin(); it != m_caseButtons.end(); ++it)
			{
				it->second->SetIsActive(false);
			}

			m_menuOffset->SetOffsetX(-m_menuBGWidth);
			m_menuOpeningDuration = 0.0f;
			m_isMenuOpening = true;
		}

		//------------------------------------------------------------------------------
		void ShowcaseMainMenuController::ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval)
		{
			if (m_isMenuOpening)
			{
				float elapsedTimeInSec = (float)((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);
				m_menuOpeningDuration += elapsedTimeInSec;

				if (m_menuOpeningDuration >= _MainMenuInternal::kMenuOpenInterval)
				{
					m_menuOffset->SetOffsetX(static_cast<Leggiero::LUI::UICoordinateType>(0.0f));
					m_isMenuOpening = false;
				}
				else
				{
					float param = Leggiero::Utility::Math::ParameterInInterval(m_menuOpeningDuration, 0.0f, _MainMenuInternal::kMenuOpenInterval);
					m_menuOffset->SetOffsetX(-m_menuBGWidth * (1.0f - ExponentialEaseOut(param)));
				}
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseMainMenuController::SetButtonActive(int activeIndex)
		{
			for (std::unordered_map<int, std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> >::iterator it = m_caseButtons.begin(); it != m_caseButtons.end(); ++it)
			{
				it->second->SetIsActive(activeIndex == it->first);
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseMainMenuController::_OnCaseButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender, int index)
		{
			m_logicCenter.RequestCaseStartIntent(index);
		}
	}
}
