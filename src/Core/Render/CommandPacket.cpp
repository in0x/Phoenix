#include "CommandPacket.hpp"

namespace Phoenix
{
	namespace commandPacket
	{
		CommandPacket* getNextCommandPacket(CommandPacket packet)
		{
			return reinterpret_cast<CommandPacket*>(reinterpret_cast<char*>(packet) + OFFSET_NEXT_COMMAND_PACKET);
		}

		SubmitFptr* getSubmitFptr(CommandPacket packet)
		{
			return reinterpret_cast<SubmitFptr*>(reinterpret_cast<char*>(packet) + OFFSET_BACKEND_DISPATCH_FUNCTION);
		}

		void setNextCommandPacket(CommandPacket packet, CommandPacket nextPacket)
		{
			*commandPacket::getNextCommandPacket(packet) = nextPacket;
		}

		void setSubmitFptr(CommandPacket packet, SubmitFptr dispatchFunction)
		{
			*commandPacket::getSubmitFptr(packet) = dispatchFunction;
		}

		const CommandPacket loadNextCommandPacket(const CommandPacket packet)
		{
			return *getNextCommandPacket(packet);
		}

		const SubmitFptr loadSubmitFptr(const CommandPacket packet)
		{
			return *getSubmitFptr(packet);
		}

		const void* loadCommand(const CommandPacket packet)
		{
			return reinterpret_cast<char*>(packet) + OFFSET_COMMAND;
		}
	};
}