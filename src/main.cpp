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

	RenderMesh createPlaneMesh(IRIDevice* renderDevice, bool bShouldHaveNormals, bool bShoudHaveUVs)
	{
		RenderMesh mesh;

		// LD, RD, RU, LU
		Vec3 vertices[] = { Vec3{ -0.5, -0.5, 0 }, Vec3{ 0.5, -0.5, 0 }, Vec3{ 0.5, 0.5, 0 }, Vec3{ -0.5, 0.5, 0 } };
		Vec3 normals[] = { Vec3{ 0,0,1 },Vec3{ 0,0,1 },Vec3{ 0,0,1 },Vec3{ 0,0,1 } };
		Vec2 uv[] = { Vec2{ 0.0 ,1.0 }, Vec2{ 1.0 ,1.0 }, Vec2{ 1.0 ,0.0 }, Vec2{ 0.0 ,0.0 } };

		uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3, },
		{ sizeof(Vec3), 4, &vertices });

		if (bShouldHaveNormals)
		{
			layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3, },
			{ sizeof(Vec3), 4, &normals });
		}

		if (bShoudHaveUVs)
		{
			layout.add({ EAttributeProperty::TexCoord, EAttributeType::Float, 2 },
			{ sizeof(Vec2), 4, &uv });
		}

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

		desc.wrapU = ETextureWrap::ClampToEdge;
		desc.wrapV = ETextureWrap::ClampToEdge;
		desc.wrapW = ETextureWrap::ClampToEdge;

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

	RenderMesh mesh = createPlaneMesh(renderDevice, true, false);
	
	ProgramHandle programHandle = loadProgram(renderDevice, "Shaders/reflect.vert", "Shaders/reflect.frag");
	
	Matrix4 viewTf = lookAtRH(Vec3{ 0, 0, 5 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 });
	Matrix4 projTf = perspectiveRH(70, (float)config.width / (float)config.height, 0.1, 100);

	UniformHandle viewMat = renderDevice->createUniform("viewTf", EUniformType::Mat4);
	UniformHandle projMat = renderDevice->createUniform("projectionTf", EUniformType::Mat4);

	renderContext.bindShaderProgram(programHandle);
	renderContext.bindUniform(viewMat, &viewTf);
	renderContext.bindUniform(projMat, &projTf);

	RenderTargetHandle tempdefault;
	tempdefault.m_idx = 0;
	RGBA clearColor{ 1.f, 1.f, 1.f, 1.f };
	float angle = 0.f;

	TextureCubeHandle cubemap;

	{
		Texture negz, posz, posy, negy, negx, posx;
		negz.load("Textures/vasa/negz.jpg");
		posz.load("Textures/vasa/posz.jpg");
		posy.load("Textures/vasa/posy.jpg");
		negy.load("Textures/vasa/negy.jpg");
		negx.load("Textures/vasa/negx.jpg");
		posx.load("Textures/vasa/posx.jpg");

		cubemap = renderDevice->createTextureCube(createDesc(negz, ETextureFilter::Nearest, ETextureFilter::Nearest), "cubemap");
		
		renderContext.uploadTextureData(cubemap, ETextureCubeSide::XPositive, posx.m_data);
		renderContext.uploadTextureData(cubemap, ETextureCubeSide::XNegative, negx.m_data);
		
		renderContext.uploadTextureData(cubemap, ETextureCubeSide::ZPositive, posz.m_data);
		renderContext.uploadTextureData(cubemap, ETextureCubeSide::ZNegative, posz.m_data);
		
		renderContext.uploadTextureData(cubemap, ETextureCubeSide::YPositive, posy.m_data);
		renderContext.uploadTextureData(cubemap, ETextureCubeSide::YNegative, negy.m_data);
	}

	renderContext.bindTexture(cubemap);

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

	delete renderDevice;
	Logger::exit();
	return 0;
}


