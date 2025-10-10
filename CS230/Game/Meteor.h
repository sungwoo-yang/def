/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#pragma once
#include "../Engine/GameObject.h"

class Meteor : public CS230::GameObject {
public:
	Meteor();
	void Update(double dt) override;
private:
	static constexpr double default_velocity = 100.0;
};