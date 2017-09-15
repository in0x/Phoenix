#pragma once

#include "RenderDefinitions.hpp"

namespace Phoenix
{
	/*
	Concept comes from Molecular Engine: https://blog.molecular-matters.com/2014/12/16/stateless-layered-multi-threaded-rendering-part-3-api-design-details/
	CommandPackets are used to build an intrusive linked list of commands.
	They can be used to enforce a certain order of execution for commands.
	A CommandPacket can optionally contain some auxillary memory used to
	store additional data required for the command. The submit function must
	know how to interpret the auxillary memory.

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

		template <class T, class Allocator>
		CommandPacket create(size_t auxMemorySize, Allocator& allocator)
		{
			//return ::operator new(getSize<T>(auxMemorySize));
			return allocator.allocate(getSize<T>(auxMemorySize));
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

		template <class T>
		const char* loadAuxiliaryMemory(T* command)
		{
			return reinterpret_cast<const char*>(command) + sizeof(T);
		}

		void setNextCommandPacket(CommandPacket packet, CommandPacket nextPacket);

		template <class T>
		void setNextCommandPacket(T* command, CommandPacket nextPacket)
		{
			*commandPacket::getNextCommandPacket<T>(command) = nextPacket;
		}
	};

}