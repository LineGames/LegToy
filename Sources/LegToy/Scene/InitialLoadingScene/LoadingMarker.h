////////////////////////////////////////////////////////////////////////////////
// Scene/InitialLoadingScene/LoadingMarker.h (LegToy - LegToy)
//
// Loading Marker in Initial Loading Scene
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__INITIAL_LOADING_SCENE__LOADING_MARKER_H
#define __LEGTOY__SCENE__INITIAL_LOADING_SCENE__LOADING_MARKER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <vector>

// External Library
#include "GLES3.h"
#include <glm/glm.hpp>

// Leggiero.Utility
#include <Utility/Math/Easing.h>
#include <Utility/Math/Vector.h>

// Leggiero.Graphics
#include <Graphics/Shader/CommonGLVertexType.h>


// Forward Declarations
namespace Leggiero
{
	namespace Application
	{
		class ApplicationComponent;
	}

	namespace Graphics
	{
		class ColorSimpleShader;
	}
}


namespace LegToy
{
	namespace InitialLoadingSceneStuffs
	{
		// Loading Marker object in Initial Loading Scene
		class LoadingMarker
		{
		public:
			LoadingMarker(Leggiero::Application::ApplicationComponent *appComponent, std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> colorShader);
			virtual ~LoadingMarker();

		public:
			void UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval);
			void Render();

			void ResetState();

			void NotifyLoadFinished();
			bool IsMarkerFinishing();
			bool IsMarkerFinished();

		protected:
			void _InitializeCoordinateTransform();
			bool _ProcessBigMarkerSubStateUpdate(bool isParkingNeeded, float currentStepElapsed = 0.0f);

		protected:
			Leggiero::Application::ApplicationComponent *m_appComponent;
			std::shared_ptr<Leggiero::Graphics::ColorSimpleShader>	m_colorShader;

			float m_screenWidth;
			float m_screenHeight;

			glm::mat4 m_projMat;
			glm::mat4 m_viewMat;
			glm::mat4 m_defaultModelMat;

			std::vector<Leggiero::Graphics::ColoredVertex>	m_vertexBuffer;
			std::vector<GLushort>							m_indexBuffer;

			float m_screenScale;
			Leggiero::Utility::Math::Vector2D<float> m_screenCenter;

			int m_grandState;
			float m_inGrandStateDuration;

			bool m_isVeryFirstFrame;
			bool m_isFinishRequested;

			float m_bigMarkerAngle;
			float m_bigMarkerLocalAngle;
			float m_bigMarkerSize;
			float m_bigMarkerRadius;
			int	m_bigMarkerAlpha;

			float m_smallMarkerAngle;
			float m_smallMarkerSize;
			float m_smallMarkerRadius;
			int	m_smallMarkerAlpha;

			float m_bigMarkerSubStateParam;
			int m_bigMarkerParkingIndex;

			Leggiero::Utility::Math::CubicBezierEasing m_bigMarkerAppearSizeEasing;
		};
	}
}

#endif
