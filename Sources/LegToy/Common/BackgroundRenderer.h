////////////////////////////////////////////////////////////////////////////////
// Common/BackgroundRenderer.h (LegToy - LegToy)
//
// Inter-scene common background renderer
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__COMMON__BACKGROUND_RENDERER_H
#define __LEGTOY__COMMON__BACKGROUND_RENDERER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <vector>

// External Library
#include "GLES3.h"
#include <glm/glm.hpp>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Graphics
#include <Graphics/Shader/CommonGLVertexType.h>
#include <Graphics/Common/GLColor.h>


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
	// LegToy common background renderer
	class CommonBackgroundRenderer
		: public Leggiero::Application::IGraphicEventObserver
	{
	public:
		CommonBackgroundRenderer(Leggiero::Application::ApplicationComponent *appComponent, std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> colorShader);
		virtual ~CommonBackgroundRenderer();

	public:
		void UpdateFrame(Leggiero::GameTimeClockType::duration frameInterval);
		void Render();

		void StartToShowcaseScene();

	public:	// Leggiero::Application::IGraphicEventObserver
		virtual void OnScreenSizeChanged(Leggiero::Application::DeviceScreenCoordType width, Leggiero::Application::DeviceScreenCoordType height) override;

	protected:
		void _InitializeCoordinateTransform();

	protected:
		Leggiero::Application::ApplicationComponent				*m_appComponent;
		std::shared_ptr<Leggiero::Graphics::ColorSimpleShader>	m_colorShader;

		int m_state;
		float m_stateTransitionDuration;

		float m_screenWidth;
		float m_screenHeight;

		glm::mat4 m_projMat;
		glm::mat4 m_viewMat;
		glm::mat4 m_defaultModelMat;

		Leggiero::Graphics::GLByteRGB m_bgColorLT;
		Leggiero::Graphics::GLByteRGB m_bgColorRT;
		Leggiero::Graphics::GLByteRGB m_bgColorLB;
		Leggiero::Graphics::GLByteRGB m_bgColorRB;

		std::vector<Leggiero::Graphics::ColoredVertex>	m_vertexBuffer;
		std::vector<GLushort>							m_indexBuffer;
	};
}

#endif
