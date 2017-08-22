#pragma once

#include "Render.hpp"
#include <vector>
#include <typeindex>
#include <typeinfo>
//#include <unordered_map>
#include "IRenderContext.hpp"

// Goals:
// Command-based: The user submits commands that specify the desired action
// and required data which is then executed using the GPU. Commands can be 
// stored in buckets, which in turn can be sorted in order to group commands
// with similar data so as to reduce state changes.
//
// Stateless: To prevent the pitfalls of stateful APIs such as OpenGL, where
// a state set by previous call can affect the next call, the system should be
// stateless: Options set by the previous draw call should not have an effect on 
// the next. 

namespace Phoenix
{
	class IRenderContext;

	typedef void(*SubmitFptr)(IRenderContext*, const void*);

	template <class T>
	struct is_submittable
	{
		template <typename U> static std::true_type check(decltype(U::SubmitFunc)*);
		template <typename> static std::false_type check(...);

		typedef decltype(check<T>(0)) result;

	public:
		static const bool value = result::value;
	};

#define SUBMITTABLE() \
		const static SubmitFptr SubmitFunc \
	
	namespace Commands
	{
		struct DrawIndexed
		{
			SUBMITTABLE();

			uint32_t start;
			uint32_t count;

			Primitive::Type primitives;
			VertexBufferHandle vertexBuffer;
			IndexBufferHandle indexBuffer;
			StateGroup state;
		};

		struct DrawLinear
		{
			SUBMITTABLE();

			uint32_t start;
			uint32_t count;

			Primitive::Type primitives;
			VertexBufferHandle vertexBuffer;
			StateGroup state;
		};
	}

	/*
	Concept comes from Molecular Engine: https://blog.molecular-matters.com/2014/12/16/stateless-layered-multi-threaded-rendering-part-3-api-design-details/
	CommandPacket Layout:
	- void* next
		If set, next CommandPacket in chain, else null
		sizeof(void*) bytes
	- SubmitFptr submitFunction
		Ptr to function with which to submit with to context, must be set
		sizeof(SubmitFptr) bytes
	- T command
		Template parameter command struct, any command that can be interpreted
		using a submit function
		sizeof(T) bytes
	- char* auxMemory
		Auxillary memory used to temporarly store additional data needed for
		the command, i.e. the data to be stored in a constant buffer
		n bytes size, specified when creating the CommandPacket
	*/

	typedef void* CommandPacket;

	namespace commandPacket
	{
		static const size_t OFFSET_NEXT_COMMAND_PACKET = 0;
		static const size_t OFFSET_BACKEND_DISPATCH_FUNCTION = OFFSET_NEXT_COMMAND_PACKET + sizeof(CommandPacket);
		static const size_t OFFSET_COMMAND = OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(SubmitFptr);

		SubmitFptr* getSubmitFptr(CommandPacket packet);
		void setSubmitFptr(CommandPacket packet, SubmitFptr dispatchFunction);
		CommandPacket* getNextCommandPacket(CommandPacket packet);
		const CommandPacket loadNextCommandPacket(const CommandPacket packet);
		const SubmitFptr loadSubmitFptr(const CommandPacket packet);
		const void* loadCommand(const CommandPacket packet);

		template <class T>
		size_t getSize(size_t auxMemorySize)
		{
			return OFFSET_COMMAND + sizeof(T) + auxMemorySize;
		};

		template <class T>
		CommandPacket create(size_t auxMemorySize)
		{
			return ::operator new(getSize<T>(auxMemorySize));
		}

		template <class T>
		CommandPacket* getNextCommandPacket(T* command)
		{
			return reinterpret_cast<CommandPacket*>(reinterpret_cast<char*>(command) - OFFSET_COMMAND + OFFSET_NEXT_COMMAND_PACKET);
		}

		template <class T>
		T* getCommand(CommandPacket packet)
		{
			return reinterpret_cast<T*>(reinterpret_cast<char*>(packet) + OFFSET_COMMAND);
		}

		template <class T>
		char* getAuxiliaryMemory(T* command)
		{
			return reinterpret_cast<char*>(command) + sizeof(T);
		}

		void setNextCommandPacket(CommandPacket packet, CommandPacket nextPacket);

		template <class T>
		void setNextCommandPacket(T* command, CommandPacket nextPacket)
		{
			*commandPacket::getNextCommandPacket<T>(command) = nextPacket;
		}
	};

	// NOTE(Phil): Uses global new as a place 
	// holder, will switch over to fixed memory 
	// when linear allocator is implemented. 
	class Renderer
	{
	public:
		Renderer(IRenderContext* rc, uint32_t maxCommands)
			: m_pContext(rc)
			, m_currentIndex(0)
			, m_commands(maxCommands)
		{}

		template <class Command>
		Command* addCommand(/*Key key, */size_t auxMemorySize = 0)
		{
			static_assert(is_submittable<Command>::value, "Commands require a SubmitFunc.");
			
			CommandPacket packet = commandPacket::create<Command>(auxMemorySize);

			// store key and pointer to the data
			{
				//m_keys[current] = key;
				m_commands[m_currentIndex] = packet;
				m_currentIndex++;
			}

			commandPacket::setNextCommandPacket(packet, nullptr);
			commandPacket::setSubmitFptr(packet, Command::SubmitFunc);

			return commandPacket::getCommand<Command>(packet);
		}

		template <class NewCommand, class OldCommand>
		NewCommand* appendCommand(OldCommand* command, size_t auxMemorySize = 0)
		{
			static_assert(is_submittable<NewCommand>::value, "Commands require a SubmitFunc.");

			CommandPacket packet = commandPacket::create<NewCommand>(auxMemorySize);

			commandPacket::setNextCommandPacket<OldCommand>(command, packet);

			commandPacket::setNextCommandPacket(packet, nullptr);
			commandPacket::setSubmitFptr(packet, NewCommand::SubmitFunc);

			return commandPacket::getCommand<NewCommand>(packet);
		}

		void submit()
		{
			for (size_t i = 0; i < m_currentIndex; ++i)
			{
				CommandPacket packet = m_commands[i];
				do
				{
					const SubmitFptr function = commandPacket::loadSubmitFptr(packet);
					const void* command = commandPacket::loadCommand(packet);
					function(m_pContext, command);

					void* previous = packet;
					packet = commandPacket::loadNextCommandPacket(packet);
					free(previous);
				} while (packet != nullptr);
			}

			m_currentIndex = 0;
			m_pContext->swapBuffer();
		}

		VertexBufferHandle createVertexBuffer(const VertexBufferFormat& vbf);
		ShaderHandle createShader(const char* source, Shader::Type shaderType);
		ProgramHandle createProgram(const Shader::List& shaders);
		TextureHandle createTexture();
		FrameBufferHandle createFrameBuffer();
		UniformHandle createUniform(const char* name, Uniform::Type type);

	private:
		IRenderContext* m_pContext;
		size_t m_currentIndex;
		std::vector<void*> m_commands;
	};
}