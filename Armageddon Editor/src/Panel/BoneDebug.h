#pragma once
#include "Scene/Scene.h"
#include "Scene/Entity.h"
class BoneDebug
{
public:
	BoneDebug() = default;
	BoneDebug(Scene& scene) : m_Scene(scene){};
	void ImGuiDraw();
private:
	Scene& m_Scene;

};