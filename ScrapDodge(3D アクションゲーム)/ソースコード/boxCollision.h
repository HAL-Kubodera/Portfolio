/*============================================================
	[boxCollision.h]
	Å•ëËñº : î†å^ÇÃìñÇΩÇËîªíË
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _BOX_COLLISION_H_
#define _BOX_COLLISION_H_

#include "collision.h"

class BoxCollision : public Collision
{
private:
	static constexpr int AXIS_COUNT = 3;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;
	bool ObbCheck(Collision* col);

	std::vector<bool> GetIsHit();

	const char* GetName()
	{
		return m_GuiName.c_str();
	}

	void DispUniqueGui() override
	{
		if (ImGui::TreeNode(GetName()))
		{
			Collision::DispUniqueGui();

			for (int i = 0; i < m_TargetTag.size(); i++)
			{
				if (ImGui::BeginCombo(("Tag" + std::to_string(i)).c_str(), TagNames[static_cast<int>(m_TargetTag[i])]))
				{
					for (int tagNum = 0; tagNum < Tag_Max; tagNum++)
					{
						if (ImGui::Selectable(TagNames[tagNum]))
						{
							SetTargetTag((Tag)tagNum);
						}
					}

					ImGui::EndCombo();
				}
				
				bool hit = m_IsHit[i];

				ImGui::Checkbox(("hit" + std::to_string(i)).c_str(), &hit);

				for (auto obj : m_HitObjects[i])
				{
					if (ImGui::TreeNode(obj->GetName()))
					{
						obj->DispUniqueGui();

						ImGui::TreePop();
					}
				}

				ImGui::Separator();
			}

			ImGui::Checkbox("disp", &m_IsDisp);

			ImGui::TreePop();
		}
	}
};

#endif // _BOX_COLLISION_H_