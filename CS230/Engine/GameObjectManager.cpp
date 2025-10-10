/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameObjectManager.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 17, 2025
*/

#include "../Engine/Engine.h"
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
	std::vector<GameObject*> destroy_objects;

	for (auto object : objects) {
		object->Update(dt);

		if (object->Destroyed()) {
			destroy_objects.push_back(object);
		}
	}

	for (GameObject* obj : destroy_objects) {
		objects.remove(obj);
		delete obj;
	}
}

void CS230::GameObjectManager::DrawAll(Math::TransformationMatrix camera_matrix) {
	for (auto object : objects) {
		object->Draw(camera_matrix);
	}
}

void CS230::GameObjectManager::CollisionTest() {
	for (GameObject* object_1 : objects) {
		for (GameObject* object_2 : objects) {
			if ((object_1 != object_2) && (object_1->CanCollideWith(object_2->Type()))) {

				if (object_1->IsCollidingWith(object_2)) {
					Engine::GetLogger().LogEvent("Collision Detected: " + object_1->TypeName() + " and " + object_2->TypeName());
					object_1->ResolveCollision(object_2);
				}
			}
		}
	}
}