#include <cassert>
#include <fstream>

#include "Tests/MathTests.hpp"
#include "Tests/MemoryTests.hpp"

#include "Core/obj.hpp"
#include "Core/Math/PhiMath.hpp"
#include "Core/Win32Window.hpp"
#include "Core/Logger.hpp"

#include "Core/Render/RenderFrontend.hpp"
#include "Core/Render/WGlRenderBackend.hpp"

#include "Core/PlatformWindows.hpp"

/*
TODO:
* Crashes when closed by closing window via taskbar
* Implement Textures
* Implement FrameBuffers
* Implement dynamic array -> I'm not using that many vectors so this isn't very high prio.
* Provide default stategroup and ensure that it works
* Provide printf style logging

* The sort keys are produced by the system that inserts into the buckets, i.e. the mesherenderer,
* uirenderer etc. This allows them to all use a different pattern. The bucket then just does a 
* transparent sort based on the bits in the key.

* The solution to being stateless is really just to set everything that isnt specified
* to a default
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
	};

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

		renderMesh.ib = RenderFrontend::createIndexBuffer(sizeof(unsigned int), mesh.indices.size(), mesh.indices.data());

		renderMesh.numVertices = mesh.vertices.size();
		renderMesh.numIndices = mesh.indices.size();

		return renderMesh;
	}

	struct PlaceholderRenderer
	{
		PlaceholderRenderer(const Matrix4& view, const Matrix4& projection, const Vec3& lightPos)
			: m_modelMat(RenderFrontend::createUniform("modelTf", EUniform::Mat4, &Matrix4::identity(), sizeof(Matrix4)))
			, m_viewMat(RenderFrontend::createUniform("viewTf", EUniform::Mat4, &view, sizeof(Matrix4)))
			, m_projectionMat(RenderFrontend::createUniform("projectionTf", EUniform::Mat4, &projection, sizeof(Matrix4)))
			, m_lightPos(RenderFrontend::createUniform("lightPosition", EUniform::Vec3, &lightPos, sizeof(Vec3)))
			, m_uniformList(RenderFrontend::createUniformList(m_modelMat, m_viewMat, m_projectionMat))
		{
		}

		void submit(const RenderMesh& mesh, ProgramHandle program)
		{
			RenderFrontend::setUniform(m_modelMat, &mesh.modelMat, sizeof(Matrix4));

			StateGroup state;
			state.program = program;
			state.depth = EDepth::Enable;
			state.uniforms = m_uniformList;
			RenderFrontend::drawIndexed(mesh.vb, mesh.ib, EPrimitive::Triangles, 0, mesh.numIndices, state);
		}

		void clear()
		{
			RenderFrontend::clearFrameBuffer({}, EBuffer::Color, { 1.f, 1.f, 1.f, 1.f });
			RenderFrontend::clearFrameBuffer({}, EBuffer::Depth, {});
			RenderFrontend::submitCommands();
		}

		void render()
		{
			RenderFrontend::submitCommands();
			RenderFrontend::swapBuffers();
		}

		UniformHandle m_modelMat;
		UniformHandle m_viewMat;
		UniformHandle m_projectionMat;
		UniformHandle m_lightPos;
		UniformList m_uniformList;
	};
}

int main(int argc, char** argv)
{
	using namespace Phoenix;

	Logger::init(true, false);
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
		false };

	Win32Window window(config);
	if (!window.isOpen())
	{
		Logger::error("Failed to initialize Win32Window");
		return -1;
	}

	WGlRenderInit renderInit(window.getNativeHandle(), 4096);
	RenderFrontend::init(&renderInit);

	RenderMesh renderMesh = loadRenderMesh(*fox);
	ProgramHandle program = loadProgram("Shaders/diffuse.vert", "Shaders/diffuse.frag");

	PlaceholderRenderer renderer(lookAtRH(Vec3{ 2, 2, -7 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 }),
								 perspectiveRH(70, (float)config.width / (float)config.height, 1, 100),
								 Vec3(-5, 3, 5));
	float angle = 0.f;

	checkGlError();

	while (window.isOpen())
	{
		angle += 0.025f;
		renderMesh.modelMat = Matrix4::rotation(0.f, angle, 0.f);

		renderer.clear();
		renderer.submit(renderMesh, program);
		renderer.render();

		window.processMessages();
	}

	Logger::exit();
	RenderFrontend::exit();

	return 0;
}


