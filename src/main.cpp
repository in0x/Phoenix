#include <Windows.h>
#include <cassert>
#include <fstream>
#include "Core/obj.hpp"
#include "Core/Math/PhiMath.hpp"
#include "Core/Win32Window.hpp"
#include "Tests/MathTests.hpp"
#include "Core/StringTokenizer.hpp"
#include "Core/Logger.hpp"
#include "Core/Render/WGlRenderContext.hpp"

char* getCMDOption(char** start, char** end, const std::string& option)
{
	char** iter = std::find(start, end, option);

	if (iter != end && ++iter != end)
	{
		return *iter;
	}

	return nullptr;
}

/*
TODO:
	* Figure out seperate implementations for uniform registration and updating
	* 
	* Crashes when closed by closing window via taskbar

Likely structure:
Window -> Interface class with game
Win32Window -> Takes care of Windows specific stuff
and is created on start up
Engine -> actual engine class, gets a window passed to it
on creation
*/

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

	//std::unique_ptr<IRenderContext> context = std::make_unique<WGlRenderContext>(window.getNativeHandle());
	std::unique_ptr<WGlRenderContext> context = std::make_unique<WGlRenderContext>(window.getNativeHandle());
	context->init();

	Matrix4 worldMat = Matrix4::identity();
	Matrix4 viewMat = lookAtRH(Vec3{ 2, 2, -7 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 });
	Matrix4 projMat = perspectiveRH(70, (float)config.width / (float)config.height, 1, 100);
	Vec3 lightPosition = Vec3(-5, 3, 5);

	VertexBufferFormat foxLayout;
	foxLayout.add({ AttributeType::Position, AttributeSize::Float, 3 },
				  { sizeof(Vec3), fox->vertices.size(), fox->vertices.data() });

	foxLayout.add({ AttributeType::Normal, AttributeSize::Float, 3 }, 
				  { sizeof(Vec3), fox->normals.size(), fox->normals.data() });

	VertexBufferHandle foxVertices = context->createVertexBuffer(foxLayout);

	IndexBufferHandle foxIndices = context->createIndexBuffer(sizeof(GLuint), fox->indices.size(), fox->indices.data());

	std::string vsSource = loadText("Shaders/diffuse.vert");
	std::string fsSource = loadText("Shaders/diffuse.frag");

	ShaderHandle vs = context->createShader(vsSource.c_str(), Shader::Vertex);
	ShaderHandle fs = context->createShader(fsSource.c_str(), Shader::Fragment);

	Shader::List shaders;
	shaders[Shader::Vertex] = vs;
	shaders[Shader::Fragment] = fs;
	ProgramHandle program = context->createProgram(shaders);

	context->tempUseProgram(program);
	context->tempUseVertexBuffer(foxVertices);
	context->tempUseIdxBuffer(foxIndices);

	glUniformMatrix4fv(2, 1, GL_FALSE, (GLfloat*)&worldMat);
	glUniformMatrix4fv(3, 1, GL_FALSE, (GLfloat*)&viewMat);
	glUniformMatrix4fv(4, 1, GL_FALSE, (GLfloat*)&projMat);
	glUniform3fv(5, 1, (GLfloat*)&lightPosition);

	getGlErrorString();

	float angle = 0.f;

	while (window.isOpen())
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat color[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, color);

		angle += 0.0025f;
		Matrix4 rotMat = Matrix4::rotation(0.f, angle, 0.f);
		glUniformMatrix4fv(2, 1, GL_FALSE, (GLfloat*)&rotMat);

		glDrawElements(GL_TRIANGLES, fox->indices.size(), GL_UNSIGNED_INT, nullptr);

		glDisable(GL_MULTISAMPLE);
		glDisable(GL_DEPTH_TEST);

		context->swapBuffer();

		window.processMessages();
	}

	Logger::exit();
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	run();
	return 0;
}


