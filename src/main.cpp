#include <cassert>
#include <fstream>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/obj.hpp"
#include "Core/Math/PhiMath.hpp"
#include "Core/Windows/Win32Window.hpp"
#include "Core/Logger.hpp"
#include "Core/Texture.hpp"

#include "Core/Render/RenderFrontend.hpp"
#include "Core/Render/WGlRenderBackend.hpp"

#include "Core/Windows/PlatformWindows.hpp"

/*
CURRENT TASK:
* Implement Textures

TODO:
* Crashes when closed by closing window via taskbar
* Implement FrameBuffers
* Implement dynamic array -> I'm not using that many vectors so this isn't very high prio.
* Provide default stategroup and ensure that it works
* Figure out how overall memory acquisition will work
* Input
* Moveable camera
* Standardized preamble for each log: TIME | SEVERITY | -> msg
* I should startup glew independently of the backend
* Add sparse array to shaderlist that tells us which shaders have been set
* The sort keys are produced by the system that inserts into the buckets, i.e. the mesherenderer,
* uirenderer etc. This allows them to all use a different pattern. The bucket then just does a
* transparent sort based on the bits in the key.

* The solution to being stateless is really just to set everything that isnt specified
* to a default

* Directly interpreting commands with OpenGL/DX might be more flexible (instead of using the IRenderBackend interface)

*/

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

	RenderMesh createPlaneMesh()
	{
		RenderMesh mesh;

		// LD, RD, RU, LU
		Vec3 vertices[] = { Vec3{-0.5, -0.5, 0}, Vec3{0.5, -0.5, 0}, Vec3{0.5, 0.5, 0}, Vec3{-0.5, 0.5, 0} };
		Vec2 uv[] = { Vec2{ 0.0 ,1.0 }, Vec2{ 1.0 ,1.0 }, Vec2{ 1.0 ,0.0 }, Vec2{ 0.0 ,0.0 } };

		uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3, },
		{ sizeof(Vec3), 4, &vertices });

		layout.add({ EAttributeProperty::TexCoord, EAttributeType::Float, 2 }, 
		{ sizeof(Vec2), 4, &uv });

		mesh.vb = RenderFrontend::createVertexBuffer(layout);
		mesh.numVertices = 4;
		mesh.ib = RenderFrontend::createIndexBuffer(sizeof(unsigned int), 6, &indices);
		mesh.numIndices = 6;

		mesh.modelMat = Matrix4::translation(0, 0, 5);
		mesh.modelMatHandle = RenderFrontend::createUniform("modelTf", EUniform::Mat4, &mesh.modelMat, sizeof(Matrix4));
		RenderFrontend::submitCommands();
	
		mesh.modelMat = Matrix4::identity();

		return mesh;
	}

	ProgramHandle loadProgram(const char* vsPath, const char* fsPath)
	{
		std::string vsSource = Platform::loadText(vsPath);
		ShaderHandle vs = RenderFrontend::createShader(vsSource.c_str(), EShader::Vertex);

		std::string fsSource = Platform::loadText(fsPath);
		ShaderHandle fs = RenderFrontend::createShader(fsSource.c_str(), EShader::Fragment);

		EShader::List shaders = EShader::createList();
		shaders[EShader::Vertex] = vs;
		shaders[EShader::Fragment] = fs;
		return RenderFrontend::createProgram(shaders);
	}

	RenderMesh loadRenderMesh(const Mesh& mesh)
	{
		RenderMesh renderMesh;

		renderMesh.modelMat = Matrix4::identity();

		VertexBufferFormat layout;
		layout.add({ EAttributeProperty::Position, EAttributeType::Float, 3 },
		{ sizeof(Vec3), mesh.vertices.size(), mesh.vertices.data() });

		layout.add({ EAttributeProperty::Normal, EAttributeType::Float, 3 },
		{ sizeof(Vec3), mesh.normals.size(), mesh.normals.data() });

		renderMesh.vb = RenderFrontend::createVertexBuffer(layout);
		renderMesh.ib = RenderFrontend::createIndexBuffer(sizeof(uint32_t), mesh.indices.size(), mesh.indices.data());
		renderMesh.modelMatHandle = RenderFrontend::createUniform("modelTf", EUniform::Mat4, &renderMesh.modelMat, sizeof(Matrix4));

		renderMesh.numVertices = mesh.vertices.size();
		renderMesh.numIndices = mesh.indices.size();

		RenderFrontend::submitCommands();

		return renderMesh;
	}

	ETexture::Description createDesc(const Texture& texture, ETexture::Filter minFilter, ETexture::Filter maxFilter)
	{
		ETexture::Description desc;
		desc.width = texture.m_width;
		desc.height = texture.m_height;

		switch (texture.m_components)
		{
		case 4:
		{ desc.components = ETexture::RGBA; } break;
		case 3:
		{ desc.components = ETexture::RGB; } break;
		case 2:
		{ desc.components = ETexture::RG; } break;
		case 1:
		{ desc.components = ETexture::R; } break;
		default: { assert(false); } break;
		// TODO(Phil): How do we handle depth?
		}

		desc.minFilter = minFilter;
		desc.magFilter = maxFilter;

		// Mips?
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

	WGlRenderInit renderInit(window.getNativeHandle(), 4);
	RenderFrontend::init(&renderInit);

	RenderMesh planeMesh = createPlaneMesh();
	ProgramHandle program = loadProgram("Shaders/textured.vert", "Shaders/textured.frag");

	UniformHandle viewMat = RenderFrontend::createUniform("viewTf", EUniform::Mat4, &lookAtRH(Vec3{ 0, 0, 2 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 }), sizeof(Matrix4));
	UniformHandle projectionMat = RenderFrontend::createUniform("projectionTf", EUniform::Mat4, &perspectiveRH(70, (float)config.width / (float)config.height, 1, 100), sizeof(Matrix4));
	UniformHandle lightPos = RenderFrontend::createUniform("lightPosition", EUniform::Vec3, &Vec3(-5, 3, 5), sizeof(Vec3));

	Texture einTex;
	einTex.load("Textures/ein.png");
	TextureHandle ein = RenderFrontend::createTexture(createDesc(einTex, ETexture::Linear, ETexture::Linear), einTex.m_data, ETexture::Tex2D, "tex");
	
	float angle = 0.f;

	checkGlError();

	while (window.isOpen())
	{
		angle += 0.5f;

		RenderFrontend::clearFrameBuffer({}, EBuffer::Color, { 0.f, 0.f, 0.f, 1.f });
		RenderFrontend::clearFrameBuffer({}, EBuffer::Depth, {});
		RenderFrontend::submitCommands();

		planeMesh.modelMat = Matrix4::rotation(0.f, angle, 0.f);

		RenderFrontend::setUniform(planeMesh.modelMatHandle, &planeMesh.modelMat, sizeof(Matrix4));

		StateGroup state;
		state.program = program;
		state.depth = EDepth::Enable;

		UniformHandle uniforms[] = { planeMesh.modelMatHandle, viewMat, projectionMat };
		state.uniforms = uniforms;
		state.uniformCount = 3;

		TextureHandle textures[] = { ein };
		state.textures = textures;
		state.textureCount = 1;

		RenderFrontend::drawIndexed(planeMesh.vb, planeMesh.ib, EPrimitive::Triangles, 0, planeMesh.numIndices, state);

		RenderFrontend::submitCommands();
		RenderFrontend::swapBuffers();

		window.processMessages();
	}

	Logger::exit();
	RenderFrontend::exit();

	return 0;
}


