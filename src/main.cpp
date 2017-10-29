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

namespace Phoenix
{
	struct RenderMesh
	{
		Matrix4 modelMat;
		VertexBufferHandle vb;
		IndexBufferHandle ib;
		uint32_t numVertices;
		uint32_t numIndices;
		Mat4UniformHandle modelMatHandle;
	};

	RenderMesh createPlaneMesh(IRIDevice* renderDevice)
	{
		RenderMesh mesh;

		// LD, RD, RU, LU
		Vec3 vertices[] = { Vec3{-0.5, -0.5, 0}, Vec3{0.5, -0.5, 0}, Vec3{0.5, 0.5, 0}, Vec3{-0.5, 0.5, 0} };
		Vec3 normals[] = { Vec3{ 0,0,1 },Vec3{ 0,0,1 },Vec3{ 0,0,1 },Vec3{ 0,0,1 } };

		uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3, },
		{ sizeof(Vec3), 4, &vertices });

		layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3, },
		{ sizeof(Vec3), 4, &normals });

		mesh.vb = renderDevice->createVertexBuffer(layout);
		mesh.numVertices = 4;
		mesh.ib = renderDevice->createIndexBuffer(sizeof(unsigned int), 6, &indices);
		mesh.numIndices = 6;

		mesh.modelMat = Matrix4::translation(0, 0, 5);
		mesh.modelMatHandle = renderDevice->createMat4Uniform("modelTf", mesh.modelMat);

		mesh.modelMat = Matrix4::identity();

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
}
//
//	TextureDesc createDesc(const Texture& texture, ETexture::Filter minFilter, ETexture::Filter maxFilter)
//	{
//		TextureDesc desc;
//		desc.width = texture.m_width;
//		desc.height = texture.m_height;
//
//		switch (texture.m_components)
//		{
//		case 4:
//		{ desc.components = ETexture::RGBA; } break;
//		case 3:
//		{ desc.components = ETexture::RGB; } break;
//		case 2:
//		{ desc.components = ETexture::RG; } break;
//		case 1:
//		{ desc.components = ETexture::R; } break;
//		default: { assert(false); } break;
//			// TODO(Phil): How do we handle depth?
//		}
//
//		desc.minFilter = minFilter;
//		desc.magFilter = maxFilter;
//
//		// Mips?
//		return desc;
//	}
//}


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
	
	Matrix4 mat;

	// We need to strap something like a factory around this so we can startup all systems properly.
	RenderInitWGL init(window.getNativeHandle(), 0);
	RIOpenGLResourceStore glResources;
	RIWGLGlewSupport glew(&init);
	IRIDevice* renderDevice = new RIDeviceOpenGL(&glResources);
	
	RenderMesh plane = createPlaneMesh(renderDevice);
	//ProgramHandle planeProgram = loadProgram(renderDevice, "Shaders/reflect.vert", "Shaders/reflect.frag");

	delete renderDevice;

	/*WGlRenderInit renderInit(window.getNativeHandle(), 4);
	RenderFrontend::init(&renderInit);

	RenderMesh planeMesh = createPlaneMesh();
	ProgramHandle planeProgram = loadProgram("Shaders/reflect.vert", "Shaders/reflect.frag");

	UniformHandle viewMat = RenderFrontend::createUniform("viewTf", EUniform::Mat4, &lookAtRH(Vec3{ 0, 0, 2 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 }), sizeof(Matrix4));
	UniformHandle projectionMat = RenderFrontend::createUniform("projectionTf", EUniform::Mat4, &perspectiveRH(70, (float)config.width / (float)config.height, 1, 100), sizeof(Matrix4));
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

	Logger::exit();
	//RenderFrontend::exit();

	return 0;
}


