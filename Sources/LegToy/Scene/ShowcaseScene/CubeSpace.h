////////////////////////////////////////////////////////////////////////////////
// Scene/ShowcaseScene/CubeSpace.h (LegToy - LegToy)
//
// Cube rotating space processor
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SHOWCASE_SCENE__CUBE_SPACE_H
#define __LEGTOY__SCENE__SHOWCASE_SCENE__CUBE_SPACE_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// External Library
#include <glm/glm.hpp>

// Leggiero.Utility
#include <Utility/Math/SimpleGeometry.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>


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
		class ColorSimpleShader;
		class TexturedColorSimpleShader;
		class GLTextureResource;
	}
}


namespace LegToy
{
	namespace ShowcaseSceneStuffs
	{
		// Cube rotating space processor
		// Should be created in graphic thread
		class CubeSpace
			: public Leggiero::Application::IGraphicEventObserver
		{
		public:
			constexpr static float kDefaultReferenceFov = Leggiero::Utility::Mathf::kPiOver6;
			constexpr static float kDefaultSeeDistance = 4.0f;

		public:
			CubeSpace(Leggiero::Application::ApplicationComponent *appComponent, Leggiero::Graphics::GraphicResourceManagerComponent *resourceManager,
				std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> colorShader, std::shared_ptr<Leggiero::Graphics::TexturedColorSimpleShader> textureShader, 
				float referenceFov = kDefaultReferenceFov, float seeDistance = kDefaultSeeDistance);
			virtual ~CubeSpace();

		public:	// Leggiero::Application::IGraphicEventObserver
			virtual void OnScreenSizeChanged(Leggiero::Application::DeviceScreenCoordType width, Leggiero::Application::DeviceScreenCoordType height) override;

		public:
			void UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval);
			void Render();

			void ForceSetSeeAzimuth(float azimuth);
			void StartToSeeAzimuth(float azimuth, float timeConstant = 1.0f);
			float GetCurrentSeeAzimuth() const { return m_eyeSeeAzimuth; }

			float CalculateSeeAzimuth(float screenXPos);

		protected:
			void _SetScreenSize(float width, float height);

		protected:
			Leggiero::Application::ApplicationComponent *m_appComponent;

			std::shared_ptr<Leggiero::Graphics::ColorSimpleShader>	m_colorShader;
			std::shared_ptr<Leggiero::Graphics::TexturedColorSimpleShader>	m_textureShader;

			std::shared_ptr<Leggiero::Graphics::GLTextureResource> m_testTexture;

			float m_screenWidth;
			float m_screenHeight;

			float m_referenceFov;
			float m_seeDistance;

			float m_screenXFov;
			float m_screenCoordinateConstant;
			float m_seeMovingEpslion;

			glm::mat4 m_projMat;
			glm::mat4 m_viewMat;
			glm::mat4 m_cubeModelMat;

			float m_cubeRotationAngle;
			float m_eyeSeeAzimuth;

			float m_seeMovingTimeConstant;
			float m_seeMovingStartAzimuth;
			float m_seeMovingTargetAzimuth;
			float m_seeMovingAcuumulatedTime;
			bool m_isSeeMoving;
		};
	}
}

#endif
