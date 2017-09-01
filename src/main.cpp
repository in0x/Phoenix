#include <cassert>
#include <fstream>

#include "Tests/MathTests.hpp"

#include "Core/obj.hpp"
#include "Core/Math/PhiMath.hpp"
#include "Core/Win32Window.hpp"
#include "Core/Logger.hpp"

#include "Core/Render/WGlRenderContext.hpp"
#include "Core/Render/CommandBucket.hpp"
#include "Core/Render/Commands.hpp"
#include "Core/Render/CommandPacket.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_WINDOWS
#include "Core/PlatformWindows.hpp"
#endif

/*
TODO:
* Support uniforms (renderer = shared, context = platform-specific implementation)
* Crashes when closed by closing window via taskbar
* Implement linear allocator
* Implement dynamic array

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

void run()
{
	initPlatform();

	using namespace Phoenix;

	Logger::init(true, false);

	Tests::RunMathTests();

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

	Matrix4 worldMat = Matrix4::identity();
	Matrix4 viewMat = lookAtRH(Vec3{ 2, 2, -7 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 });
	Matrix4 projMat = perspectiveRH(70, (float)config.width / (float)config.height, 1, 100);
	Vec3 lightPosition = Vec3(-5, 3, 5);

	std::unique_ptr<IRenderContext> context = std::make_unique<WGlRenderContext>(window.getNativeHandle());
	context->init();
	CommandBucket commandBucket(16);
	
	VertexBufferHandle foxVertices;
	IndexBufferHandle foxIndices;
	{
		VertexBufferFormat foxLayout;
		foxLayout.add({ AttributeProperty::Position, AttributeType::Float, 3 },
		{ sizeof(Vec3), fox->vertices.size(), fox->vertices.data() });

		foxLayout.add({ AttributeProperty::Normal, AttributeType::Float, 3 },
		{ sizeof(Vec3), fox->normals.size(), fox->normals.data() });

		foxVertices = context->alloc<VertexBufferHandle>();
		auto cvb = commandBucket.addCommand<Commands::CreateVertexBuffer>();
		cvb->format = foxLayout;
		cvb->handle = foxVertices;
	
		foxIndices = context->alloc<IndexBufferHandle>();
		auto cib = commandBucket.addCommand<Commands::CreateIndexBuffer>();
		cib->size = sizeof(unsigned int);
		cib->count = fox->indices.size();
		cib->data = fox->indices.data();
		cib->handle = foxIndices;
	}

	ProgramHandle program;
	{
		std::string vsSource = loadText("Shaders/diffuse.vert");
		std::string fsSource = loadText("Shaders/diffuse.frag");

		size_t len = vsSource.size();
		Commands::CreateShader* cvs = commandBucket.addCommand<Commands::CreateShader>(len);	
		cvs->shaderType = Shader::Vertex;
		cvs->handle = context->alloc<ShaderHandle>(); 
		cvs->source = commandPacket::getAuxiliaryMemory(cvs);
		memcpy(commandPacket::getAuxiliaryMemory(cvs), &vsSource[0], len);
		cvs->source[len] = '\0';

		len = fsSource.size();
		Commands::CreateShader* cfs = commandBucket.appendCommand<Commands::CreateShader>(cvs, len);
		cfs->shaderType = Shader::Fragment;
		cfs->handle = context->alloc<ShaderHandle>();	
		cfs->source = commandPacket::getAuxiliaryMemory(cfs);
		memcpy(commandPacket::getAuxiliaryMemory(cfs), &fsSource[0], len);
		cfs->source[len] = '\0';

		auto createProg = commandBucket.appendCommand<Commands::CreateProgram>(cfs);
		program = context->alloc<ProgramHandle>();

		Shader::List shaders;
		shaders[Shader::Vertex] = cvs->handle;
		shaders[Shader::Fragment] = cfs->handle;
		
		createProg->shaders = shaders;
		createProg->handle = program;
	}
	
	commandBucket.submit(context.get());

	context->setProgram(program);
	
	glUniformMatrix4fv(2, 1, GL_FALSE, (GLfloat*)&worldMat);
	glUniformMatrix4fv(3, 1, GL_FALSE, (GLfloat*)&viewMat);
	glUniformMatrix4fv(4, 1, GL_FALSE, (GLfloat*)&projMat);
	glUniform3fv(5, 1, (GLfloat*)&lightPosition);

	float angle = 0.f;

	checkGlError();

	while (window.isOpen())
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat color[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, color);

		angle += 0.025f;
		Matrix4 rotMat = Matrix4::rotation(0.f, angle, 0.f);

		glUniformMatrix4fv(2, 1, GL_FALSE, (GLfloat*)&rotMat);

		auto dc = commandBucket.addCommand<Commands::DrawIndexed>();
		
		dc->vertexBuffer = foxVertices;
		dc->indexBuffer = foxIndices;
		dc->primitives = Primitive::Triangles;
		dc->start = 0;
		dc->count = fox->indices.size();

		dc->state.program = program;

		commandBucket.submit(context.get());
		context->swapBuffer();

		glDisable(GL_MULTISAMPLE);
		glDisable(GL_DEPTH_TEST);

		window.processMessages();
	}

	Logger::exit();
}

int main(int argc, char** argv)
{
	run();
	return 0;
}


