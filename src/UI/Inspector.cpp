#include "Inspector.hpp"

#include <UI/PhiImGui.h>
#include <Core/World.hpp>
#include <Core/Components/CTransform.hpp>

namespace Phoenix
{
	void applyEditorCmdCTransform(void* cmd)
	{
		EditorCmd* command = (EditorCmd*)cmd;
		CmdPayloadCTransform* payload = (CmdPayloadCTransform*)command->payload;

		if (!payload->bWritten)
		{
			return;
		}

		float* data = payload->data;

		payload->transform->setTranslation({ data[0], data[1], data[2] });
		payload->transform->setRotation({ data[3], data[4], data[5] });
		payload->transform->setScale({ data[6], data[7], data[8] });
	}

	void drawEditorCmdCTransform(void* cmd)
	{
		EditorCmd* command = (EditorCmd*)cmd;
		CmdPayloadCTransform* payload = (CmdPayloadCTransform*)command->payload;
		float* data = payload->data;
		bool bWritten = false;

		bWritten |= ImGui::InputFloat3("Translation", &data[0], ImGuiInputTextFlags_EnterReturnsTrue);
		bWritten |= ImGui::InputFloat3("Rotation", &data[3], ImGuiInputTextFlags_EnterReturnsTrue);
		bWritten |= ImGui::InputFloat3("Scale", &data[6], ImGuiInputTextFlags_EnterReturnsTrue);

		payload->bWritten = bWritten;
	}

	EditorCmd* allocEditorCmdTransform(StackAllocator& allocator, void* observedObj)
	{
		EditorCmd* cmd = alloc<EditorCmd>(allocator);
		cmd->applyFunc = applyEditorCmdCTransform;

		CTransform* transform = (CTransform*)observedObj;
		CmdPayloadCTransform* payload = alloc<CmdPayloadCTransform>(allocator);
		payload->transform = transform;

		auto setDataFromVec = [](float* data, const Vec3& vec) {
			data[0] = vec.x;
			data[1] = vec.y;
			data[2] = vec.z;
		};

		setDataFromVec(payload->data, transform->getTranslation());
		setDataFromVec(&payload->data[3], transform->getRotation());
		setDataFromVec(&payload->data[6], transform->getScale());

		cmd->payload = payload;

		return cmd;
	}

	void drawEditorCmdNull(void* cmd) 
	{
	}

	EditorCmd* allocEditorCmdNull(StackAllocator& allocator, void* observedObj) 
	{
		return nullptr; 
	}

	CmdDrawFunc* DrawCmdFuncTbl[ECType::CT_Max] =
	{
		&drawEditorCmdCTransform,
		&drawEditorCmdNull,
		&drawEditorCmdNull,
		&drawEditorCmdNull,
	};

	CmdAllocFunc* AllocCmdFuncTbl[ECType::CT_Max] =
	{
		&allocEditorCmdTransform,
		&allocEditorCmdNull,
		&allocEditorCmdNull,
		&allocEditorCmdNull,
	};

	void Inspector::drawDemoReference()
	{
		static bool checkBox = false;
		ImGui::Checkbox("Demo Window", &checkBox);

		if (checkBox)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
			ImGui::ShowDemoWindow(&checkBox);
		}
	}

	void Inspector::drawEntityFilter()
	{
		if (ImGui::TreeNode("Filter Entities"))
		{
			ImGui::Text("Only show entities with the following components:");

			bool* currentFilter = &m_componentFilters[0];

			for (const char* ectype : g_ecTypeNames)
			{
				ImGui::Selectable(ectype, currentFilter);
				currentFilter++;
			}

			ImGui::TreePop();
		}
	}

	void Inspector::drawEntityList(World* world)
	{
		ImGui::Begin("World");

		drawEntityFilter();

		const bool bCreatedEntity = ImGui::Button("Create Entity");
		if (bCreatedEntity)
		{
			world->createEntity();
		}

		for (size_t handle = World::FIRST_VALID_ENTITY; handle <= world->m_lastEntityIdx; ++handle)
		{
			bool bShowEntity = false;

 			for (size_t filterIdx = 0; filterIdx < ECType::CT_Max; ++filterIdx)
			{
				if (m_componentFilters[filterIdx])
				{
					bShowEntity |= world->getComponent(handle, (ECType)filterIdx) != nullptr;
				}
			}

			if (!bShowEntity)
			{
				continue;
			}

			bool bSelected = ImGui::MenuItem("Entity");

			ImGui::SameLine();
			ImGui::Text("%lld", handle);

			char bufCtx[32];
			sprintf(bufCtx, "Ctx%d", handle);

			if (ImGui::BeginPopupContextWindow(bufCtx))
			{
				if (ImGui::Button("Destroy"))
				{
					//world->destroyEntity(m_selectedEntity);
				}

				ImGui::EndPopup();
			}
			else if (bSelected)
			{
				m_selectedEntity = handle;
			}
		}

		if (bCreatedEntity)
		{
			m_selectedEntity = world->m_lastEntityIdx;
		}

		ImGui::End();
	}

	void Inspector::applyEditorCommands()
	{
		while (m_cmdhead)
		{
			m_cmdhead->applyFunc(m_cmdhead);
			m_cmdhead = m_cmdhead->nextCmd;
		}
	}

	void Inspector::clearEditorCommands()
	{
		m_allocator.clear();
		m_cmdhead = nullptr;
	}

	void Inspector::insertCommand(EditorCmd* cmd)
	{
		if (!cmd)
		{
			return;
		}

		if (!m_cmdhead)
		{
			m_cmdhead = cmd;
			m_cmdtail = cmd;
		}

		m_cmdtail->nextCmd = cmd;
		m_cmdtail = cmd;
		cmd->nextCmd = nullptr;
	}

	void Inspector::drawEntityEditor(World* world)
	{
		applyEditorCommands();
		clearEditorCommands();

		if (!world->handleIsValid(m_selectedEntity))
		{
			return;
		}

		Entity* entity = world->getEntity(m_selectedEntity);

		ImGui::Begin("Inspector");
		ImGui::Text("Entity %lld", m_selectedEntity);

		for (auto& kvp : entity->m_components)
		{
			Component* component = kvp.second;

			ImGui::BeginGroup();
			ImGui::Text(component->typeName());

			CmdAllocFunc* allocCmd = AllocCmdFuncTbl[component->type()];
			EditorCmd* cmd = allocCmd(m_allocator, component);

			CmdDrawFunc* drawFunc = DrawCmdFuncTbl[component->type()];
			drawFunc(cmd);

			insertCommand(cmd);

			ImGui::EndGroup();
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 255, 255));
		}

		ImGui::End();
	}

}