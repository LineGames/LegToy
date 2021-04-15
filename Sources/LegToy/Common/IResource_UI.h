////////////////////////////////////////////////////////////////////////////////
// Common/IResource_UI.h (LegToy - LegToy)
//
// UI Shared Resources Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__COMMON__I_RESOURCE_UI_H
#define __LEGTOY__COMMON__I_RESOURCE_UI_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>


// Forward Declarations
namespace Leggiero
{
	namespace LUI
	{
		class UIManager;

		namespace Description
		{
			class DescriptionManager;
		}

		namespace Shaders
		{
			class UITextureColorShader;
			class UITextureBlurShader;
		}
	}
}


namespace LegToy
{
	namespace SharedResource
	{
		// UI Interface
		class IUIResources
		{
		public:
			virtual ~IUIResources() { }

		public:
			virtual std::shared_ptr<Leggiero::LUI::UIManager> GetSharedUIManager() = 0;
			virtual std::shared_ptr<Leggiero::LUI::Description::DescriptionManager> GetBaseUIDescriptionManager() = 0;

			virtual std::shared_ptr<Leggiero::LUI::Shaders::UITextureColorShader> GetCommonTextureColorShader() = 0;
			virtual std::shared_ptr<Leggiero::LUI::Shaders::UITextureBlurShader> GetCommonTextureBlurShader() = 0;
		};
	}
}

#endif
