#pragma once

//@todo: declare vfuncs instead of virtuals.

#include <string>

#include "macros.h"
#include "vector.h"

#define EVENT_DEBUG_ID_INIT 42
#define EVENT_DEBUG_ID_SHUTDOWN 13

class bf_write;
class bf_read;

struct damage_log_event_data_t {
    int user_id;
    int attacker_id;
    int health;
    int armor;
    std::string weapon;
    int dmg_health;
    int dmg_armor;
    int hit_group;
};

struct damage_log_data_t {
    damage_log_event_data_t data;
    std::string name;
    int alpha;
};

struct footstep_data_t {
    vector_t position;
    int alpha;
};

struct item_purchase_data_t {
    int user_id;
    std::string item;
};

struct grenade_detonate_data_t {
    int entity_id;
    vector_t pos;
    float start_time;
};

class c_game_event {
public:
    DECLARE_VFUNC(1, get_name(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(2, is_reliable(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(3, is_local(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(4, is_empty(const char *key), bool(__thiscall *)(void *, const char *))(key);
    DECLARE_VFUNC(5, get_bool(const char *key, bool default_value = false), bool(__thiscall *)(void *, const char *, bool))
    (key, default_value);
    DECLARE_VFUNC(6, get_int(const char *key, int default_value = 0), int(__thiscall *)(void *, const char *, int))(key, default_value);
    DECLARE_VFUNC(7, get_long(const char *key, unsigned long default_value = 0),
                  unsigned long(__thiscall *)(void *, const char *, unsigned long))
    (key, default_value);
    DECLARE_VFUNC(8, get_float(const char *key, float default_value = 0.0f), float(__thiscall *)(void *, const char *, float))
    (key, default_value);
    DECLARE_VFUNC(9, get_string(const char *key, const char *default_value = ""),
                  const char *(__thiscall *) (void *, const char *, const char *) )
    (key, default_value);
    DECLARE_VFUNC(10, get_wstring(const char *key, const wchar_t *default_value = L""),
                  const wchar_t *(__thiscall *) (void *, const char *, const wchar_t *) )
    (key, default_value);
    DECLARE_VFUNC(11, set_bool(const char *key, bool value), void(__thiscall *)(void *, const char *, bool))(key, value);
    DECLARE_VFUNC(12, set_int(const char *key, int value), void(__thiscall *)(void *, const char *, int))(key, value);
    DECLARE_VFUNC(13, set_long(const char *key, unsigned long value), void(__thiscall *)(void *, const char *, unsigned long))(key, value);
    DECLARE_VFUNC(14, set_float(const char *key, float value), void(__thiscall *)(void *, const char *, float))(key, value);
    DECLARE_VFUNC(15, set_string(const char *key, const char *value), void(__thiscall *)(void *, const char *, const char *))(key, value);
    DECLARE_VFUNC(16, set_wstring(const char *key, const wchar_t *value), void(__thiscall *)(void *, const char *, const wchar_t *))
    (key, value);
};

class c_game_event_listener2 {
public:
    virtual ~c_game_event_listener2() {
    }

    virtual void fire_game_event(c_game_event *event) = 0;
    virtual int get_event_debug_id() = 0;

public:
    int debug_id;
};

class c_game_event_manager2 {
public:
    DECLARE_VFUNC(3, add_listener(void *listener, const char *event_name, bool server_side),
                  bool(__thiscall *)(void *, void *, const char *, bool))
    (listener, event_name, server_side);
    DECLARE_VFUNC(4, find_listener(void *listener, const char *event_name), bool(__thiscall *)(void *, void *, const char *))
    (listener, event_name);
    DECLARE_VFUNC(5, remove_listener(void *listener), bool(__thiscall *)(void *, void *))(listener);
};

class c_event_listener : public c_game_event_listener2 {
public:
    c_event_listener();
    ~c_event_listener();

    void on_fired_game_event(c_game_event *game_event);

    void fire_game_event(c_game_event *game_event) override {
        on_fired_game_event(game_event);
    }

    bool exists(const char *listener);
    bool add(const char *listener);

    int get_event_debug_id() override {
        return EVENT_DEBUG_ID_INIT;
    }
};
