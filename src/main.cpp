#include <assert.h>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/Logger.hpp"
#include "Core/Texture.hpp"
#include "Core/Windows/PlatformWindows.hpp"

#include "Core/Mesh.hpp"
#include "Core/Shader.hpp"
#include "Core/Texture.hpp"

#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"

int main(int argc, char** argv)
{
	using namespace Phoenix;

	Logger::init(true, false, 1024);
	Logger::setAnsiColorEnabled(Platform::enableConsoleColor(true));

	SetProcessDPIAware();

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

	RenderMesh mesh = loadRenderMesh("Models/Fox/RedFox.obj", renderDevice);

	ProgramHandle program = loadShaderProgram(renderDevice, "Shaders/diffuse.vert", "Shaders/diffuse.frag");
	
	Matrix4 viewTf = lookAtRH(Vec3( 0, 0, 5 ), Vec3( 0,0,0 ), Vec3( 0,1,0 ));
	Matrix4 projTf = perspectiveRH(70.f, (float)config.width / (float)config.height, 0.1f, 100.f);

	UniformHandle modelMat = renderDevice->createUniform("modelTf", EUniformType::Mat4);
	UniformHandle viewMat = renderDevice->createUniform("viewTf", EUniformType::Mat4);
	UniformHandle projMat = renderDevice->createUniform("projectionTf", EUniformType::Mat4);
	UniformHandle ambient = renderDevice->createUniform("cAmbient", EUniformType::Vec3);
	UniformHandle diffuse = renderDevice->createUniform("cDiffuse", EUniformType::Vec3);
	UniformHandle specular = renderDevice->createUniform("cSpecular", EUniformType::Vec3);
	UniformHandle light = renderDevice->createUniform("lightPosition", EUniformType::Vec3); 

	RenderTargetHandle tempdefault;
	tempdefault.m_idx = 0;
	RGBA clearColor{ 1.f, 1.f, 1.f, 1.f };
	float angle = 0.f;

	Vec3 cAmbient = Vec3(0.3f, 0.3f, 0.3f);
	Vec3 cDiffuse = Vec3(0.6f, 0.15f, 0.15f);
	Vec3 cSpecular = Vec3(0.6f, 0.15f, 0.15f);
	
	Vec3 lightPos = Vec3(0.f, 5.f, 3.f);

	renderContext->bindShaderProgram(program);
	renderContext->bindUniform(viewMat, &viewTf);
	renderContext->bindUniform(projMat, &projTf);
	renderContext->bindUniform(ambient, &cAmbient);
	renderContext->bindUniform(diffuse, &cDiffuse);
	renderContext->bindUniform(specular, &cSpecular);
	renderContext->bindUniform(light, &lightPos);

	while (!window->wantsToClose())
	{
		renderContext->clearColor();
		renderContext->clearDepth();

		angle += 0.5f;
		Matrix4 rotation = Matrix4::rotation(0, angle, 0);

		renderContext->bindUniform(modelMat, &rotation);
		
		renderContext->drawIndexed(mesh.vb, mesh.ib, EPrimitive::Triangles, mesh.numIndices);
		
		renderContext->endPass();

		renderInterface.swapBufferToWindow(window);

		Platform::pollEvents();		
	}

	renderInterface.exit();
	Logger::exit();
	return 0;
}


