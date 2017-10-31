#include <cassert>
#include <fstream>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/obj.hpp"
#include "Core/Math/PhiMath.hpp"
#include "Core/Windows/Win32Window.hpp"
#include "Core/Logger.hpp"
#include "Core/Texture.hpp"

#include "Core/Windows/PlatformWindows.hpp"

#include "Core/Render/RIOpenGL/RIOpenGLResourceStore.hpp"
#include "Core/Render/RIOpenGL/RIDeviceOpenGL.hpp"
#include "Core/Render/RIOpenGL/RenderInitWGL.hpp"
#include "Core/Render/RIOpenGL/RIWGLGlewSupport.hpp"
#include "Core/Render/RIOpenGL/RIContextOpenGL.hpp"
#include "Core/Render/RIOpenGL/OpenGL.hpp"

namespace Phoenix
{
	struct RenderMesh
	{
		Matrix4 modelMat;
		VertexBufferHandle vb;
		IndexBufferHandle ib;
		uint32_t numVertices;
		uint32_t numIndices;
		UniformHandle modelMatHandle;
	};

	RenderMesh createPlaneMesh(IRIDevice* renderDevice)
	{
		RenderMesh mesh;

		// LD, RD, RU, LU
		Vec3 vertices[] = { Vec3{ -0.5, -0.5, 0 }, Vec3{ 0.5, -0.5, 0 }, Vec3{ 0.5, 0.5, 0 }, Vec3{ -0.5, 0.5, 0 } };
		Vec2 uv[] = { Vec2{ 0.0 ,1.0 }, Vec2{ 1.0 ,1.0 }, Vec2{ 1.0 ,0.0 }, Vec2{ 0.0 ,0.0 } };

		uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3, },
		{ sizeof(Vec3), 4, &vertices });

		layout.add({ EAttributeProperty::TexCoord, EAttributeType::Float, 2 },
		{ sizeof(Vec2), 4, &uv });

		mesh.vb = renderDevice->createVertexBuffer(layout);
		mesh.numVertices = 4;
		mesh.ib = renderDevice->createIndexBuffer(sizeof(unsigned int), 6, &indices);
		mesh.numIndices = 6;

		mesh.modelMat = Matrix4::identity();
		mesh.modelMatHandle = renderDevice->createUniform("modelTf", EUniformType::Mat4);
		
		return mesh;
	}

	ProgramHandle loadProgram(IRIDevice* renderDevice, char* vsPath, const char* fsPath)
	{
		std::string vsSource = Platform::loadText(vsPath);
		VertexShaderHandle vs = renderDevice->createVertexShader(vsSource.c_str());

		std::string fsSource = Platform::loadText(fsPath);
		FragmentShaderHandle fs = renderDevice->createFragmentShader(fsSource.c_str());

		return renderDevice->createProgram(vs, fs);
	}

	RenderMesh loadRenderMesh(const Mesh& mesh, IRIDevice* renderDevice)
	{
		RenderMesh renderMesh;

		renderMesh.modelMat = Matrix4::identity();

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
		{ sizeof(Vec3), mesh.vertices.size(), mesh.vertices.data() });

		layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
		{ sizeof(Vec3), mesh.normals.size(), mesh.normals.data() });

		renderMesh.vb = renderDevice->createVertexBuffer(layout);
		renderMesh.ib = renderDevice->createIndexBuffer(sizeof(uint32_t), mesh.indices.size(), mesh.indices.data());
		renderMesh.modelMatHandle = renderDevice->createUniform("modelTf", EUniformType::Mat4);

		assert(renderMesh.vb.isValid());
		assert(renderMesh.ib.isValid());
		assert(renderMesh.modelMatHandle.isValid());

		renderMesh.numVertices = mesh.vertices.size();
		renderMesh.numIndices = mesh.indices.size();

		return renderMesh;
	}
	
	TextureDesc createDesc(const Texture& texture, ETextureFilter minFilter, ETextureFilter magFilter, uint8_t numMips = 0)
	{
		TextureDesc desc;
		desc.width = texture.m_width;
		desc.height = texture.m_height;

		switch (texture.m_components)
		{
		case 4:
		{ desc.pixelFormat = EPixelFormat::R8G8B8A8; } break;
		case 3: 
		{ desc.pixelFormat = EPixelFormat::R8G8B8; } break;
		case 2:
		{ assert(false); } break; 
		case 1:
		{ assert(false); } break;
		default: { assert(false); } break;
		}

		desc.minFilter = minFilter;
		desc.magFilter = magFilter;

		desc.numMips = numMips;

		return desc;
	}
}

int main(int argc, char** argv)
{
	using namespace Phoenix;

	Logger::init(true, false, 1024);
	Logger::setAnsiColorEnabled(Platform::enableConsoleColor(true));

	SetProcessDPIAware();

	Tests::runMathTests();
	Tests::runMemoryTests();

	std::unique_ptr<Mesh> fox = loadObj("Models/Fox/", "RedFox.obj");
	assert(fox != nullptr);

	WindowConfig config = {
		800, 600,
		0,0,
		std::wstring(L"Phoenix"),
		false,
		true };

	Win32Window window(config);
	if (!window.isOpen())
	{
		Logger::error("Failed to initialize Win32Window");
		return -1;
	}
	
	// We need to strap something like a factory around this so we can startup all systems properly.
	RenderInitWGL init(window.getNativeHandle(), 2);
	
	RIOpenGLResourceStore glResources;
	
	RIWGLGlewSupport wgl(&init);
	IRIDevice* renderDevice = new RIDeviceOpenGL(&glResources);
	RIContextOpenGL renderContext(&glResources);

	RenderMesh mesh = createPlaneMesh(renderDevice);
	
	ProgramHandle programHandle = loadProgram(renderDevice, "Shaders/textured.vert", "Shaders/textured.frag");
	
	Matrix4 viewTf = lookAtRH(Vec3{ 0, 0, 5 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 });
	Matrix4 projTf = perspectiveRH(70, (float)config.width / (float)config.height, 0.1, 100);

	UniformHandle viewMat = renderDevice->createUniform("viewTf", EUniformType::Mat4);
	UniformHandle projMat = renderDevice->createUniform("projectionTf", EUniformType::Mat4);

	renderContext.setShaderProgram(programHandle);
	renderContext.bindUniform(viewMat, &viewTf);
	renderContext.bindUniform(projMat, &projTf);

	RenderTargetHandle tempdefault;
	tempdefault.m_idx = 0;
	RGBA clearColor{ 1.f, 1.f, 1.f, 1.f };
	float angle = 0.f;

	Texture testTexture;
	testTexture.load("Textures/ein.png");
	TextureDesc desc = createDesc(testTexture, ETextureFilter::Linear, ETextureFilter::Linear);
	Texture2DHandle texture = renderDevice->createTexture2D(desc, "testTexture");
	
	renderContext.uploadTextureData(texture, testTexture.m_data);
	renderContext.bindTexture(texture);

	while (window.isOpen())
	{
		renderContext.clearRenderTargetColor(tempdefault, clearColor);
		renderContext.clearRenderTargetDepth(tempdefault);

		angle += 0.5f;
		mesh.modelMat = Matrix4::rotation(0, angle, 0);

		renderContext.bindUniform(mesh.modelMatHandle, &mesh.modelMat);

		renderContext.drawIndexed(mesh.vb, mesh.ib, EPrimitive::Triangles);

		wgl.swapBuffers();
		window.processMessages();

		checkGlErrorOccured();
	
		renderContext.endFrame();
	}

	/*ProgramHandle program = loadProgram(renderDevice, "Shaders/diffuse.vert", "Shaders/diffuse.frag");

	Matrix4 viewTf = lookAtRH(Vec3{ 0, 0, 5 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 });
	Matrix4 projTf = perspectiveRH(70, (float)config.width / (float)config.height, 0.1, 100);

	UniformHandle viewMat = renderDevice->createUniform("viewTf", EUniformType::Mat4);
	UniformHandle projMat = renderDevice->createUniform("projectionTf", EUniformType::Mat4);

	RGBA clearColor{ 0.f, 0.f, 0.f, 1.f };
	RenderTargetHandle tempdefault;
	tempdefault.m_idx = 0;

	float angle = 0.f;

	renderContext.setShaderProgram(program);

	renderContext.setShaderData(viewMat, program, &viewTf, sizeof(Matrix4));
	renderContext.setShaderData(projMat, program, &projMat, sizeof(Matrix4));

	while (window.isOpen())
	{
		renderContext.clearRenderTargetColor(tempdefault, clearColor);
		renderContext.clearRenderTargetDepth(tempdefault);

		angle += 0.05f;
		mesh.modelMat = Matrix4::rotation(0, angle, 0);

		renderContext.setShaderData(mesh.modelMatHandle, program, &mesh.modelMat, sizeof(Matrix4));

		renderContext.drawIndexed(mesh.vb, mesh.ib, EPrimitive::Triangles, mesh.numVertices, 0);

		wgl.swapBuffers();
		window.processMessages();

		checkGlErrorOccured();
	}*/

	delete renderDevice;
	Logger::exit();
	return 0;

	/*WGlRenderInit renderInit(window.getNativeHandle(), 4);
	RenderFrontend::init(&renderInit);

	RenderMesh planeMesh = createPlaneMesh();
	ProgramHandle planeProgram = loadProgram("Shaders/reflect.vert", "Shaders/reflect.frag");

	UniformHandle lightPos = RenderFrontend::createUniform("lightPosition", EUniform::Vec3, &Vec3(-5, 3, 5), sizeof(Vec3));

	float angle = 0.f;

	checkGlError();

	TextureHandle cubemap;

	{
		Texture front, back, up, down, left, right;
		front.load("Textures/vasa/negz.jpg");
		back.load("Textures/vasa/posz.jpg");
		up.load("Textures/vasa/posy.jpg");
		down.load("Textures/vasa/negy.jpg");
		left.load("Textures/vasa/negx.jpg");
		right.load("Textures/vasa/posx.jpg");

		CubemapData cbData;
		cbData.data[CubemapData::Right] = right.m_data;
		cbData.data[CubemapData::Left] = left.m_data;
		cbData.data[CubemapData::Up] = up.m_data;
		cbData.data[CubemapData::Down] = down.m_data;
		cbData.data[CubemapData::Back] = back.m_data;
		cbData.data[CubemapData::Front] = front.m_data;

		cubemap = RenderFrontend::createCubemap(createDesc(front, ETexture::Nearest, ETexture::Nearest), "cubemap", cbData);

		RenderFrontend::submitCommands();
	}

	while (window.isOpen())
	{
		angle += 0.5f;

		RenderFrontend::clearRenderTarget({}, EBuffer::Color, { 0.f, 0.f, 0.f, 1.f });
		RenderFrontend::clearRenderTarget({}, EBuffer::Depth, {});
		
		StateGroup state;
		state.depth = EDepth::Enable;

		planeMesh.modelMat = Matrix4::rotation(0, angle, 0);
		RenderFrontend::setUniform(planeMesh.modelMatHandle, &planeMesh.modelMat, sizeof(Matrix4));

		state.program = planeProgram;

		UniformHandle uniforms[] = { planeMesh.modelMatHandle, viewMat, projectionMat };
		state.uniforms = uniforms;
		state.uniformCount = 3;

		TextureHandle textures[] = { cubemap };
		state.textures = textures;
		state.textureCount = 1;

		RenderFrontend::drawIndexed(planeMesh.vb, planeMesh.ib, EPrimitive::Triangles, 0, planeMesh.numIndices, state);
		
		RenderFrontend::submitCommands();
		RenderFrontend::swapBuffers();

		window.processMessages();

		checkGlError();
	}*/

	//RenderFrontend::exit();
}


