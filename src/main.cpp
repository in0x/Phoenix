#include <cassert>
#include <Windows.h>
#include "Core/obj.hpp"
#include "Core/Math/PhiMath.hpp"
#include "Core/Win32Window.hpp"
#include "Core/Win32GLContext.hpp"
#include "Core/GLShader.hpp"
#include "Tests/MathTests.hpp"

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
Likely structure:
Window -> Interface class with game
Win32Window -> Takes care of Windows specific stuff
and is created on start up
Engine -> actual engine class, gets a window passed to it
on creation
*/

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	using namespace Phoenix;

	Logger::LogOut::get().logToConsole = true;
	Logger::LogOut::get().logToFile = true;

	Tests::RunMathTests();

	std::unique_ptr<Mesh> fox = parseOBJ("Fox/", "RedFox.obj");
	
	assert(fox != nullptr);
	
	WindowConfig config = { 
		1400, 900,
		0,0,
		std::wstring(L"Phoenix"),
		false };

	Win32Window window(config);
	Win32GLContext glc(window.getNativeHandle());

	if (!window.isOpen())
	{
		Logger::Error("Failed to initialize Win32Window");
		return -1;
	}

	GLenum err = glewInit(); // I need to do this when a context is created / made current for the first time.
	if (err != GLEW_OK) 
	{
		Logger::Error("Failed to initialize gl3w");
		return -1;
	}
	
	Matrix4 worldMat = Matrix4::identity();
	Matrix4 viewMat = lookAtRH(Vec3{ 2, 1, -8 }, Vec3{ 0,0,0 }, Vec3{ 0,1,0 });
	Matrix4 projMat = perspectiveRH(70, (float)config.width / (float)config.height, 1, 10000);
	Vec3 lightPosition = Vec3(-5, 3, -5);
	
	GLuint vert = createShader("Shaders/diffuse.vert", GL_VERTEX_SHADER);
	GLuint frag = createShader("Shaders/diffuse.frag", GL_FRAGMENT_SHADER);

	GLuint progHandle = glCreateProgram();

	glAttachShader(progHandle, vert);
	glAttachShader(progHandle, frag);

	glLinkProgram(progHandle);

	glDeleteShader(vert);
	glDeleteShader(frag);

	GLuint vbo;
	GLuint normals_vbo;
	GLuint vao;
	GLuint ibo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * fox->vertices.size(), fox->vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * fox->normals.size(), fox->normals.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vec3), nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vec3), nullptr);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fox->indices.size() * sizeof(GLuint), fox->indices.data(), GL_STATIC_DRAW);
	
	glUseProgram(progHandle);

	glUniformMatrix4fv(2, 1, GL_FALSE, (GLfloat*)&worldMat);
	glUniformMatrix4fv(3, 1, GL_FALSE, (GLfloat*)&viewMat);
	glUniformMatrix4fv(4, 1, GL_FALSE, (GLfloat*)&projMat);
	glUniform3fv(5, 1, (GLfloat*)&lightPosition);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	getGlErrorString();

	float angle = 0.f;

	while (window.isOpen())
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat color[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, color);

		angle += 0.5f; 
		Matrix4 rotMat = Matrix4::rotation(0.f, angle, 0.f);
		glUniformMatrix4fv(2, 1, GL_FALSE, (GLfloat*)&rotMat);

		glDrawElements(GL_TRIANGLES, fox->indices.size(), GL_UNSIGNED_INT, nullptr);

		glDisable(GL_MULTISAMPLE);
		glDisable(GL_DEPTH_TEST);

		glc.SwapBuffer();

		window.processMessages();
	}

	return 0;
}


