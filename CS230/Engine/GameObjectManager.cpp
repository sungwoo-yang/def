/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameObjectManager.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 17, 2025
*/

#include "GameObjectManager.h"

void CS230::GameObjectManager::Add(GameObject* object) {
	objects.push_back(object);
}

void CS230::GameObjectManager::Unload() {
	for (auto object : objects) {
		delete object;
	}
	objects.clear();
}

void CS230::GameObjectManager::UpdateAll(double dt) {
	for (auto object : objects) {
		object->Update(dt);
	}
}

void CS230::GameObjectManager::DrawAll(Math::TransformationMatrix camera_matrix) {
	for (auto object : objects) {
		object->Draw(camera_matrix);
	}
}