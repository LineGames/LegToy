////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/CubeSpace.cpp (LegToy - LegToy)
//
// Cube Space Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "CubeSpace.h"

// Standard Library
#include <cmath>

// External Library
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>
#include <Utility/Math/Vector.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/SystemEventDispatcher.h>
#include <Application/IAppInformationProvider.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>
#include <Graphics/Common/GLColor.h>
#include <Graphics/Shader/CommonGLVertexType.h>
#include <Graphics/Shader/Basic/ColorSimpleShader.h>
#include <Graphics/Shader/Basic/TexturedColorSimpleShader.h>
#include <Graphics/Texture/TextureManager.h>
#include <Graphics/Texture/GLTextureResource.h>


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// Cube Space Internal Utility

		namespace _CubeSpaceInternal
		{
			constexpr float kMaximumLongerFov = Leggiero::Utility::Mathf::kPi * 0.9f;

			const glm::vec3 kRotationAxis(glm::normalize(glm::vec3(1.0f, 2.0f, 3.0f)));
			constexpr float kCubeRotationAngularSpeed = 1.2f;

			namespace PredefinedCubeModel
			{
				Leggiero::Graphics::ColoredVertex g_cubeColorVertices[] =
				{
					{ { -0.5f, -0.5f, -0.5f, 1.0f },	{ 0, 0, 0, 255 } },
					{ { 0.5f, -0.5f, -0.5f, 1.0f },		{ 255, 0, 0, 255 } },
					{ { -0.5f, 0.5f, -0.5f, 1.0f },		{ 0, 255, 0, 255 } },
					{ { 0.5f, 0.5f, -0.5f, 1.0f },		{ 255, 255, 0, 255 } },
					{ { -0.5f, -0.5f, 0.5f, 1.0f },		{ 0, 0, 255, 255 } },
					{ { 0.5f, -0.5f, 0.5f, 1.0f },		{ 255, 0, 255, 255 } },
					{ { -0.5f, 0.5f, 0.5f, 1.0f },		{ 0, 255, 255, 255 } },
					{ { 0.5f, 0.5f, 0.5f, 1.0f },		{ 255, 255, 255, 255 } }
				};

				GLubyte g_cubeColorIndices[] =
				{
					0, 1, 5, 5, 4, 0,
					0, 2, 6, 6, 4, 0,
					1, 3, 7, 7, 5, 1,
					2, 3, 7, 7, 6, 2,
					4, 5, 7, 7, 6, 4,
				};

				Leggiero::Graphics::TextureColoredVertex g_cubeTexturedVertices[] =
				{
					{ { -0.5f, -0.5f, -0.5f, 1.0f },	{ 255, 255, 255, 255 },		{ 1.0f, 1.0f } },
					{ { 0.5f, -0.5f, -0.5f, 1.0f },		{ 255, 255, 255, 255 },		{ 0.0f, 1.0f } },
					{ { -0.5f, 0.5f, -0.5f, 1.0f },		{ 255, 255, 255, 255 },		{ 1.0f, 0.0f } },
					{ { 0.5f, 0.5f, -0.5f, 1.0f },		{ 255, 255, 255, 255 },	{ 0.0f, 0.0f } }
				};

				GLubyte g_cubeTexturedIndices[] =
				{
					0, 1, 3, 3, 2, 0
				};
			}
		}


		//////////////////////////////////////////////////////////////////////////////// CubeSpace

		//------------------------------------------------------------------------------
		CubeSpace::CubeSpace(Leggiero::Application::ApplicationComponent *appComponent, Leggiero::Graphics::GraphicResourceManagerComponent *resourceManager, std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> colorShader, std::shared_ptr<Leggiero::Graphics::TexturedColorSimpleShader> textureShader, float referenceFov, float seeDistance)
			: m_appComponent(appComponent), m_colorShader(colorShader), m_textureShader(textureShader)
			, m_referenceFov(referenceFov), m_seeDistance(seeDistance)
			, m_cubeModelMat(1.0f), m_cubeRotationAngle(0.0f), m_eyeSeeAzimuth(0.0f)
			, m_isSeeMoving(false)
		{
			_SetScreenSize(static_cast<float>(appComponent->AppInformation().GetPixelWidth()), static_cast<float>(appComponent->AppInformation().GetPixelHeight()));
			m_testTexture = resourceManager->GetTextureManager().LoadTextureFromImageInBundle("texture.png");

			m_appComponent->SystemEventCenter().RegisterGraphicEventObserver(this);
		}

		//------------------------------------------------------------------------------
		CubeSpace::~CubeSpace()
		{
			m_appComponent->SystemEventCenter().UnRegisterGraphicEventObserver(this);
		}

		//------------------------------------------------------------------------------
		void CubeSpace::OnScreenSizeChanged(Leggiero::Application::DeviceScreenCoordType width, Leggiero::Application::DeviceScreenCoordType height)
		{
			_SetScreenSize(static_cast<float>(width), static_cast<float>(height));
		}

		//------------------------------------------------------------------------------
		void CubeSpace::UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval)
		{
			float elapsedTimeInSec = (float)((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);
			float effectiveElapsedTimeInSec = elapsedTimeInSec;
			if (effectiveElapsedTimeInSec > 0.5f)
			{
				// Cap time to 500ms
				effectiveElapsedTimeInSec = 0.5f;
			}

			// Update Cube Animation
			m_cubeRotationAngle = Leggiero::Utility::Mathf::WrapAngle2Pi(m_cubeRotationAngle + _CubeSpaceInternal::kCubeRotationAngularSpeed * effectiveElapsedTimeInSec);

			// Update See Moving
			if (m_isSeeMoving)
			{
				m_seeMovingAcuumulatedTime += effectiveElapsedTimeInSec;

				float azimuthDelta = m_seeMovingStartAzimuth - m_seeMovingTargetAzimuth;
				float decayingDelta = azimuthDelta * exp(-m_seeMovingTimeConstant * m_seeMovingAcuumulatedTime);
				if (abs(decayingDelta) < m_seeMovingEpslion)
				{
					m_eyeSeeAzimuth = m_seeMovingTargetAzimuth;
					m_isSeeMoving = false;
				}
				else
				{
					m_eyeSeeAzimuth = m_seeMovingTargetAzimuth + decayingDelta;
				}
			}

			// Updating Graphics
			Leggiero::Utility::Math::Vector2D<float> eyeSeeVector(m_seeDistance * sin(m_eyeSeeAzimuth), m_seeDistance * cos(m_eyeSeeAzimuth));
			glm::vec3 m_eyePos;
			glm::vec3 m_eyeSee;
			glm::vec3 m_eyeUp;
			m_eyePos[0] = 0.0f; m_eyePos[1] = 0.0f; m_eyePos[2] = -m_seeDistance;
			m_eyeSee[0] = eyeSeeVector.x; m_eyeSee[1] = 0.0f; m_eyeSee[2] = eyeSeeVector.y - m_seeDistance;
			m_eyeUp[0] = 0.0f; m_eyeUp[1] = 1.0f; m_eyeUp[2] = 0.0f;

			m_viewMat = glm::lookAt(m_eyePos, m_eyeSee, m_eyeUp);
			
			glm::vec3 moveTo;
			moveTo[0] = 0.0f;
			moveTo[1] = 0.0f;
			moveTo[2] = 2.0f;

			glm::mat4 modelMat(1.0f);
			modelMat = glm::translate(modelMat, moveTo);
			modelMat = glm::rotate(modelMat, m_cubeRotationAngle, _CubeSpaceInternal::kRotationAxis);
			m_cubeModelMat = modelMat;
		}

		//------------------------------------------------------------------------------
		void CubeSpace::Render()
		{
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// First, render with color shader
			if (m_colorShader->Use())
			{
				glUniformMatrix4fv(m_colorShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_projMat));
				glUniformMatrix4fv(m_colorShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_viewMat));
				glUniformMatrix4fv(m_colorShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_cubeModelMat));

				auto attribEnabledContext = m_colorShader->EnableUsingVertexAttribArray(true);
				Leggiero::Graphics::ColoredVertex::SetGLVertexAttribPointer(m_colorShader->GetPositionSlot(), m_colorShader->GetColorSlot(), _CubeSpaceInternal::PredefinedCubeModel::g_cubeColorVertices);

				glDrawElements(GL_TRIANGLES, sizeof(_CubeSpaceInternal::PredefinedCubeModel::g_cubeColorIndices) / sizeof(_CubeSpaceInternal::PredefinedCubeModel::g_cubeColorIndices[0]), GL_UNSIGNED_BYTE, _CubeSpaceInternal::PredefinedCubeModel::g_cubeColorIndices);
			}

			// Next, render with texture shader
			if (m_textureShader->Use())
			{
				glUniformMatrix4fv(m_textureShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_projMat));
				glUniformMatrix4fv(m_textureShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_viewMat));
				glUniformMatrix4fv(m_textureShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_cubeModelMat));

				auto attribEnabledContext = m_textureShader->EnableUsingVertexAttribArray(true);
				Leggiero::Graphics::TextureColoredVertex::SetGLVertexAttribPointer(m_textureShader->GetPositionSlot(), m_textureShader->GetColorSlot(), m_textureShader->GetTextureUVSlot(), _CubeSpaceInternal::PredefinedCubeModel::g_cubeTexturedVertices);

				if (m_testTexture)
				{
					m_testTexture->Bind(GL_TEXTURE0);
				}
				glUniform1i(m_textureShader->GetTextureLocation(), 0);

				glDrawElements(GL_TRIANGLES, sizeof(_CubeSpaceInternal::PredefinedCubeModel::g_cubeTexturedIndices) / sizeof(_CubeSpaceInternal::PredefinedCubeModel::g_cubeTexturedIndices[0]), GL_UNSIGNED_BYTE, _CubeSpaceInternal::PredefinedCubeModel::g_cubeTexturedIndices);
			}
		}

		//------------------------------------------------------------------------------
		void CubeSpace::ForceSetSeeAzimuth(float azimuth)
		{
			m_eyeSeeAzimuth = azimuth;
			m_isSeeMoving = false;
		}

		//------------------------------------------------------------------------------
		void CubeSpace::StartToSeeAzimuth(float azimuth, float timeConstant)
		{
			if (timeConstant < 0.0f)
			{
				// Cannot be negative
				m_seeMovingTimeConstant = 0.0f;
			}
			else
			{
				m_seeMovingTimeConstant = timeConstant;
			}

			m_seeMovingStartAzimuth = m_eyeSeeAzimuth;
			m_seeMovingTargetAzimuth = azimuth;
			m_seeMovingAcuumulatedTime = 0.0f;

			float azimuthDelta = m_seeMovingStartAzimuth - m_seeMovingTargetAzimuth;
			if (abs(azimuthDelta) < m_seeMovingEpslion)
			{
				m_eyeSeeAzimuth = azimuth;
				m_isSeeMoving = false;
			}
			else
			{
				m_isSeeMoving = true;
			}
		}

		//------------------------------------------------------------------------------
		float CubeSpace::CalculateSeeAzimuth(float screenXPos)
		{
			return atan((screenXPos - m_screenWidth * 0.5f) / (m_screenCoordinateConstant * m_seeDistance));
		}

		//------------------------------------------------------------------------------
		void CubeSpace::_SetScreenSize(float width, float height)
		{
			m_screenWidth = width;
			m_screenHeight = height;

			float screenAspect = m_screenWidth / m_screenHeight;
			float fovY = 1.0f;
			if (height < width)
			{
				if (m_referenceFov * screenAspect > _CubeSpaceInternal::kMaximumLongerFov)
				{
					fovY = _CubeSpaceInternal::kMaximumLongerFov / screenAspect;
				}
				else
				{
					fovY = m_referenceFov;
				}
			}
			else
			{
				if (m_referenceFov / screenAspect > _CubeSpaceInternal::kMaximumLongerFov)
				{
					fovY = _CubeSpaceInternal::kMaximumLongerFov;
				}
				else
				{
					fovY = m_referenceFov / screenAspect;
				}
			}

			m_projMat = glm::perspective(fovY, screenAspect, 1.0f, 100.f);
			m_screenXFov = fovY * screenAspect;

			m_screenCoordinateConstant = width * 0.5f / (m_seeDistance * tan(m_screenXFov * 0.5f));
			m_seeMovingEpslion = CalculateSeeAzimuth(m_screenWidth + 0.5f) - CalculateSeeAzimuth(m_screenWidth);
		}
	}
}
