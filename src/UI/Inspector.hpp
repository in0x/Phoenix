#pragma once

#include <Memory/StackAllocator.hpp>
#include <Core/ECType.hpp>
#include <Core/EntityHandle.hpp>

namespace Phoenix
{
	class CTransform;
	class World;

	typedef void CmdDrawFunc(void* cmd);
	typedef void CmdApplyFunc(void* cmd);

	struct EditorCmd
	{
		EditorCmd* nextCmd;
		void* payload;
		CmdApplyFunc* applyFunc;
	};

	typedef EditorCmd* CmdAllocFunc(StackAllocator& allocator, void* observedObj);

	struct CmdPayloadCTransform
	{
		CTransform* transform;
		float data[9];
		bool bWritten;
	};

	void applyEditorCmdCTransform(void* cmd);
	void drawEditorCmdCTransform(void* cmd);
	EditorCmd* allocEditorCmdTransform(StackAllocator& allocator, void* observedObj);

	void drawEditorCmdNull(void* cmd);
	EditorCmd* allocEditorCmdNull(StackAllocator& allocator, void* observedObj);

	class Inspector
	{
		StackAllocator m_allocator;
		EditorCmd* m_cmdhead;
		EditorCmd* m_cmdtail;
		EntityHandle m_selectedEntity;

		void applyEditorCommands();
		void clearEditorCommands();
		void insertCommand(EditorCmd* cmd);
		void drawEntityFilter();

		bool m_componentFilters[ECType::CT_Max];

	public:
		Inspector(size_t cmdMemoryBytes)
			: m_allocator(cmdMemoryBytes)
			, m_selectedEntity(0)
		{}

		void drawDemoReference();
		void drawEntityList(World* world);
		void drawEntityEditor(World* world);
	};
}