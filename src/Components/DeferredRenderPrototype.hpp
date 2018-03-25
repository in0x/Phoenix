#pragma once

#include <Core/Component.hpp>
#include <Core/System.hpp>

#include <Render/DeferredRenderer.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;

	/*
		Outline: DeferredRenderComponent is the singleton component offering 
		the renderer to each system. Each system then applies certain operations
		to the renderer. E.g. the DrawStaticMeshSystem draw all static meshes into 
		the gbuffer. The lighting system draws all the lights into the gbuffer. etc.
	*/
	
	struct DeferredRenderComponent : public Component<DeferredRenderComponent>
	{
		DeferredRenderComponent(IRIDevice* renderDevice, uint32_t gBufferWidth, uint32_t gBufferHeight);

		DeferredRenderer m_renderer;
	};

	DeferredRenderComponent::DeferredRenderComponent(IRIDevice* renderDevice, uint32_t gBufferWidth, uint32_t gBufferHeight)
		: m_renderer(renderDevice, gBufferWidth, gBufferHeight)
	{
	}
	
	struct DrawStaticMeshSystem : public ISystem
	{
		virtual void tick(World* world, float dt) override;
	};

	void DrawStaticMeshSystem::tick(World* world, float dt)
	{
		auto* renderComp = world->getSingletonComponent<DeferredRenderComponent>();
		renderComp->m_renderer.setupGBufferPass();

		ComponentIterator<StaticMeshComponent> iter(world);
	
		for (; iter; ++iter)
		{
			renderComp->m_renderer.fillGBuffer();
		}
	}

	struct LightingSystem : public ISystem
	{

	};
}
