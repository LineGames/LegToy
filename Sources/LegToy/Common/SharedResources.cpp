////////////////////////////////////////////////////////////////////////////////
// Common/SharedResources.cpp (LegToy - LegToy)
//
// LegToy Game Shared Resources Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "SharedResources.h"

// Leggiero.Application
#include <Application/ApplicationComponent.h>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>
#include <Graphics/Shader/Basic/ColorSimpleShader.h>
#include <Graphics/Shader/Basic/TexturedColorSimpleShader.h>

// Leggiero.Font
#include <Font/GlyphManagerComponent.h>

// Leggiero.LegacyUI
#include <LUI/UIManager.h>
#include <LUI/Description/UIDescriptionManager.h>
#include <LUI/Description/UIDescriptionSystemUnit.h>
#include <LUI/Loader/UIBundleAssetLoader.h>
#include <LUI/Rendering/UITextureManager.h>
#include <LUI/Rendering/UIFontFaceManager.h>
#include <LUI/Rendering/UIShaders.h>

// LegToy
#include "../Game.h"
#include "BackgroundRenderer.h"

// LegToy - Shared Resources
#include "IResource_CommonRendering.h"
#include "IResource_UI.h"


namespace LegToy
{
	//////////////////////////////////////////////////////////////////////////////// ConcreteSharedResources

	//------------------------------------------------------------------------------
	// Concrete Game Shared Resources that contains real resources
	// Separated resources using interfaces in concrete object to reduce build dependency
	class ConcreteSharedResources
		: public SharedResources
		, public SharedResource::ICommonRenderingResources
		, public SharedResource::IUIResources
	{
	public:
		ConcreteSharedResources();
		virtual ~ConcreteSharedResources();

	public:
		virtual void PrepareResources(Game *pGame) override;
		virtual void GraphicPrepareResources(Game *pGame) override;

		virtual void GraphicShutdownResources(Game *pGame) override;
		virtual void ShutdownResources(Game *pGame) override;

	public:	// ICommonRenderingResources
		void CommonRenderingResources_PrepareResources(Game *pGame);
		void CommonRenderingResources_GraphicPrepareResources(Game *pGame);

		void CommonRenderingResources_GraphicShutdownResources(Game *pGame);
		void CommonRenderingResources_ShutdownResources(Game *pGame);

		virtual std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> GetCommonShader_ColorSimple() override { return m_commonShader_colorSimple; }
		virtual std::shared_ptr<Leggiero::Graphics::TexturedColorSimpleShader> GetCommonShader_TextureSimple() override { return m_commonShader_textureSimple; }

		virtual std::shared_ptr<CommonBackgroundRenderer> GetCommonBackgroundRenderer() override { return m_commonBackgroundRenderer; }

		std::shared_ptr<Leggiero::Graphics::ColorSimpleShader> m_commonShader_colorSimple;
		std::shared_ptr<Leggiero::Graphics::TexturedColorSimpleShader> m_commonShader_textureSimple;

		std::shared_ptr<CommonBackgroundRenderer> m_commonBackgroundRenderer;

	public:	// IUIResources
		void UIResources_PrepareResources(Game *pGame);
		void UIResources_GraphicPrepareResources(Game *pGame);

		void UIResources_GraphicShutdownResources(Game *pGame);
		void UIResources_ShutdownResources(Game *pGame);

		virtual std::shared_ptr<Leggiero::LUI::UIManager> GetSharedUIManager() override { return m_sharedUIManager; }
		virtual std::shared_ptr<Leggiero::LUI::Description::DescriptionManager> GetBaseUIDescriptionManager() override { return m_baseUIDescriptionManager; }

		virtual std::shared_ptr<Leggiero::LUI::Shaders::UITextureColorShader> GetCommonTextureColorShader() override { return m_commonUITextureColorShader; }
		virtual std::shared_ptr<Leggiero::LUI::Shaders::UITextureBlurShader> GetCommonTextureBlurShader() override { return m_commonUITextureBlurShader; }

		std::shared_ptr<Leggiero::LUI::UIManager> m_sharedUIManager;
		std::shared_ptr<Leggiero::LUI::Description::DescriptionManager> m_baseUIDescriptionManager;

		std::shared_ptr<Leggiero::LUI::UIBundleAssetLoader> m_baseUIBundleLoader;
		std::shared_ptr<Leggiero::LUI::UITextureManager> m_baseUITextureManager;
		std::shared_ptr<Leggiero::LUI::UIFontFaceManager> m_baseUIFontFaceManager;

		std::shared_ptr<Leggiero::LUI::Description::SystemUnit> m_baseUIDescriptionSystemUnit;

		std::shared_ptr<Leggiero::LUI::Shaders::UITextureColorShader> m_commonUITextureColorShader;
		std::shared_ptr<Leggiero::LUI::Shaders::UITextureBlurShader> m_commonUITextureBlurShader;

	protected:

	};

	//------------------------------------------------------------------------------
	ConcreteSharedResources::ConcreteSharedResources()
	{

	}

	//------------------------------------------------------------------------------
	ConcreteSharedResources::~ConcreteSharedResources()
	{

	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::PrepareResources(Game *pGame)
	{
		CommonRenderingResources_PrepareResources(pGame);
		UIResources_PrepareResources(pGame);
	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::GraphicPrepareResources(Game *pGame)
	{
		CommonRenderingResources_GraphicPrepareResources(pGame);
		UIResources_GraphicPrepareResources(pGame);
	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::GraphicShutdownResources(Game *pGame)
	{
		UIResources_GraphicShutdownResources(pGame);
		CommonRenderingResources_GraphicShutdownResources(pGame);
	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::ShutdownResources(Game *pGame)
	{
		UIResources_ShutdownResources(pGame);
		CommonRenderingResources_ShutdownResources(pGame);
	}


	//////////////////////////////////////////////////////////////////////////////// ConcreteSharedResources - ICommonRenderingResources

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::CommonRenderingResources_PrepareResources(Game *pGame)
	{

	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::CommonRenderingResources_GraphicPrepareResources(Game *pGame)
	{
		Leggiero::Graphics::GraphicResourceManagerComponent *graphicResourceManagerComponent = pGame->GetComponent<Leggiero::Graphics::GraphicResourceManagerComponent>();

		m_commonShader_colorSimple = Leggiero::Graphics::ColorSimpleShader::Create(graphicResourceManagerComponent);
		m_commonShader_textureSimple = Leggiero::Graphics::TexturedColorSimpleShader::Create(graphicResourceManagerComponent);

		m_commonBackgroundRenderer = std::make_shared<CommonBackgroundRenderer>(pGame->GetComponent<Leggiero::Application::ApplicationComponent>(), m_commonShader_colorSimple);
	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::CommonRenderingResources_GraphicShutdownResources(Game *pGame)
	{
		m_commonBackgroundRenderer.reset();

		m_commonShader_colorSimple.reset();
		m_commonShader_textureSimple.reset();
	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::CommonRenderingResources_ShutdownResources(Game *pGame)
	{

	}


	//////////////////////////////////////////////////////////////////////////////// ConcreteSharedResources - IUIResources

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::UIResources_PrepareResources(Game *pGame)
	{

	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::UIResources_GraphicPrepareResources(Game *pGame)
	{
		Leggiero::Graphics::GraphicResourceManagerComponent *graphicResourceManagerComponent = pGame->GetComponent<Leggiero::Graphics::GraphicResourceManagerComponent>();

		m_commonUITextureColorShader = Leggiero::LUI::Shaders::UITextureColorShader::Create(graphicResourceManagerComponent);
		m_commonUITextureBlurShader = Leggiero::LUI::Shaders::UITextureBlurShader::Create(graphicResourceManagerComponent);

		m_sharedUIManager = std::make_shared<Leggiero::LUI::UIManager>();
		m_sharedUIManager->Initialize(
			pGame->GetComponent<Leggiero::Application::ApplicationComponent>(),
			graphicResourceManagerComponent
		);

		m_baseUIBundleLoader = std::make_shared<Leggiero::LUI::UIBundleAssetLoader>(
			pGame->GetComponent<Leggiero::FileSystem::BundleFileResourceComponent>(), 
			graphicResourceManagerComponent,
			"UI"
			);
		m_baseUITextureManager = std::make_shared<Leggiero::LUI::UITextureManager>(*m_baseUIBundleLoader);
		m_baseUIFontFaceManager = std::make_shared<Leggiero::LUI::UIFontFaceManager>(*m_baseUIBundleLoader);
		m_baseUIDescriptionManager = std::make_shared<Leggiero::LUI::Description::DescriptionManager>(
			*pGame->GetComponent<Leggiero::Font::GlyphManagerComponent>(), 
			m_baseUIBundleLoader.get(), m_baseUITextureManager.get(), m_baseUIFontFaceManager.get()
			);

		m_baseUIDescriptionSystemUnit = std::make_shared<Leggiero::LUI::Description::SystemUnit>(pGame->GetComponent<Leggiero::Application::ApplicationComponent>());
		m_baseUIDescriptionManager->AddUnit(Leggiero::LUI::Description::SystemUnit::kSystemUnitName, m_baseUIDescriptionSystemUnit);
	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::UIResources_GraphicShutdownResources(Game *pGame)
	{

	}

	//------------------------------------------------------------------------------
	void ConcreteSharedResources::UIResources_ShutdownResources(Game *pGame)
	{
		m_baseUIDescriptionManager.reset();
		m_baseUIFontFaceManager.reset();
		m_baseUITextureManager.reset();
		m_baseUIBundleLoader.reset();

		m_baseUIDescriptionSystemUnit.reset();

		m_sharedUIManager.reset();

		m_commonUITextureColorShader.reset();
		m_commonUITextureBlurShader.reset();
	}


	//////////////////////////////////////////////////////////////////////////////// SharedResources

	//------------------------------------------------------------------------------
	// Concrete Factory Function
	SharedResources *SharedResources::CreateSharedResources()
	{
		return new ConcreteSharedResources();
	}
}
