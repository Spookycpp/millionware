#pragma once

#include <cstdint>

#include "matrix.h"
#include "vector.h"

using quaternion_t = typename float[4];

enum e_hitbox {
    HITBOX_HEAD,
    HITBOX_NECK,
    HITBOX_PELVIS,
    HITBOX_STOMACH,
    HITBOX_THORAX,
    HITBOX_LOWER_CHEST,
    HITBOX_UPPER_CHEST,
    HITBOX_RIGHT_THIGH,
    HITBOX_LEFT_THIGH,
    HITBOX_RIGHT_CALF,
    HITBOX_LEFT_CALF,
    HITBOX_RIGHT_FOOT,
    HITBOX_LEFT_FOOT,
    HITBOX_RIGHT_HAND,
    HITBOX_LEFT_HAND,
    HITBOX_RIGHT_UPPERARM,
    HITBOX_RIGHT_FOREARM,
    HITBOX_LEFT_UPPERARM,
    HITBOX_LEFT_FOREARM,
};

enum e_hit_group {
    HIT_GROUP_INVALID = -1,
    HIT_GROUP_GENERIC,
    HIT_GROUP_HEAD,
    HIT_GROUP_CHEST,
    HIT_GROUP_STOMACH,
    HIT_GROUP_LEFT_ARM,
    HIT_GROUP_RIGHT_ARM,
    HIT_GROUP_LEFT_LEG,
    HIT_GROUP_RIGHT_LEG,
    HIT_GROUP_GEAR = 10
};

struct studio_bone_t {
    int name_index;
    int parent;
    int bone_controller[6];

    vector_t position;
    quaternion_t quaternion;
    vector_t rotation;
    vector_t position_scale;
    vector_t rotation_scale;

    matrix3x4_t pose_to_bone;
    quaternion_t quaternion_alignement;

    int flags;
    int proc_type;
    int proc_index;
    int physics_bone;
    int surface_prop_idx;
    int contents;
    int surf_prop_lookup;
    int unused[7];
};

struct studio_box_t {
    int bone;
    int group;
    vector_t mins;
    vector_t maxs;
    int name_index;
    int pad01[3];
    float radius;
    int pad02[4];
};

struct studio_hitbox_set_t {
    int name_index;
    int hitbox_count;
    int hitbox_index;

    inline studio_box_t *get_hitbox(int index) const {
        return (studio_box_t *) ((uintptr_t) this + hitbox_index) + index;
    }
};

struct studio_hdr_t {
    int id;
    int version;
    long checksum;
    char name_char_array[64];
    int length;
    vector_t eye_position;
    vector_t illium_position;
    vector_t hull_mins;
    vector_t hull_maxs;
    vector_t mins;
    vector_t maxs;
    int flags;
    int bones_count;
    int bone_index;
    int bone_controllers_count;
    int bone_controller_index;
    int hitbox_sets_count;
    int hitbox_set_index;
    int local_anim_count;
    int local_anim_index;
    int local_seq_count;
    int local_seq_index;
    int activity_list_version;
    int events_indexed;
    int textures_count;
    int texture_index;

    inline studio_hitbox_set_t *get_hitbox_set(int index) const {
        if (index > hitbox_sets_count)
            return nullptr;

        return (studio_hitbox_set_t *) ((uintptr_t) this + hitbox_set_index) + index;
    }

    inline studio_box_t *get_hitbox(int index, int set_index) const {
        const auto set = get_hitbox_set(set_index);

        if (!set)
            return nullptr;

        return set->get_hitbox(index);
    }

    inline studio_bone_t *get_bone(int index) const {
        if (index > bones_count)
            return nullptr;

        return (studio_bone_t *) ((uintptr_t) this + bone_index) + index;
    }
};
