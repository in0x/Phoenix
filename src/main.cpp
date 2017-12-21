#include <assert.h>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/Logger.hpp"
#include "Core/Windows/PlatformWindows.hpp"

#include "Core/WorldObject.hpp"

#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"
#include "Render/DeferredRenderer.hpp"

#include "Core/GameFramework/ECS.hpp"

int main(int argc, char** argv)
{
	using namespace Phoenix;

	Logger::init(true, false, 1024);
	Logger::setAnsiColorEnabled(Platform::enableConsoleColor(true));

	Tests::runMathTests();
	Tests::runMemoryTests();

	RIOpenGL renderInterface;
	bool bRIstarted = renderInterface.init();

	if (!bRIstarted)
	{
		Logger::error("Failed to initialize RenderInterface");
		assert(false);
	}

	IRIDevice* renderDevice = renderInterface.getRenderDevice();
	IRIContext* renderContext = renderInterface.getRenderContex();

	WindowConfig config;
	config.width = 800;
	config.height = 600;
	config.title = "Phoenix";
	config.bFullscreen = false;

	std::unique_ptr<RenderWindow> windowPtr = renderInterface.createWindow(config);

	RenderWindow* window = windowPtr.get();

	renderInterface.setWindowToRenderTo(window);

	WorldObject entity;
	entity.m_mesh = loadRenderMesh("Models/Fox/RedFox.obj", renderDevice);
	entity.m_material = Material(Vec3(1.f, 1.f, 1.f), Vec3(0.3f, 0.3f, 0.3f), 100.f);

	Matrix4 viewTf = lookAtRH(Vec3(0.f, 0.f, 7.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	Matrix4 projTf = perspectiveRH(70.f, (float)config.width / (float)config.height, 0.1f, 100.f);
	
	renderContext->setDepthTest(EDepth::Enable);

	DeferredRenderer renderer(renderDevice, config.width, config.height);
	renderer.setViewMatrix(viewTf);
	renderer.setProjectionMatrix(projTf);

	DirectionalLight lightRed;
	lightRed.m_direction = Vec3(0.f, -1.f, 0.f);
	lightRed.m_color = Vec3(0.3f, 0.f, 0.f);

	DirectionalLight lightBlue;
	lightBlue.m_direction = Vec3(1.f, 0.f, 0.f);
	lightBlue.m_color = Vec3(0.f, 0.f, 1.f);

	DirectionalLight lightGreen;
	lightGreen.m_direction = Vec3(0.f, 1.f, 0.f);
	lightGreen.m_color = Vec3(0.f, 1.f, 0.f);

	// Entity test

	Entity testEntity;

	TransformComponent tfComp;

	testEntity.addComponent(&tfComp);
	TransformComponent* gotComponent = testEntity.getComponent<TransformComponent>();
	assert(gotComponent == &tfComp);
	testEntity.removeComponent(&tfComp);

	//

	while (!window->wantsToClose())
	{
		renderer.setupGBufferPass(renderContext);
		renderer.fillGBuffer(entity, renderContext);

		renderer.setupLightPass(renderContext);
		renderer.drawLight(lightRed, renderContext);
		renderer.drawLight(lightBlue, renderContext);
		renderer.drawLight(lightGreen, renderContext);

		renderContext->endPass();

		renderInterface.swapBufferToWindow(window);

		Platform::pollEvents();
	}

	renderInterface.exit();
	Logger::exit();
	return 0;
}


