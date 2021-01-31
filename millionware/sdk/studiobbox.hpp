#pragma once
#include "vector.hpp"

struct mstudiobbox_t {
	int bone_index;
	int group;
	vector3_t bb_min;
	vector3_t bb_max;
	int hitbox_name_index;
	int m_iPad01[3];
	float radius;
	int m_iPad02[4];

	const char* pszHitboxName() const {
		if (hitbox_name_index == 0)
			return "";

		return (reinterpret_cast<const char*>(this) + hitbox_name_index);
	}
};