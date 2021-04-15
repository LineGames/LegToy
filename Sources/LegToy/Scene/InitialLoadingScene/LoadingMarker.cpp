////////////////////////////////////////////////////////////////////////////////
// Scene/InitialLoadingScene/LoadingMarker.cpp (LegToy - LegToy)
//
// Loading Marker Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "LoadingMarker.h"

// Standard Library
#include <cmath>

// External Library
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AHEasing/easing.h>

// Leggiero.Utility
#include <Utility/Math/SimpleGeometry.h>
#include <Utility/Math/SimpleMath.h>

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IAppInformationProvider.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>
#include <Graphics/Shader/Basic/ColorSimpleShader.h>


namespace LegToy
{
	namespace InitialLoadingSceneStuffs
	{
		//////////////////////////////////////////////////////////////////////////////// LoadingMarker Internal Utility

		namespace _LoadingMarkerInternal
		{
			void _PushSingleColoredQuadToVBV(std::vector<Leggiero::Graphics::ColoredVertex> &vertexBuffer, std::vector<GLushort> &indexBuffer,
				const Leggiero::Graphics::GLByteARGB &quadColor,
				float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y)
			{
				Leggiero::Graphics::ColoredVertex funcTempVertex;

				GLuint startIndex = (GLuint)vertexBuffer.size();
				funcTempVertex = { { p1x, p1y, 0.0f, 1.0f },{ quadColor.red, quadColor.green, quadColor.blue, quadColor.alpha } }; vertexBuffer.push_back(funcTempVertex);
				funcTempVertex = { { p2x, p2y, 0.0f, 1.0f },{ quadColor.red, quadColor.green, quadColor.blue, quadColor.alpha } }; vertexBuffer.push_back(funcTempVertex);
				funcTempVertex = { { p3x, p3y, 0.0f, 1.0f },{ quadColor.red, quadColor.green, quadColor.blue, quadColor.alpha } }; vertexBuffer.push_back(funcTempVertex);
				funcTempVertex = { { p4x, p4y, 0.0f, 1.0f },{ quadColor.red, quadColor.green, quadColor.blue, quadColor.alpha } }; vertexBuffer.push_back(funcTempVertex);

				indexBuffer.push_back((GLushort)(startIndex)); indexBuffer.push_back((GLushort)(startIndex + 1)); indexBuffer.push_back((GLushort)(startIndex + 3));
				indexBuffer.push_back((GLushort)(startIndex)); indexBuffer.push_back((GLushort)(startIndex + 2)); indexBuffer.push_back((GLushort)(startIndex + 3));
			};


			constexpr float kDurationGrandState0_bigMarkerAppear = 0.36f;
			constexpr float kDurationGrandState0_bigMarkerGo = 0.2f;

			constexpr float kDurationGrandState0_smallMarkerStartOffset = kDurationGrandState0_bigMarkerAppear + 0.1f;
			constexpr float kDurationGrandState0_smallMarkerAppear = 0.1f;
			constexpr float kDurationGrandState0_smallMarkerGo = 0.1f;

			constexpr float kDurationGrandState0 = kDurationGrandState0_smallMarkerStartOffset + kDurationGrandState0_smallMarkerAppear + kDurationGrandState0_smallMarkerGo;

			constexpr float kBigMarkerLocalRotationTime = 0.3f;
			constexpr float kBigMarkerStopAfterLocalRotationTime = 0.2f;
			constexpr float kBigMarkerBigRotationTime = 0.4f;
			constexpr float kBigMarkerStopAfterBigRotationTime = 0.5f;
			constexpr float kBigMarkerStepTime = kBigMarkerLocalRotationTime + kBigMarkerStopAfterLocalRotationTime + kBigMarkerBigRotationTime + kBigMarkerStopAfterBigRotationTime;
			constexpr float kBigMarkerState1ParamTotal = kBigMarkerStepTime * 8.0f;

			constexpr float kDurationGrandState2 = 0.8f;

			constexpr float kBigMarkerSize = 0.074f;
			constexpr float kBigMarkerSpaceRadius = 0.32f;
			constexpr GLubyte kBigMarkerFullAlpha = 96;
			
			constexpr float kSmallMarkerSize = 0.014f;
			constexpr GLubyte kSmallMarkerFullAlpha = 216;
			constexpr float kSmallMarkerAngularSpeed = 5.0f;

			constexpr float kSmallMarkerDisappearDistance = 0.12f;
		}


		//////////////////////////////////////////////////////////////////////////////// LoadingMarker

		//------------------------------------------------------------------------------
		LoadingMarker::LoadingMarker(Leggiero::Application::ApplicationComponent *appComponent, std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> colorShader)
			: m_appComponent(appComponent), m_colorShader(colorShader)
			, m_screenWidth(static_cast<float>(appComponent->AppInformation().GetPixelWidth())), m_screenHeight(static_cast<float>(appComponent->AppInformation().GetPixelHeight()))
			, m_defaultModelMat(1.0f)
			, m_screenCenter(static_cast<float>(appComponent->AppInformation().GetPixelWidth()) * 0.5f, static_cast<float>(appComponent->AppInformation().GetPixelHeight()) * 0.5f)
			, m_bigMarkerAppearSizeEasing(0.34, 1.56, 0.64, 1.0)
		{
			_InitializeCoordinateTransform();
			ResetState();
		}

		//------------------------------------------------------------------------------
		LoadingMarker::~LoadingMarker()
		{
		}

		//------------------------------------------------------------------------------
		void LoadingMarker::UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval)
		{
			float elapsedTimeInSec = (float)((float)std::chrono::duration_cast<std::chrono::microseconds>(frameInterval).count() / 1000000.0);
			float effectiveElapsedTimeInSec = elapsedTimeInSec;
			if (effectiveElapsedTimeInSec > 0.1f)
			{
				// Cap time to 100ms
				effectiveElapsedTimeInSec = 0.1f;
			}
			if (m_isVeryFirstFrame)
			{
				// for first frame, just assume a half frame in 60Hz
				effectiveElapsedTimeInSec = 0.008f;
				m_isVeryFirstFrame = false;
			}

			switch (m_grandState)
			{
				case 0:
					{
						m_inGrandStateDuration += effectiveElapsedTimeInSec;
						if (m_inGrandStateDuration >= _LoadingMarkerInternal::kDurationGrandState0)
						{
							m_grandState = 1;

							m_bigMarkerSubStateParam = Leggiero::Utility::Math::WrapMax(m_inGrandStateDuration - (_LoadingMarkerInternal::kDurationGrandState0_bigMarkerAppear + _LoadingMarkerInternal::kDurationGrandState0_bigMarkerGo), _LoadingMarkerInternal::kBigMarkerState1ParamTotal);
							m_smallMarkerAngle = Leggiero::Utility::Mathf::WrapAngle2Pi((m_inGrandStateDuration - (_LoadingMarkerInternal::kDurationGrandState0_smallMarkerStartOffset + _LoadingMarkerInternal::kDurationGrandState0_smallMarkerAppear)) * _LoadingMarkerInternal::kSmallMarkerAngularSpeed);

							m_bigMarkerSize = _LoadingMarkerInternal::kBigMarkerSize * m_screenScale;
							m_bigMarkerRadius = _LoadingMarkerInternal::kBigMarkerSpaceRadius * m_screenScale;
							m_bigMarkerAlpha = _LoadingMarkerInternal::kBigMarkerFullAlpha;
							_ProcessBigMarkerSubStateUpdate(false);

							m_smallMarkerSize = _LoadingMarkerInternal::kSmallMarkerSize * m_screenScale;
							m_smallMarkerRadius = _LoadingMarkerInternal::kSmallMarkerSize * m_screenScale * 2.0f;
							m_smallMarkerAlpha = _LoadingMarkerInternal::kSmallMarkerFullAlpha;
						}
						else
						{
							if (m_inGrandStateDuration < _LoadingMarkerInternal::kDurationGrandState0_bigMarkerAppear)
							{
								float param = Leggiero::Utility::Math::ParameterInInterval(m_inGrandStateDuration, 0.0f, _LoadingMarkerInternal::kDurationGrandState0_bigMarkerAppear);

								m_bigMarkerAngle = 0.0f;
								m_bigMarkerLocalAngle = 0.0f;
								m_bigMarkerSize = Leggiero::Utility::Math::LerpValue(0.0f, _LoadingMarkerInternal::kBigMarkerSize * m_screenScale, m_bigMarkerAppearSizeEasing.Easy(param));
								m_bigMarkerRadius = 0.0f;
								m_bigMarkerAlpha = _LoadingMarkerInternal::kBigMarkerFullAlpha;
							}
							else if (m_inGrandStateDuration < _LoadingMarkerInternal::kDurationGrandState0_bigMarkerAppear + _LoadingMarkerInternal::kDurationGrandState0_bigMarkerGo)
							{
								float param = (m_inGrandStateDuration - _LoadingMarkerInternal::kDurationGrandState0_bigMarkerAppear) / _LoadingMarkerInternal::kDurationGrandState0_bigMarkerGo;

								m_bigMarkerAngle = 0.0f;
								m_bigMarkerLocalAngle = 0.0f;
								m_bigMarkerSize = _LoadingMarkerInternal::kBigMarkerSize * m_screenScale;
								m_bigMarkerRadius = Leggiero::Utility::Math::LerpValue(0.0f, _LoadingMarkerInternal::kBigMarkerSpaceRadius * m_screenScale, CubicEaseOut(param));
								m_bigMarkerAlpha = _LoadingMarkerInternal::kBigMarkerFullAlpha;
							}
							else
							{
								m_bigMarkerSize = _LoadingMarkerInternal::kBigMarkerSize * m_screenScale;
								m_bigMarkerRadius = _LoadingMarkerInternal::kBigMarkerSpaceRadius * m_screenScale;
								m_bigMarkerAlpha = _LoadingMarkerInternal::kBigMarkerFullAlpha;

								m_bigMarkerSubStateParam = Leggiero::Utility::Math::WrapMax(m_inGrandStateDuration - (_LoadingMarkerInternal::kDurationGrandState0_bigMarkerAppear + _LoadingMarkerInternal::kDurationGrandState0_bigMarkerGo), _LoadingMarkerInternal::kBigMarkerState1ParamTotal);
								_ProcessBigMarkerSubStateUpdate(false);
							}

							if (m_inGrandStateDuration < _LoadingMarkerInternal::kDurationGrandState0_smallMarkerStartOffset)
							{
								m_smallMarkerAngle = 0.0f;
								m_smallMarkerSize = 0.0f;
								m_smallMarkerRadius = 0.0f;
								m_smallMarkerAlpha = 0;
							}
							else if (m_inGrandStateDuration < _LoadingMarkerInternal::kDurationGrandState0_smallMarkerStartOffset + _LoadingMarkerInternal::kDurationGrandState0_smallMarkerAppear)
							{
								float param = (m_inGrandStateDuration - _LoadingMarkerInternal::kDurationGrandState0_smallMarkerStartOffset) / _LoadingMarkerInternal::kDurationGrandState0_smallMarkerAppear;

								m_smallMarkerAngle = 0.0f;
								m_smallMarkerSize = Leggiero::Utility::Math::LerpValue(0.0f, _LoadingMarkerInternal::kSmallMarkerSize * m_screenScale, param);
								m_smallMarkerRadius = 0.0f;
								m_smallMarkerAlpha = _LoadingMarkerInternal::kSmallMarkerFullAlpha;
							}
							else
							{
								float inStateTime = m_inGrandStateDuration - (_LoadingMarkerInternal::kDurationGrandState0_smallMarkerStartOffset + _LoadingMarkerInternal::kDurationGrandState0_smallMarkerAppear);
								float param = inStateTime / _LoadingMarkerInternal::kDurationGrandState0_smallMarkerGo;

								m_smallMarkerAngle = Leggiero::Utility::Mathf::WrapAngle2Pi(inStateTime * _LoadingMarkerInternal::kSmallMarkerAngularSpeed);
								m_smallMarkerSize = _LoadingMarkerInternal::kSmallMarkerSize * m_screenScale;
								m_smallMarkerRadius = Leggiero::Utility::Math::LerpValue(0.0f, _LoadingMarkerInternal::kSmallMarkerSize * m_screenScale * 2.0f, param);
								m_smallMarkerAlpha = _LoadingMarkerInternal::kSmallMarkerFullAlpha;
							}
						}
					}
					break;

				case 1:
					{
						m_bigMarkerSize = _LoadingMarkerInternal::kBigMarkerSize * m_screenScale;
						m_bigMarkerRadius = _LoadingMarkerInternal::kBigMarkerSpaceRadius * m_screenScale;
						m_bigMarkerAlpha = _LoadingMarkerInternal::kBigMarkerFullAlpha;

						m_bigMarkerSubStateParam = Leggiero::Utility::Math::WrapMax(m_bigMarkerSubStateParam + effectiveElapsedTimeInSec, _LoadingMarkerInternal::kBigMarkerState1ParamTotal);
						if (!_ProcessBigMarkerSubStateUpdate(m_isFinishRequested, effectiveElapsedTimeInSec))
						{
							m_grandState = 2;
							m_inGrandStateDuration = 0.0f;
						}

						m_smallMarkerSize = _LoadingMarkerInternal::kSmallMarkerSize * m_screenScale;
						m_smallMarkerRadius = _LoadingMarkerInternal::kSmallMarkerSize * m_screenScale * 2.0f;
						m_smallMarkerAlpha = _LoadingMarkerInternal::kSmallMarkerFullAlpha;

						m_smallMarkerAngle = Leggiero::Utility::Mathf::WrapAngle2Pi(m_smallMarkerAngle + effectiveElapsedTimeInSec * _LoadingMarkerInternal::kSmallMarkerAngularSpeed);
					}
					break;

				case 2:
					{
						m_inGrandStateDuration += effectiveElapsedTimeInSec;
						if (m_inGrandStateDuration >= _LoadingMarkerInternal::kDurationGrandState2)
						{
							m_grandState = 3;

							m_bigMarkerAlpha = 0;
							m_smallMarkerAlpha = 0;
						}
						else
						{
							float param = m_inGrandStateDuration / _LoadingMarkerInternal::kDurationGrandState2;

							m_bigMarkerSize = _LoadingMarkerInternal::kBigMarkerSize * m_screenScale;
							m_bigMarkerAlpha = Leggiero::Utility::Math::LerpValue((int)_LoadingMarkerInternal::kBigMarkerFullAlpha, 0, param);

							int parkingRotationNumber = m_bigMarkerParkingIndex / 2;
							int parkingToRotationNumber = (parkingRotationNumber + 2) % 8;
							float bigMarkerFromAngle = Leggiero::Utility::Mathf::kPiOver4 * parkingRotationNumber;
							float bigMarkerToAngle = Leggiero::Utility::Mathf::kPiOver4 * parkingToRotationNumber;

							Leggiero::Utility::Math::Vector2D<float> markerAxisFrom(cos(-bigMarkerFromAngle), sin(-bigMarkerFromAngle));
							Leggiero::Utility::Math::Vector2D<float> markerAxisTo(cos(-bigMarkerToAngle), sin(-bigMarkerToAngle));
							Leggiero::Utility::Math::Vector2D<float> markerNow = Leggiero::Utility::Math::Vector2D<float>::Lerp(_LoadingMarkerInternal::kBigMarkerSpaceRadius * m_screenScale * markerAxisFrom, _LoadingMarkerInternal::kBigMarkerSpaceRadius * m_screenScale * markerAxisTo, ExponentialEaseIn(param));
							m_bigMarkerAngle = Leggiero::Utility::Mathf::AzimuthPi(markerNow.x, markerNow.y);
							m_bigMarkerRadius = markerNow.Length();

							m_bigMarkerLocalAngle = Leggiero::Utility::Mathf::kPiOver4 * (parkingRotationNumber + (m_bigMarkerParkingIndex % 2)) + (m_bigMarkerAngle - bigMarkerFromAngle);

							m_smallMarkerSize = _LoadingMarkerInternal::kSmallMarkerSize * m_screenScale;
							m_smallMarkerRadius = Leggiero::Utility::Math::LerpValue(_LoadingMarkerInternal::kSmallMarkerSize * m_screenScale * 2.0f, _LoadingMarkerInternal::kSmallMarkerDisappearDistance * m_screenScale, ExponentialEaseIn(param));
							m_smallMarkerAlpha = Leggiero::Utility::Math::LerpValue((int)_LoadingMarkerInternal::kSmallMarkerFullAlpha, 0, param);
							m_smallMarkerAngle = Leggiero::Utility::Mathf::WrapAngle2Pi(m_smallMarkerAngle + effectiveElapsedTimeInSec * _LoadingMarkerInternal::kSmallMarkerAngularSpeed);
						}
					}
					break;

				default:
					{
						m_bigMarkerAlpha = 0;
						m_smallMarkerAlpha = 0;
					}
					break;
			}
		}

		//------------------------------------------------------------------------------
		void LoadingMarker::Render()
		{
			// Collect Shapes
			if (m_bigMarkerAlpha > 0 && m_bigMarkerSize > 0.1f)
			{
				Leggiero::Utility::Math::Vector2D<float> markerAxisX(cos(-m_bigMarkerAngle), sin(-m_bigMarkerAngle));
				Leggiero::Utility::Math::Vector2D<float> markerAxisY(markerAxisX.y, -markerAxisX.x);

				Leggiero::Utility::Math::Vector2D<float> markerLocalAxisX(cos(-m_bigMarkerAngle + m_bigMarkerLocalAngle), sin(-m_bigMarkerAngle + m_bigMarkerLocalAngle));
				Leggiero::Utility::Math::Vector2D<float> markerLocalAxisY(markerLocalAxisX.y, -markerLocalAxisX.x);

				Leggiero::Utility::Math::Vector2D<float> marker1Center = m_screenCenter + m_bigMarkerRadius * markerAxisX;
				Leggiero::Utility::Math::Vector2D<float> markerLT = marker1Center - m_bigMarkerSize * markerLocalAxisX;
				Leggiero::Utility::Math::Vector2D<float> markerRT = marker1Center - m_bigMarkerSize * markerLocalAxisY;
				Leggiero::Utility::Math::Vector2D<float> markerLB = marker1Center + m_bigMarkerSize * markerLocalAxisY;
				Leggiero::Utility::Math::Vector2D<float> markerRB = marker1Center + m_bigMarkerSize * markerLocalAxisX;
				_LoadingMarkerInternal::_PushSingleColoredQuadToVBV(m_vertexBuffer, m_indexBuffer, Leggiero::Graphics::GLByteARGB(m_bigMarkerAlpha, 255, 255, 255), 
					markerLT.x, markerLT.y, markerRT.x, markerRT.y, markerLB.x, markerLB.y, markerRB.x, markerRB.y);

				Leggiero::Utility::Math::Vector2D<float> marker2Center = m_screenCenter - m_bigMarkerRadius * markerAxisX;
				markerLT = marker2Center - m_bigMarkerSize * markerLocalAxisX;
				markerRT = marker2Center - m_bigMarkerSize * markerLocalAxisY;
				markerLB = marker2Center + m_bigMarkerSize * markerLocalAxisY;
				markerRB = marker2Center + m_bigMarkerSize * markerLocalAxisX;
				_LoadingMarkerInternal::_PushSingleColoredQuadToVBV(m_vertexBuffer, m_indexBuffer, Leggiero::Graphics::GLByteARGB(m_bigMarkerAlpha, 255, 255, 255),
					markerLT.x, markerLT.y, markerRT.x, markerRT.y, markerLB.x, markerLB.y, markerRB.x, markerRB.y);

				Leggiero::Utility::Math::Vector2D<float> marker3Center = m_screenCenter + m_bigMarkerRadius * markerAxisY;
				markerLT = marker3Center - m_bigMarkerSize * markerLocalAxisX;
				markerRT = marker3Center - m_bigMarkerSize * markerLocalAxisY;
				markerLB = marker3Center + m_bigMarkerSize * markerLocalAxisY;
				markerRB = marker3Center + m_bigMarkerSize * markerLocalAxisX;
				_LoadingMarkerInternal::_PushSingleColoredQuadToVBV(m_vertexBuffer, m_indexBuffer, Leggiero::Graphics::GLByteARGB(m_bigMarkerAlpha, 255, 255, 255),
					markerLT.x, markerLT.y, markerRT.x, markerRT.y, markerLB.x, markerLB.y, markerRB.x, markerRB.y);

				Leggiero::Utility::Math::Vector2D<float> marker4Center = m_screenCenter - m_bigMarkerRadius * markerAxisY;
				markerLT = marker4Center - m_bigMarkerSize * markerLocalAxisX;
				markerRT = marker4Center - m_bigMarkerSize * markerLocalAxisY;
				markerLB = marker4Center + m_bigMarkerSize * markerLocalAxisY;
				markerRB = marker4Center + m_bigMarkerSize * markerLocalAxisX;
				_LoadingMarkerInternal::_PushSingleColoredQuadToVBV(m_vertexBuffer, m_indexBuffer, Leggiero::Graphics::GLByteARGB(m_bigMarkerAlpha, 255, 255, 255),
					markerLT.x, markerLT.y, markerRT.x, markerRT.y, markerLB.x, markerLB.y, markerRB.x, markerRB.y);
			}
			if (m_smallMarkerAlpha > 0 && m_smallMarkerSize > 0.1f)
			{
				Leggiero::Utility::Math::Vector2D<float> markerAxisX(cos(m_smallMarkerAngle), sin(m_smallMarkerAngle));
				Leggiero::Utility::Math::Vector2D<float> markerAxisY(markerAxisX.y, -markerAxisX.x);

				Leggiero::Utility::Math::Vector2D<float> marker1Center = m_screenCenter + m_smallMarkerRadius * markerAxisX;
				Leggiero::Utility::Math::Vector2D<float> markerLT = marker1Center - m_smallMarkerSize * markerAxisX - m_smallMarkerSize * markerAxisY;
				Leggiero::Utility::Math::Vector2D<float> markerRT = marker1Center + m_smallMarkerSize * markerAxisX - m_smallMarkerSize * markerAxisY;
				Leggiero::Utility::Math::Vector2D<float> markerLB = marker1Center - m_smallMarkerSize * markerAxisX + m_smallMarkerSize * markerAxisY;
				Leggiero::Utility::Math::Vector2D<float> markerRB = marker1Center + m_smallMarkerSize * markerAxisX + m_smallMarkerSize * markerAxisY;
				_LoadingMarkerInternal::_PushSingleColoredQuadToVBV(m_vertexBuffer, m_indexBuffer, Leggiero::Graphics::GLByteARGB(m_smallMarkerAlpha, 255, 255, 255),
					markerLT.x, markerLT.y, markerRT.x, markerRT.y, markerLB.x, markerLB.y, markerRB.x, markerRB.y);

				Leggiero::Utility::Math::Vector2D<float> marker2Center = m_screenCenter - m_smallMarkerRadius * markerAxisX;
				markerLT = marker2Center - m_smallMarkerSize * markerAxisX - m_smallMarkerSize * markerAxisY;
				markerRT = marker2Center + m_smallMarkerSize * markerAxisX - m_smallMarkerSize * markerAxisY;
				markerLB = marker2Center - m_smallMarkerSize * markerAxisX + m_smallMarkerSize * markerAxisY;
				markerRB = marker2Center + m_smallMarkerSize * markerAxisX + m_smallMarkerSize * markerAxisY;
				_LoadingMarkerInternal::_PushSingleColoredQuadToVBV(m_vertexBuffer, m_indexBuffer, Leggiero::Graphics::GLByteARGB(m_smallMarkerAlpha, 255, 255, 255),
					markerLT.x, markerLT.y, markerRT.x, markerRT.y, markerLB.x, markerLB.y, markerRB.x, markerRB.y);

				Leggiero::Utility::Math::Vector2D<float> marker3Center = m_screenCenter + m_smallMarkerRadius * markerAxisY;
				markerLT = marker3Center - m_smallMarkerSize * markerAxisX - m_smallMarkerSize * markerAxisY;
				markerRT = marker3Center + m_smallMarkerSize * markerAxisX - m_smallMarkerSize * markerAxisY;
				markerLB = marker3Center - m_smallMarkerSize * markerAxisX + m_smallMarkerSize * markerAxisY;
				markerRB = marker3Center + m_smallMarkerSize * markerAxisX + m_smallMarkerSize * markerAxisY;
				_LoadingMarkerInternal::_PushSingleColoredQuadToVBV(m_vertexBuffer, m_indexBuffer, Leggiero::Graphics::GLByteARGB(m_smallMarkerAlpha, 255, 255, 255),
					markerLT.x, markerLT.y, markerRT.x, markerRT.y, markerLB.x, markerLB.y, markerRB.x, markerRB.y);

				Leggiero::Utility::Math::Vector2D<float> marker4Center = m_screenCenter - m_smallMarkerRadius * markerAxisY;
				markerLT = marker4Center - m_smallMarkerSize * markerAxisX - m_smallMarkerSize * markerAxisY;
				markerRT = marker4Center + m_smallMarkerSize * markerAxisX - m_smallMarkerSize * markerAxisY;
				markerLB = marker4Center - m_smallMarkerSize * markerAxisX + m_smallMarkerSize * markerAxisY;
				markerRB = marker4Center + m_smallMarkerSize * markerAxisX + m_smallMarkerSize * markerAxisY;
				_LoadingMarkerInternal::_PushSingleColoredQuadToVBV(m_vertexBuffer, m_indexBuffer, Leggiero::Graphics::GLByteARGB(m_smallMarkerAlpha, 255, 255, 255),
					markerLT.x, markerLT.y, markerRT.x, markerRT.y, markerLB.x, markerLB.y, markerRB.x, markerRB.y);
			}

			// Render Buffer
			if (m_indexBuffer.size() > 3)
			{
				m_colorShader->Use();

				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);

				glUniformMatrix4fv(m_colorShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_projMat));
				glUniformMatrix4fv(m_colorShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_viewMat));
				glUniformMatrix4fv(m_colorShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(m_defaultModelMat));

				auto vertexAttribEnabledContext = m_colorShader->EnableUsingVertexAttribArray(true);
				Leggiero::Graphics::ColoredVertex::SetGLVertexAttribPointer(m_colorShader->GetPositionSlot(), m_colorShader->GetColorSlot(), &m_vertexBuffer[0]);
				glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);
			}

			m_vertexBuffer.clear();
			m_indexBuffer.clear();
		}

		//------------------------------------------------------------------------------
		void LoadingMarker::ResetState()
		{
			m_grandState = 0;
			m_inGrandStateDuration = 0.0f;

			m_isVeryFirstFrame = true;
			m_isFinishRequested = false;

			m_bigMarkerAngle = 0.0f;
			m_bigMarkerLocalAngle = 0.0f;
			m_bigMarkerSize = 0.0f;
			m_bigMarkerRadius = 0.0f;
			m_bigMarkerAlpha = 0;

			m_smallMarkerAngle = 0.0f;
			m_smallMarkerSize = 0.0f;
			m_smallMarkerRadius = 0.0f;
			m_smallMarkerAlpha = 0;

			m_bigMarkerParkingIndex = 0;
		}

		//------------------------------------------------------------------------------
		void LoadingMarker::NotifyLoadFinished()
		{
			m_isFinishRequested = true;
		}

		//------------------------------------------------------------------------------
		bool LoadingMarker::IsMarkerFinishing()
		{
			return (m_grandState >= 2);
		}

		//------------------------------------------------------------------------------
		bool LoadingMarker::IsMarkerFinished()
		{
			return (m_grandState == 3);
		}

		//------------------------------------------------------------------------------
		void LoadingMarker::_InitializeCoordinateTransform()
		{
			m_projMat = glm::ortho(0.0f, m_screenWidth, -m_screenHeight, 0.0f, -1.0f, 1.0f);

			glm::vec3 eyePos; eyePos[0] = 0.0f; eyePos[1] = 0.0f; eyePos[2] = -1.0f;
			glm::vec3 eyeSee; eyeSee[0] = eyePos[0]; eyeSee[1] = eyePos[1]; eyeSee[2] = 0.0f;
			glm::vec3 eyeUp; eyeUp[0] = 0.0f; eyeUp[1] = -1.0f; eyeUp[2] = 0.0f;
			m_viewMat = glm::lookAt(eyePos, eyeSee, eyeUp);

			if (m_screenWidth < m_screenHeight)
			{
				m_screenScale = m_screenWidth;
			}
			else
			{
				m_screenScale = m_screenHeight;
			}
		}

		//------------------------------------------------------------------------------
		bool LoadingMarker::_ProcessBigMarkerSubStateUpdate(bool isParkingNeeded, float currentStepElapsed)
		{
			bool isProcessed = false;
			bool isParked = false;
			for (int i = 0; i < 8; ++i)
			{
				if (m_bigMarkerSubStateParam <= _LoadingMarkerInternal::kBigMarkerStepTime * (i + 1))
				{
					float timeInStep = m_bigMarkerSubStateParam - _LoadingMarkerInternal::kBigMarkerStepTime * i;
					if (timeInStep < _LoadingMarkerInternal::kBigMarkerLocalRotationTime)
					{
						if (isParkingNeeded && timeInStep < currentStepElapsed)
						{
							m_bigMarkerAngle = Leggiero::Utility::Mathf::kPiOver4 * i;
							m_bigMarkerLocalAngle = Leggiero::Utility::Mathf::kPiOver4 * i;
							isParked = true;
							m_bigMarkerParkingIndex = i * 2;
						}
						else
						{
							m_bigMarkerAngle = Leggiero::Utility::Mathf::kPiOver4 * i;
							m_bigMarkerLocalAngle = Leggiero::Utility::Mathf::kPiOver4 * i + Leggiero::Utility::Mathf::kPiOver4 * (timeInStep / _LoadingMarkerInternal::kBigMarkerLocalRotationTime);
						}
					}
					else if(timeInStep < _LoadingMarkerInternal::kBigMarkerLocalRotationTime + _LoadingMarkerInternal::kBigMarkerStopAfterLocalRotationTime)
					{
						m_bigMarkerAngle = Leggiero::Utility::Mathf::kPiOver4 * i;
						m_bigMarkerLocalAngle = Leggiero::Utility::Mathf::kPiOver4 * (i + 1);
						isParked = true;
						m_bigMarkerParkingIndex = i * 2 + 1;
					}
					else if (timeInStep < _LoadingMarkerInternal::kBigMarkerLocalRotationTime + _LoadingMarkerInternal::kBigMarkerStopAfterLocalRotationTime + _LoadingMarkerInternal::kBigMarkerBigRotationTime)
					{
						float subStepTime = timeInStep - (_LoadingMarkerInternal::kBigMarkerLocalRotationTime + _LoadingMarkerInternal::kBigMarkerStopAfterLocalRotationTime);
						if (isParkingNeeded && subStepTime < currentStepElapsed)
						{
							m_bigMarkerAngle = Leggiero::Utility::Mathf::kPiOver4 * i;
							m_bigMarkerLocalAngle = Leggiero::Utility::Mathf::kPiOver4 * (i + 1);
							isParked = true;
							m_bigMarkerParkingIndex = i * 2 + 1;
						}
						else
						{
							m_bigMarkerAngle = Leggiero::Utility::Mathf::kPiOver4 * i + Leggiero::Utility::Mathf::kPiOver4 * (subStepTime / _LoadingMarkerInternal::kBigMarkerBigRotationTime);
							m_bigMarkerLocalAngle = Leggiero::Utility::Mathf::kPiOver4 * (i + 1);
						}
					}
					else
					{
						m_bigMarkerAngle = Leggiero::Utility::Mathf::kPiOver4 * (i + 1);
						m_bigMarkerLocalAngle = Leggiero::Utility::Mathf::kPiOver4 * (i + 1);
						isParked = true;
						m_bigMarkerParkingIndex = (i + 1) * 2;
					}
					isProcessed = true;
					break;
				}
			}

			if (!isProcessed)
			{
				// Might be occured due to floating point error
				m_bigMarkerAngle = 0.0f;
				m_bigMarkerLocalAngle = 0.0f;
				isParked = true;
				m_bigMarkerParkingIndex = 0;
			}

			if (isParkingNeeded)
			{
				if (isParked)
				{
					return false;
				}
			}

			return true;
		}
	}
}
