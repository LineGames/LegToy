////////////////////////////////////////////////////////////////////////////////
// Common/BackgroundRenderer.cpp (LegToy - LegToy)
//
// Inter-scene common background renderer implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "BackgroundRenderer.h"

// Standard Library
#include <vector>

// External Library
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/SystemEventDispatcher.h>
#include <Application/IAppInformationProvider.h>

// Leggiero.Graphics
#include <Graphics/Shader/Basic/ColorSimpleShader.h>


namespace LegToy
{
	//////////////////////////////////////////////////////////////////////////////// BackgroundRenderer Internal Utility

	namespace _BackgroundRendererInternal
	{
		namespace AppStartColor
		{
			constexpr GLubyte kRed = 27;
			constexpr GLubyte kGreen = 95;
			constexpr GLubyte kBlue = 134;
		}

		namespace ShowcaseScene
		{
			namespace LTColor
			{
				constexpr GLubyte kRed = 11;
				constexpr GLubyte kGreen = 64;
				constexpr GLubyte kBlue = 120;
			}

			// C'est Violette de droite.
			namespace RTColor
			{
				constexpr GLubyte kRed = 52;
				constexpr GLubyte kGreen = 36;
				constexpr GLubyte kBlue = 77;
			}

			// C'est Hortensia de gauche.
			namespace LBColor
			{
				constexpr GLubyte kRed = 3;
				constexpr GLubyte kGreen = 154;
				constexpr GLubyte kBlue = 190;
			}

			namespace RBColor
			{
				constexpr GLubyte kRed = 66;
				constexpr GLubyte kGreen = 60;
				constexpr GLubyte kBlue = 124;
			}
		}

		constexpr float kStateTransitionDuration_0_to_1 = 0.8f;
	}


	//////////////////////////////////////////////////////////////////////////////// BackgroundRenderer

	//------------------------------------------------------------------------------
	CommonBackgroundRenderer::CommonBackgroundRenderer(Leggiero::Application::ApplicationComponent *appComponent, std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> colorShader)
		: m_appComponent(appComponent), m_colorShader(colorShader)
		, m_state(0)
		, m_screenWidth(static_cast<float>(appComponent->AppInformation().GetPixelWidth())), m_screenHeight(static_cast<float>(appComponent->AppInformation().GetPixelHeight()))
		, m_defaultModelMat(1.0f)
	{
		_InitializeCoordinateTransform();

		m_appComponent->SystemEventCenter().RegisterGraphicEventObserver(this);
	}

	//------------------------------------------------------------------------------
	CommonBackgroundRenderer::~CommonBackgroundRenderer()
	{
		m_appComponent->SystemEventCenter().UnRegisterGraphicEventObserver(this);
	}

	//------------------------------------------------------------------------------
	void CommonBackgroundRenderer::UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval)
	{
		bool reCalculationNeeded = false;
		do
		{
			reCalculationNeeded = false;
			switch (m_state)
			{
				case 0:
					{
						m_bgColorLT.red = _BackgroundRendererInternal::AppStartColor::kRed;
						m_bgColorLT.green = _BackgroundRendererInternal::AppStartColor::kGreen;
						m_bgColorLT.blue = _BackgroundRendererInternal::AppStartColor::kBlue;

						m_bgColorRT = m_bgColorLT;
						m_bgColorLB = m_bgColorLT;
						m_bgColorRB = m_bgColorLT;
					}
					break;

				case 1:
					{
						float elapsedTimeInSec = (float)((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);
						float effectiveElapsedTimeInSec = elapsedTimeInSec;
						if (effectiveElapsedTimeInSec > 0.5f)
						{
							// Cap time to 500ms
							effectiveElapsedTimeInSec = 0.5f;
						}

						m_stateTransitionDuration += effectiveElapsedTimeInSec;
						if (m_stateTransitionDuration >= _BackgroundRendererInternal::kStateTransitionDuration_0_to_1)
						{
							m_state = 2;
							reCalculationNeeded = true;
						}
						else
						{
							float param = m_stateTransitionDuration / _BackgroundRendererInternal::kStateTransitionDuration_0_to_1;

							m_bgColorLT.red =  Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kRed, _BackgroundRendererInternal::ShowcaseScene::LTColor::kRed, param);
							m_bgColorLT.green = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kGreen, _BackgroundRendererInternal::ShowcaseScene::LTColor::kGreen, param);
							m_bgColorLT.blue = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kBlue, _BackgroundRendererInternal::ShowcaseScene::LTColor::kBlue, param);

							m_bgColorRT.red = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kRed, _BackgroundRendererInternal::ShowcaseScene::RTColor::kRed, param);
							m_bgColorRT.green = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kGreen, _BackgroundRendererInternal::ShowcaseScene::RTColor::kGreen, param);
							m_bgColorRT.blue = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kBlue, _BackgroundRendererInternal::ShowcaseScene::RTColor::kBlue, param);

							m_bgColorLB.red = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kRed, _BackgroundRendererInternal::ShowcaseScene::LBColor::kRed, param);
							m_bgColorLB.green = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kGreen, _BackgroundRendererInternal::ShowcaseScene::LBColor::kGreen, param);
							m_bgColorLB.blue = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kBlue, _BackgroundRendererInternal::ShowcaseScene::LBColor::kBlue, param);

							m_bgColorRB.red = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kRed, _BackgroundRendererInternal::ShowcaseScene::RBColor::kRed, param);
							m_bgColorRB.green = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kGreen, _BackgroundRendererInternal::ShowcaseScene::RBColor::kGreen, param);
							m_bgColorRB.blue = Leggiero::Utility::Math::LerpValue(_BackgroundRendererInternal::AppStartColor::kBlue, _BackgroundRendererInternal::ShowcaseScene::RBColor::kBlue, param);
						}
					}
					break;

				case 2:
					{
						m_bgColorLT.red = _BackgroundRendererInternal::ShowcaseScene::LTColor::kRed;
						m_bgColorLT.green = _BackgroundRendererInternal::ShowcaseScene::LTColor::kGreen;
						m_bgColorLT.blue = _BackgroundRendererInternal::ShowcaseScene::LTColor::kBlue;

						m_bgColorRT.red = _BackgroundRendererInternal::ShowcaseScene::RTColor::kRed;
						m_bgColorRT.green = _BackgroundRendererInternal::ShowcaseScene::RTColor::kGreen;
						m_bgColorRT.blue = _BackgroundRendererInternal::ShowcaseScene::RTColor::kBlue;

						m_bgColorLB.red = _BackgroundRendererInternal::ShowcaseScene::LBColor::kRed;
						m_bgColorLB.green = _BackgroundRendererInternal::ShowcaseScene::LBColor::kGreen;
						m_bgColorLB.blue = _BackgroundRendererInternal::ShowcaseScene::LBColor::kBlue;

						m_bgColorRB.red = _BackgroundRendererInternal::ShowcaseScene::RBColor::kRed;
						m_bgColorRB.green = _BackgroundRendererInternal::ShowcaseScene::RBColor::kGreen;
						m_bgColorRB.blue = _BackgroundRendererInternal::ShowcaseScene::RBColor::kBlue;
					}
					break;
			}
		} while (reCalculationNeeded);
	}

	//------------------------------------------------------------------------------
	void CommonBackgroundRenderer::Render()
	{
		// Render Background
		Leggiero::Graphics::ColoredVertex tempVertex;
		
		tempVertex = { { 0.0f, 0.0f, 0.0f, 1.0f },{ m_bgColorLT.red, m_bgColorLT.green, m_bgColorLT.blue, 255 } }; m_vertexBuffer.push_back(tempVertex);
		tempVertex = { { m_screenWidth, 0.0f, 0.0f, 1.0f },{ m_bgColorRT.red, m_bgColorRT.green, m_bgColorRT.blue, 255 } }; m_vertexBuffer.push_back(tempVertex);
		tempVertex = { { 0.0f, m_screenHeight, 0.0f, 1.0f },{ m_bgColorLB.red, m_bgColorLB.green, m_bgColorLB.blue, 255 } }; m_vertexBuffer.push_back(tempVertex);
		tempVertex = { { m_screenWidth, m_screenHeight, 0.0f, 1.0f },{ m_bgColorRB.red, m_bgColorRB.green, m_bgColorRB.blue, 255 } }; m_vertexBuffer.push_back(tempVertex);

		m_indexBuffer.push_back((GLushort)0); m_indexBuffer.push_back((GLushort)1); m_indexBuffer.push_back((GLushort)2);
		m_indexBuffer.push_back((GLushort)2); m_indexBuffer.push_back((GLushort)3); m_indexBuffer.push_back((GLushort)1);

		m_colorShader->Use();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		glUniformMatrix4fv(m_colorShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_projMat));
		glUniformMatrix4fv(m_colorShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_viewMat));
		glUniformMatrix4fv(m_colorShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_defaultModelMat));

		auto vertexAttribEnabledContext = m_colorShader->EnableUsingVertexAttribArray(true);
		Leggiero::Graphics::ColoredVertex::SetGLVertexAttribPointer(m_colorShader->GetPositionSlot(), m_colorShader->GetColorSlot(), &m_vertexBuffer[0]);
		glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);

		m_vertexBuffer.clear();
		m_indexBuffer.clear();
	}

	//------------------------------------------------------------------------------
	void CommonBackgroundRenderer::StartToShowcaseScene()
	{
		if (m_state == 0)
		{
			m_state = 1;
			m_stateTransitionDuration = 0.0f;
		}
	}

	//------------------------------------------------------------------------------
	void CommonBackgroundRenderer::OnScreenSizeChanged(Leggiero::Application::DeviceScreenCoordType width, Leggiero::Application::DeviceScreenCoordType height)
	{
		m_screenWidth = static_cast<float>(width);
		m_screenHeight = static_cast<float>(height);

		_InitializeCoordinateTransform();
	}

	//------------------------------------------------------------------------------
	void CommonBackgroundRenderer::_InitializeCoordinateTransform()
	{
		m_projMat = glm::ortho(0.0f, m_screenWidth, -m_screenHeight, 0.0f, -1.0f, 1.0f);

		glm::vec3 eyePos; eyePos[0] = 0.0f; eyePos[1] = 0.0f; eyePos[2] = -1.0f;
		glm::vec3 eyeSee; eyeSee[0] = eyePos[0]; eyeSee[1] = eyePos[1]; eyeSee[2] = 0.0f;
		glm::vec3 eyeUp; eyeUp[0] = 0.0f; eyeUp[1] = -1.0f; eyeUp[2] = 0.0f;
		m_viewMat = glm::lookAt(eyePos, eyeSee, eyeUp);
	}
}
