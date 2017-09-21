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
* Implement Uniforms
* Implement Textures
* Implement FrameBuffers
* Implement dynamic array -> I'm not using that many vectors so this isn't very high prio.
* Provide default stategroup and ensure that it works
* Provide printf style logging

* The solution to being stateless is really just to set everything that isnt specified
* to a default
*/

char* getCMDOption(char** start, char** end, const std::string& option)
{
	char** iter = std::find(start, end, option);

	if (iter != end && ++iter != end)
	{
		return *iter;
	}

	return nullptr;
}

std::string loadText(const char* path)
{
	std::string fileString;
	std::ifstream fileStream(path);
	
	if (fileStream)
	{
		fileString.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
		fileString.erase(std::remove_if(fileString.begin(), fileString.end(),
			[](const char c) {
			return !(c >= 0 && c < 128);
		}), fileString.end());
	
		return fileString;
	}
	else
	{
		Phoenix::Logger::error("Failed to load shader");
		return nullptr;
	}
}

#include "Core/Memory/PoolAllocator.hpp"
#include "Core/Memory/StackAllocator.hpp"

namespace Phoenix
{
	struct RenderMesh
	{
		VertexBufferHandle vb;
		IndexBufferHandle ib;
		uint32_t numVertices;
		uint32_t numIndices;
	};

	ProgramHandle loadProgram(const char* vsPath, const char* fsPath)
	{
		std::string vsSource = loadText(vsPath);
		ShaderHandle vs = RenderFrontend::createShader(vsSource.c_str(), Shader::Vertex);

		std::string fsSource = loadText(fsPath);
		ShaderHandle fs = RenderFrontend::createShader(fsSource.c_str(), Shader::Fragment);

		Shader::List shaders = Shader::createList();
		shaders[Shader::Vertex] = vs;
		shaders[Shader::Fragment] = fs;
		return RenderFrontend::createProgram(shaders);
	}
	
	RenderMesh loadRenderMesh(Mesh* mesh)
	{
		RenderMesh renderMesh;
		
		VertexBufferFormat layout;
		layout.add({ AttributeProperty::Position, AttributeType::Float, 3 },
		{ sizeof(Vec3), mesh->vertices.size(), mesh->vertices.data() });

		layout.add({ AttributeProperty::Normal, AttributeType::Float, 3 },
		{ sizeof(Vec3), mesh->normals.size(), mesh->normals.data() });

		renderMesh.vb = RenderFrontend::createVertexBuffer(layout);

		renderMesh.ib = RenderFrontend::createIndexBuffer(sizeof(unsigned int), mesh->indices.size(), mesh->indices.data());

		renderMesh.numVertices = mesh->vertices.size();
		renderMesh.numIndices = mesh->indices.size();

		return renderMesh;
	}
}

void run()
{
	using namespace Phoenix;

	Logger::init(true, false);
	Logger::setAnsiColorEnabled(Platform::enableConsoleColor(true));
	
	SetProcessDPIAware();

	Tests::runMathTests();
	Tests::runMemoryTests();

	std::unique_ptr<Mesh> fox = loadObj("Fox/", "RedFox.obj");

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
		return;
	}

	WGlRenderInit renderInit(window.getNativeHandle(), 4096);
	RenderFrontend::init(&renderInit);

	RenderMesh renderMesh = loadRenderMesh(fox.get());
	ProgramHandle program = loadProgram("Shaders/diffuse.vert", "Shaders/diffuse.frag");
	
	Matrix4 worldMat = Matrix4::identity();
	Matrix4 viewMat = lookAtRH(Vec3{ 2, 2, -7 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 });
	Matrix4 projMat = perspectiveRH(70, (float)config.width / (float)config.height, 1, 100);
	Vec3 lightPosition = Vec3(-5, 3, 5);
	
	UniformHandle mmat = RenderFrontend::createUniform("modelTf", Uniform::Mat4, &worldMat, sizeof(Matrix4));
	UniformHandle vmat = RenderFrontend::createUniform("viewTf", Uniform::Mat4, &viewMat, sizeof(Matrix4));
	UniformHandle pmat = RenderFrontend::createUniform("projectionTf", Uniform::Mat4, &projMat, sizeof(Matrix4));
	UniformHandle lit = RenderFrontend::createUniform("lightPosition", Uniform::Vec3, &lightPosition, sizeof(Vec3));

	auto uniforms = RenderFrontend::createUniformList(mmat, vmat, pmat);

	float angle = 0.f;

	checkGlError();

	while (window.isOpen())
	{
		RenderFrontend::clearFrameBuffer({}, Buffer::Color, { 1.f, 1.f, 1.f, 1.f });
		RenderFrontend::clearFrameBuffer({}, Buffer::Depth, { });

		angle += 0.025f;
		Matrix4 rotMat = Matrix4::rotation(0.f, angle, 0.f);

		RenderFrontend::setUniform(mmat, &rotMat, sizeof(Matrix4));
		
		StateGroup state;
		state.program = program;
		state.depth = Depth::Enable;
		state.uniforms = uniforms;

		RenderFrontend::drawIndexed(renderMesh.vb, renderMesh.ib, Primitive::Triangles, 0, renderMesh.numIndices, state);

		RenderFrontend::submitCommands();
		RenderFrontend::swapBuffers();
		
		window.processMessages();
	}

	Logger::exit();
	RenderFrontend::exit();
}

int main(int argc, char** argv)
{
	run();
	return 0;
}


