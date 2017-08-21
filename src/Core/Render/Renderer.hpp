#pragma once

#include "Render.hpp"
#include <vector>

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

	// NOTE(Phil): Uses global new as a place 
	// holder, will switch over to fixed memory 
	// when linear allocator is implemented. 
	class Renderer
	{
	public:
		Renderer(uint32_t maxCommands)
			: m_currentIndex(0)
			, m_commands(maxCommands)
		{}

		template<class Command>
		Command* create()
		{
			//static_assert(std::is_pod<Command>::value, "Command type should be plain-old-data.");
			//static_assert(is_submittable<Command>::value, "Command should be submittable");

			Command* command = new Command();
			command->submitFunc = Command::SUBMIT_FUNC;

			m_commands[m_currentIndex] = static_cast<void*>(command);
			m_currentIndex++;
			return command;
		}

		void submit(IRenderContext* rc);

	private:
		size_t m_currentIndex;
		std::vector<void*> m_commands;
	};
}