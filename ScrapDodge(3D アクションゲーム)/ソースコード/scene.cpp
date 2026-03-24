/*============================================================
	[scene.cpp]
	▼題名 : シーンの基底クラス
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "GameObject.h"
#include "camera.h"
#include "fade.h"

#include <list>
#include "scene.h"
#include <typeinfo>
#include <string>

#include "collision.h"

void Scene::Init()
{
}



void Scene::Uninit()
{
	for (int i = 0; i < Layer_Num; i++)
	{
		for (auto GameObject : m_GameObject[i])
		{
			GameObject->Uninit();
			delete GameObject;
		}

		m_GameObject[i].clear();
	}
}

void Scene::Update()
{
	for (int i = 0; i < Layer_Num; i++)
	{
		for (auto GameObject : m_GameObject[i])
		{
			GameObject->Update();
		}
	}

	for (int i = 0; i < Layer_Num; i++)
	{
		m_GameObject[i].remove_if([](GameObject* object)
			{
				return object->Destroy();
			});
	}
}

void Scene::Draw()
{
	Camera* pCamera = GetGameObject<Camera>();
	std::list<GameObject*> drawList = m_GameObject[object];

	if (pCamera != nullptr)
	{
		Vector3 cameraPos = pCamera->GetPosition();

		drawList.sort([&](GameObject* object1, GameObject* object2)
			{
				return object1->GetDistance(cameraPos) > object2->GetDistance(cameraPos);
			});
	}
	
	for (int i = 0; i < Layer_Num; i++)
	{
		if (i == object)
		{
			for (auto GameObject : drawList)
			{
				GameObject->Draw();
			}
		}
		else
		{
			for (auto GameObject : m_GameObject[i])
			{
				GameObject->Draw();
			}
		}
	}

	Fade::Draw();

#ifdef _DEBUG

	// 親ウィンドウを作成
	ImGui::Begin("Hierarchy", nullptr);

	ImGuiID dockspace_id = ImGui::GetID("HierarchyDock");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

	static bool first_time[2] = { true,true };
	if (first_time[0])
	{
		first_time[0] = false;

		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id);
		ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

		// 中央ノードに各レイヤーを自動でドッキング
		for (int layer = 0; layer < Layer_Num; layer++)
		{
			std::string layerName = "Layer";

			switch (layer)
			{
			case camera:
				layerName = std::to_string(layer) + ":" + "camera";
				break;
			case field:
				layerName = std::to_string(layer) + ":" + "field";
				break;
			case area:
				layerName = std::to_string(layer) + ":" + "area";
				break;
			case laser:
				layerName = std::to_string(layer) + ":" + "laser";
				break;
			case object:
				layerName = std::to_string(layer) + ":" + "object";
				break;
			case screen:
				layerName = std::to_string(layer) + ":" + "screen";
				break;
			case ui:
				layerName = std::to_string(layer) + ":" + "ui";
				break;
			default:
				break;
			}

			ImGui::DockBuilderDockWindow(layerName.c_str(), dockspace_id);
		}

		ImGui::DockBuilderFinish(dockspace_id);
	}

	ImGui::End();

	// 親ウィンドウを作成
	ImGui::Begin("Inspecter", nullptr);

	dockspace_id = ImGui::GetID("InspecterDock");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

	if (first_time[1])
	{
		first_time[1] = false;

		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id);
		ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

		std::string layerName = "GameObject";

		ImGui::DockBuilderDockWindow(layerName.c_str(), dockspace_id);

		ImGui::DockBuilderFinish(dockspace_id);
	}

	ImGui::End();

	for (int layer = 0; layer < Layer_Num; layer++)
	{
		std::string layerName = "Layer";

		switch (layer)
		{
		case camera:
			layerName = std::to_string(layer) + ":" + "camera";
			break;
		case field:
			layerName = std::to_string(layer) + ":" + "field";
			break;
		case area:
			layerName = std::to_string(layer) + ":" + "area";
			break;
		case laser:
			layerName = std::to_string(layer) + ":" + "laser";
			break;
		case object:
			layerName = std::to_string(layer) + ":" + "object";
			break;
		case screen:
			layerName = std::to_string(layer) + ":" + "screen";
			break;
		case ui:
			layerName = std::to_string(layer) + ":" + "ui";
			break;
		default:
			break;
		}

		ImGui::Begin(layerName.c_str());
		int Num = 1;
		for (auto gameObject : m_GameObject[layer])
		{
			std::string name = std::to_string(Num) + ":" + gameObject->GetName();

			if (ImGui::Selectable(name.c_str()))
			{
				gameObject->SetShowInspecter(!gameObject->GetShowInspecter());

				for (int lay = 0; lay < Layer_Num; lay++)
				{
					int objNum = 1;

					for (auto obj : m_GameObject[lay])
					{
						if (!(objNum == Num && lay == layer))
						{
							obj->SetShowInspecter(false);
						}

						objNum++;
					}
				}
			}

			if (gameObject->GetShowInspecter())
			{
				ImGui::Begin("GameObject");

				DEBUG_DATA* pData = gameObject->GetDebugData();
				std::list<MyComponent*>* components = pData->m_Components;

				ImGui::Separator();

				gameObject->DispUniqueGui();

				ImGui::Separator();
				
				// マテリアル編集GUI
				ImGui::Text("component");

				for (auto component : *components)
				{
					std::string componentName = component->GetName();

					DEBUG_COMPONENT_DATA* dcData = component->GetDebugData();

					Vector3* pos = dcData->m_Position;
					Vector3* scale = dcData->m_Scale;
					Vector3* rot = dcData->m_Rotation;

				    component->DispUniqueGui();
				}
				
				ImGui::Separator();

				ImGui::End();
			}
			

			Num++;
		}
		ImGui::End();
	}
#endif
}

void Scene::LayerDraw(Layer layer)
{
	//オブジェクトの場合最初に書く順番を整理する
	if (layer == object)
	{
		Camera* pCamera = GetGameObject<Camera>();
		std::list<GameObject*> drawList = m_GameObject[object];

		if (pCamera != nullptr)
		{
			Vector3 cameraPos = pCamera->GetPosition();

			drawList.sort([&](GameObject* object1, GameObject* object2)
				{
					return object1->GetDistance(cameraPos) > object2->GetDistance(cameraPos);
				});
		}

		for (auto GameObject : drawList)
		{
			GameObject->Draw();
		}
	}
	else
	{
		for (auto GameObject : m_GameObject[layer])
		{
			GameObject->Draw();
		}
	}
}
