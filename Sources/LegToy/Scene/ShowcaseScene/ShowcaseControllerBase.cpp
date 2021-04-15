////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/ShowcaseControllerBase.cpp (LegToy - LegToy)
//
// Showcase Controller Base Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ShowcaseControllerBase.h"

// Standard Library
#include <functional>

// External Library
#include <AHEasing/easing.h>

// Leggiero.Utility
#include <Utility/Math/Easing.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/UIObject.h>
#include <LUI/Common/UITransform.h>
#include <LUI/Component/UILayoutComponent.h>
#include <LUI/Component/UITransformComponent.h>
#include <LUI/Element/UIElementSingleSimpleButton.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Description/UIDescriptionUnit.h>
#include <LUI/Prefab/UIFabricatedComponent.h>
#include <LUI/Controller/CommonButtonRenderingController.h>

// LegToy - Showcase Scene
#include "ShowcaseLogicCenter.h"


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// Showcase Base Internal

		namespace _ShowcaseBaseInternal
		{
			constexpr float kOpeningAnimationDuration = 0.24f;
			constexpr float kClosingAnimationDuration = 0.2f;
		}


		//////////////////////////////////////////////////////////////////////////////// ShowcaseControllerBase

		//------------------------------------------------------------------------------
		ShowcaseControllerBase::ShowcaseControllerBase(ShowcaseLogicCenter &logicCenter)
			: m_logicCenter(logicCenter)
			, m_caseIndex(-1)
		{
		}

		//------------------------------------------------------------------------------
		ShowcaseControllerBase::~ShowcaseControllerBase()
		{
		}

		//------------------------------------------------------------------------------
		void ShowcaseControllerBase::_InitializeMainUI(std::shared_ptr<Leggiero::LUI::UIObject> showcaseMainUIObject, Leggiero::LUI::Description::DescriptionManager &descriptionManager)
		{
			std::shared_ptr<Leggiero::LUI::Description::DescriptionUnit> showcaseBaseUnit = descriptionManager.GetUnit("Showcase/Showcase");

			m_animOpeningMoving = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseBaseUnit, "openingAnimMovingAmount"));
			m_animClosingMoving = static_cast<Leggiero::LUI::UICoordinateType>(descriptionManager.ReadDescriptionValue<Leggiero::LUI::Description::FloatingPointValueType>(showcaseBaseUnit, "closingAnimMovingAmount"));

			m_uiMainObject = showcaseMainUIObject;
			m_mainUIOffset = std::make_shared<Leggiero::LUI::OffsetTransform>(static_cast<Leggiero::LUI::UICoordinateType>(0.0f), static_cast<Leggiero::LUI::UICoordinateType>(0.0f));
			m_uiMainObject->RegisterComponent(std::make_shared<Leggiero::LUI::UIFixedTransformComponent>(m_mainUIOffset));

			std::shared_ptr<Leggiero::LUI::UIFabricatedComponent> showcaseUIFabricatedComponent(m_uiMainObject->GetComponentObject<Leggiero::LUI::UIFabricatedComponent>(Leggiero::LUI::UIComponentType::kFabricated));

			m_closeButton = std::dynamic_pointer_cast<Leggiero::LUI::Element::SingleSimpleButton>(showcaseUIFabricatedComponent->FindObject("btnClose"));
			m_closeButton->SetRenderingController(std::make_shared<Leggiero::LUI::Controller::SimpleButtonRenderingController>(*m_closeButton));
			m_closeButton->SetOnClickHandler(std::bind(&ShowcaseControllerBase::_OnCloseButtonClicked, this, std::placeholders::_1));

			m_uiMainObject->SetIsVisible(false);
		}

		//------------------------------------------------------------------------------
		void ShowcaseControllerBase::FinalizeController()
		{
			_FinalizeCaseController();

			m_closeButton->DiscardOnClickHandler();
			m_closeButton.reset();

			m_uiMainObject.reset();
			m_mainUIOffset.reset();
		}

		//------------------------------------------------------------------------------
		void ShowcaseControllerBase::ResetAllStates()
		{
			m_state = 0;
			m_uiMainObject->SetIsVisible(false);

			_ResetCaseStates();
		}

		//------------------------------------------------------------------------------
		void ShowcaseControllerBase::ProcessFrameLogic(Leggiero::GameTimeClockType::time_point frameReferenceTime, Leggiero::GameTimeClockType::duration frameInterval)
		{
			_ProcessCaseFrameLogic(frameReferenceTime, frameInterval);

			switch (m_state)
			{
				case 1:	// Opening
					{
						float elapsedTimeInSec = (float)((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);
						m_inStateTime += elapsedTimeInSec;

						if (m_inStateTime >= _ShowcaseBaseInternal::kOpeningAnimationDuration)
						{
							m_state = 0;
							m_uiMainObject->SetIsVisible(true);
							m_uiMainObject->SetAlpha(1.0f);
							m_mainUIOffset->SetOffset(static_cast<Leggiero::LUI::UICoordinateType>(0.0f), static_cast<Leggiero::LUI::UICoordinateType>(0.0f));
						}
						else
						{
							float inStateParam = Leggiero::Utility::Math::ParameterInInterval(m_inStateTime, 0.0f, _ShowcaseBaseInternal::kOpeningAnimationDuration);

							m_uiMainObject->SetAlpha(CircularEaseOut(inStateParam));
							m_mainUIOffset->SetOffsetX(-static_cast<Leggiero::LUI::UICoordinateType>((1.0f - ElasticEaseOut(inStateParam)) * m_animOpeningMoving));
						}
					}
					break;

				case 2:	// Closing
					{
						float elapsedTimeInSec = (float)((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);
						m_inStateTime += elapsedTimeInSec;

						if (m_inStateTime >= _ShowcaseBaseInternal::kClosingAnimationDuration)
						{
							m_state = 0;
							m_uiMainObject->SetIsVisible(false);

							m_logicCenter.OnCaseUIClosed(m_caseIndex);
						}
						else
						{
							float inStateParam = Leggiero::Utility::Math::ParameterInInterval(m_inStateTime, 0.0f, _ShowcaseBaseInternal::kClosingAnimationDuration);

							m_uiMainObject->SetAlpha(1.0f - inStateParam);
							m_mainUIOffset->SetOffsetY(static_cast<Leggiero::LUI::UICoordinateType>(QuadraticEaseIn(inStateParam) * m_animClosingMoving));
						}
					}
					break;
			}
		}

		//------------------------------------------------------------------------------
		void ShowcaseControllerBase::StartOpen()
		{
			_PrepareCloseStart();

			m_state = 1;
			m_inStateTime = 0.0f;

			m_uiMainObject->SetIsVisible(true);
			m_uiMainObject->SetAlpha(0.0f);
			m_mainUIOffset->SetOffset(-m_animOpeningMoving, static_cast<Leggiero::LUI::UICoordinateType>(0.0f));
		}

		//------------------------------------------------------------------------------
		void ShowcaseControllerBase::StartClose()
		{
			m_state = 2;
			m_inStateTime = 0.0f;

			m_uiMainObject->SetIsVisible(true);
			m_uiMainObject->SetAlpha(1.0f);
			m_mainUIOffset->SetOffset(static_cast<Leggiero::LUI::UICoordinateType>(0.0f), static_cast<Leggiero::LUI::UICoordinateType>(0.0f));
		}

		//------------------------------------------------------------------------------
		void ShowcaseControllerBase::ForceClose()
		{
			m_uiMainObject->SetIsVisible(false);
			m_state = 0;
		}

		//------------------------------------------------------------------------------
		bool ShowcaseControllerBase::ProcessBackRequest()
		{
			if (m_state == 0)
			{
				StartClose();
				return true;
			}
			return false;
		}

		//------------------------------------------------------------------------------
		void ShowcaseControllerBase::_OnCloseButtonClicked(std::shared_ptr<Leggiero::LUI::Element::SingleSimpleButton> sender)
		{
			if (m_state != 0)
			{
				return;
			}
			StartClose();
		}
	}
}
