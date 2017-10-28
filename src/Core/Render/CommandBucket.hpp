#pragma once

#include <vector>
#include <typeindex>
#include <typeinfo>

#include "RenderDefinitions.hpp"
#include "CommandPacket.hpp"
#include "../Memory/StackAllocator.hpp"

namespace Phoenix
{
	class RIContext;

	class CommandBucket
	{
	public:
		CommandBucket(uint32_t maxCommands, size_t memorySizeBytes)
			: m_memory(memorySizeBytes)
			, m_currentIndex(0)
			, m_commands(maxCommands)
		{
		}

		template <class Command>
		Command* addCommand(/*Key key, */size_t auxMemorySize = 0)
		{
			static_assert(is_submittable<Command>::value, "Commands require a SubmitFunc.");
			
			CommandPacket packet = commandPacket::create<Command>(auxMemorySize, m_memory);

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

			CommandPacket packet = commandPacket::create<NewCommand>(auxMemorySize, m_memory);

			commandPacket::setNextCommandPacket<OldCommand>(command, packet);

			commandPacket::setNextCommandPacket(packet, nullptr);
			commandPacket::setSubmitFptr(packet, NewCommand::SubmitFunc);

			return commandPacket::getCommand<NewCommand>(packet);
		}

		void submit(RIContext* rc) 
		{
			for (size_t i = 0; i < m_currentIndex; ++i)
			{
				CommandPacket packet = m_commands[i];
				do
				{
					const SubmitFptr function = commandPacket::loadSubmitFptr(packet);
					const void* command = commandPacket::loadCommand(packet);
					function(rc, command);

					void* previous = packet;
					packet = commandPacket::loadNextCommandPacket(packet);
					
				} while (packet != nullptr);
			}

			m_currentIndex = 0;
			m_memory.clear();
		}

	private:
		StackAllocator m_memory;
		size_t m_currentIndex;
		std::vector<void*> m_commands;
	};
}