#include <assert.h>
#include <fstream>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/obj.hpp"
#include "Core/Logger.hpp"
#include "Core/Texture.hpp"

#include "Core/Windows/PlatformWindows.hpp"
#include "Math/PhiMath.hpp"

#include "Render/RIOpenGL/RIOpenGL.hpp"

namespace Phoenix
{
	struct RenderMesh
	{
		Matrix4 modelMat;
		VertexBufferHandle vb;
		IndexBufferHandle ib;
		size_t numVertices;
		size_t numIndices;
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

	OBJImport fox = loadObj("Models/Fox/", "RedFox.obj");
	assert(fox.mesh != nullptr);

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

	ProgramHandle program = loadProgram(renderDevice, "Shaders/diffuse.vert", "Shaders/diffuse.frag");
	RenderMesh mesh = loadRenderMesh(*fox.mesh, renderDevice);
	
	Matrix4 viewTf = lookAtRH(Vec3( 0, 0, 5 ), Vec3( 0,0,0 ), Vec3( 0,1,0 ));
	Matrix4 projTf = perspectiveRH(70.f, (float)config.width / (float)config.height, 0.1f, 100.f);

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
		mesh.modelMat = Matrix4::rotation(0, angle, 0);

		renderContext->bindUniform(mesh.modelMatHandle, &mesh.modelMat);
		
		renderContext->drawIndexed(mesh.vb, mesh.ib, EPrimitive::Triangles, mesh.numIndices, 0);
		
		renderContext->endPass();

		renderInterface.swapBufferToWindow(window);

		Platform::pollEvents();		
	}

	renderInterface.exit();
	Logger::exit();
	return 0;
}


