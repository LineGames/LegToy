////////////////////////////////////////////////////////////////////////////////
// Common/IResource_CommonRendering.h (LegToy - LegToy)
//
// Common Rendering Shared Resources Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__COMMON__I_RESOURCE_COMMON_RENDERING_H
#define __LEGTOY__COMMON__I_RESOURCE_COMMON_RENDERING_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>


// Forward Declarations
namespace Leggiero
{
	namespace Graphics
	{
		class ColorSimpleShader;
		class TexturedColorSimpleShader;
	}
}


namespace LegToy
{
	// Forward Declarations
	class CommonBackgroundRenderer;


	namespace SharedResource
	{
		// Common Rendering Resources Interface
		class ICommonRenderingResources
		{
		public:
			virtual ~ICommonRenderingResources() { }

		public:
			virtual std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> GetCommonShader_ColorSimple() = 0;
			virtual std::shared_ptr<Leggiero::Graphics::TexturedColorSimpleShader> GetCommonShader_TextureSimple() = 0;

			virtual std::shared_ptr<CommonBackgroundRenderer> GetCommonBackgroundRenderer() = 0;
		};
	}
}

#endif
