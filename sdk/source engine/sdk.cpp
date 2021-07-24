#ifndef NOMINMAX
#define NOMINMAX
#endif

#define INVALID_EHANDLE_INDEX 0xFFFFFFFF

#include "entity.h"

#include "../core/cheat/cheat.h"
#include "../core/interfaces/interfaces.h"
#include "../core/util/util.h"

#include "../engine/math/math.h"
#include "../engine/security/xorstr.h"

#include <vector>

c_entity *entity_handle_t::get() const
{
	if (this->handle == 0xFFFFFFFF)
		return nullptr;

	return interfaces::entity_list->get_entity_from_handle(this->handle);
}

bool entity_handle_t::operator ==(const entity_handle_t &other) const
{
	return handle == other.handle;
}

bool entity_handle_t::operator ==(c_entity *entity) const
{
	return get() == entity;
}

bool entity_handle_t::operator !=(const entity_handle_t &other) const
{
	return handle != other.handle;
}

bool entity_handle_t::operator !=(c_entity *entity) const
{
	return get() != entity;
}

bool c_entity::is_grenade() {
    auto classid = this->get_networkable()->get_client_class()->class_id;
    return classid == CBaseCSGrenadeProjectile || classid == CSmokeGrenadeProjectile || classid == CMolotovProjectile || classid == CDecoyProjectile;
}

CUtlVector<c_animation_layer>& c_player::animation_overlay() {
	const static auto offset = netvars::get(CRC_CT("DT_BaseAnimating:m_hLightingOrigin"));

	return *(CUtlVector<c_animation_layer>*)((uintptr_t)this + offset + 0x3c);
}

CUtlVector<matrix3x4_t>& c_player::get_cached_bone_data() {
	return *(CUtlVector<matrix3x4_t>*) ((uintptr_t)this + 0x290C + sizeof(void*));
}

void c_player::set_abs_angles(const vector_t& angle) {
	if (!this) //-V704
		return;

	using Fn = void(__thiscall*)(void*, const vector_t&);
	static auto fn = reinterpret_cast<Fn>(patterns::set_abs_angles);

	return fn(this, angle);
}

void c_player::set_absolute_origin(const vector_t& new_origin) {
	static auto set_abs_origin = reinterpret_cast<void(__thiscall*)(c_player*, const vector_t&)>(patterns::set_absolute_origin);
	set_abs_origin(this, new_origin);
}

vector_t c_player::get_eye_pos() const {
	return get_vec_origin() + get_view_offset();
}

vector_t c_player::extrapolate_position(const vector_t& pos) {
	return pos + this->get_velocity() * TICK_INTERVAL();
}

vector_t c_player::get_hitbox_pos(const int idx) {

	if (idx == -1) {
		return {};
	}

	matrix3x4_t matrices[128];

	if (!get_renderable()->setup_bones(matrices, 128, 0x100, interfaces::global_vars->current_time)) {
		return {};
	}

	const auto model = get_renderable()->get_model();

	if (!model) {
		return {};
	}

	const auto studio_hdr = interfaces::model_info->get_studio_model(model);

	if (!studio_hdr) {
		return {};
	}

	const auto bbox = studio_hdr->get_hitbox(idx, 0);

	if (!bbox) {
		return {};
	}

	const matrix3x4_t& matrix = matrices[bbox->bone];
	const float modifier = bbox->radius != -1.0f ? bbox->radius : 0.0f;

	const auto min = math::vector_transform(bbox->mins - modifier, matrix);
	const auto max = math::vector_transform(bbox->maxs + modifier, matrix);

	return (min + max) * 0.5f;
}

float c_player::get_flash_time() {
	return *reinterpret_cast<float*>(uintptr_t(this) + patterns::get_flash_time());
}

int c_player::max_health() {

	// dangerzone check
	if (interfaces::game_types->get_current_game_type() == GAME_TYPE_FREEFORALL) 
		return 120;

    return 100;
}

int c_player::sequence_activity(int sequence) {
	const auto model = this->get_renderable()->get_model();
	if (!model) {
		return -1;
	}

	const auto hdr = interfaces::model_info->get_studio_model(model);
	if (!hdr) {
		return -1;
	}

	return reinterpret_cast<int(__fastcall*)(void*, studio_hdr_t*, int)>(patterns::get_sequence_activity)(this, hdr, sequence);
}

bool c_player::is_flashed() {
	return get_flash_time() > interfaces::global_vars->current_time;
}

bool c_player::can_shoot() {
	const auto weapon = (c_weapon*)get_active_weapon_handle().get();

	if (!weapon) 
		return false;

	return can_shoot(weapon);
}

bool c_player::can_shoot(c_weapon* weapon) {
	const float server_time = float(this->get_tick_base()) * interfaces::global_vars->interval_per_tick;

	if (this->get_next_attack() > server_time)
		return false;

	if ((weapon->get_item_definition_index() == WEAPON_FAMAS || weapon->get_item_definition_index() == WEAPON_GLOCK) && weapon->get_is_burst_mode() && weapon->get_burst_shots_remaining() > 0)
		return false;

	if (weapon->get_next_primary_attack() > server_time)
		return false;

	if (weapon->get_item_definition_index() == WEAPON_REVOLVER && weapon->get_ready_time() > server_time)
		return false;

	return true;
}

bool c_player::is_alive() {
	return this->get_life_state() == LIFE_STATE_ALIVE;
}

bool c_player::is_enemy() {
	if (!this)
		return false;

	c_entity* player = (c_entity*)this;
	
	// dangerzone check
    if (interfaces::game_types->get_current_game_type() == GAME_TYPE_FREEFORALL)
        return this->get_survival_team() != cheat::local_player->get_survival_team();

	const static auto mp_teammates_are_enemies = interfaces::convar_system->find_convar(xs("mp_teammates_are_enemies"));

	if (!mp_teammates_are_enemies)
		return player->get_team_num() != cheat::local_player->get_team_num();

	if (mp_teammates_are_enemies->get_int())
		return this != cheat::local_player;

	return player->get_team_num() != cheat::local_player->get_team_num();
}

bool c_player::is_visible(c_player* local, const vector_t& src, const vector_t& dst) {
	c_trace_filter filter;

	if (local)
		filter.skip = local;

	trace_t tr;
	interfaces::trace->trace_ray({ src, dst }, MASK_SHOT, &filter, &tr);

	return (tr.hit_ent == this || tr.fraction > 0.99f);
}

bool c_player::is_visible(c_player* local, const vector_t& src) {
	bool ret = false;

	matrix3x4_t matrices[128];

	if (!get_renderable()->setup_bones(matrices, 128, 0x100, interfaces::global_vars->current_time))
		return false;

	const auto model = this->get_renderable()->get_model();

	if (!model)
		return false;

	const auto studio_hdr = interfaces::model_info->get_studio_model(model);

	if (!studio_hdr)
		return false;

	static std::vector< int > hitboxes = {
		HEAD,
		R_CALF, L_CALF,
		R_FOOT, L_FOOT,
		R_HAND, L_HAND,
		R_UPPERARM, R_FOREARM,
	};

	for (const auto& it : hitboxes) {
		const auto bbox = studio_hdr->get_hitbox(it, 0);

		if (!bbox)
			continue;

		const matrix3x4_t& matrix = matrices[bbox->bone];
		const float modifier = bbox->radius != -1.0f ? bbox->radius : 0.0f;

		const auto min = math::vector_transform(bbox->mins - modifier, matrix);
		const auto max = math::vector_transform(bbox->maxs + modifier, matrix);

		const vector_t dst = (min + max) * 0.5f;

		c_trace_filter filter;

		if (local)
			filter.skip = local;

		trace_t tr;
		interfaces::trace->trace_ray({ src, dst }, MASK_SHOT, &filter, &tr);

		if (tr.hit_ent == this || tr.fraction > 0.99f) {
			ret = true;
			break;
		}
	}

	return ret;
}

bool c_player::is_reloading() {
	const auto& reload_layer = animation_overlay().element(1);

	if (reload_layer.owner) {
		const int activity = sequence_activity(reload_layer.sequence);

		return activity == 967 && reload_layer.weight != 0.f;
	}

	return false;
}

bool c_player::is_smoked() {
	return util::line_goes_through_smoke(cheat::local_player->get_eye_pos(), this->get_eye_pos());
}

bool c_player::has_bomb() {
	using has_bomb_t = bool(__thiscall*)(void*);
	static has_bomb_t has_bomb = (has_bomb_t)patterns::has_bomb;

	return has_bomb(this);
}

bool c_player::is_valid(const bool check_alive) {
	if (!this || this->get_networkable()->is_dormant() || !this->is_player())
		return false;

	return check_alive ? this->is_alive() : !this->is_alive();
}

c_econ_item_view *c_economy_item::get_econ_item_view() {
    static auto address = patterns::get_econ_item_view();

    if (!address)
        return nullptr;

    static auto get_econ_item_view_fn = reinterpret_cast<c_econ_item_view *(__thiscall *) (void *)>(*reinterpret_cast<uintptr_t *>(&address + 1) + &address + 5);

    if (!get_econ_item_view_fn)
        return nullptr;

    return get_econ_item_view_fn(this);
}

weapon_info_t *c_weapon::get_info() {
    return interfaces::weapon_system->get_weapon_info(this->get_item_definition_index());
}

int c_weapon::get_weapon_type() {
    switch (this->get_item_definition_index()) {
    case WEAPON_DEAGLE:                return WEAPON_TYPE_PISTOL;
		case WEAPON_ELITE:                 return WEAPON_TYPE_PISTOL;
		case WEAPON_FIVESEVEN:             return WEAPON_TYPE_PISTOL;
		case WEAPON_GLOCK:                 return WEAPON_TYPE_PISTOL;
		case WEAPON_AK47:                  return WEAPON_TYPE_RIFLE;
		case WEAPON_AUG:                   return WEAPON_TYPE_RIFLE;
		case WEAPON_AWP:                   return WEAPON_TYPE_SNIPER;
		case WEAPON_FAMAS:                 return WEAPON_TYPE_RIFLE;
		case WEAPON_G3SG1:                 return WEAPON_TYPE_SNIPER;
		case WEAPON_GALILAR:               return WEAPON_TYPE_RIFLE;
		case WEAPON_M249:                  return WEAPON_TYPE_MG;
		case WEAPON_M4A1:                  return WEAPON_TYPE_RIFLE;
		case WEAPON_MAC10:                 return WEAPON_TYPE_SMG;
		case WEAPON_P90:                   return WEAPON_TYPE_SMG;
		case WEAPON_UMP45:                 return WEAPON_TYPE_SMG;
		case WEAPON_XM1014:                return WEAPON_TYPE_SHOTGUN;
		case WEAPON_BIZON:                 return WEAPON_TYPE_SMG;
		case WEAPON_MAG7:                  return WEAPON_TYPE_SHOTGUN;
		case WEAPON_NEGEV:                 return WEAPON_TYPE_MG;
		case WEAPON_SAWEDOFF:              return WEAPON_TYPE_SHOTGUN;
		case WEAPON_TEC9:                  return WEAPON_TYPE_PISTOL;
		case WEAPON_TASER:                 return WEAPON_TYPE_TASER;
		case WEAPON_HKP2000:               return WEAPON_TYPE_PISTOL;
		case WEAPON_MP7:                   return WEAPON_TYPE_SMG;
		case WEAPON_MP9:                   return WEAPON_TYPE_SMG;
		case WEAPON_NOVA:                  return WEAPON_TYPE_SHOTGUN;
		case WEAPON_P250:                  return WEAPON_TYPE_PISTOL;
		case WEAPON_SCAR20:                return WEAPON_TYPE_SNIPER;
		case WEAPON_SG556:                 return WEAPON_TYPE_RIFLE;
		case WEAPON_SSG08:                 return WEAPON_TYPE_SNIPER;
		case WEAPON_KNIFE:                 return WEAPON_TYPE_KNIFE;
		case WEAPON_FLASHBANG:             return WEAPON_TYPE_GRENADE;
		case WEAPON_HEGRENADE:             return WEAPON_TYPE_GRENADE;
		case WEAPON_SMOKEGRENADE:          return WEAPON_TYPE_GRENADE;
		case WEAPON_MOLOTOV:               return WEAPON_TYPE_GRENADE;
		case WEAPON_DECOY:                 return WEAPON_TYPE_GRENADE;
		case WEAPON_INCGRENADE:            return WEAPON_TYPE_GRENADE;
		case WEAPON_C4:                    return WEAPON_TYPE_INVALID;
		case WEAPON_KNIFE_T:               return WEAPON_TYPE_KNIFE;
		case WEAPON_M4A1_SILENCER:         return WEAPON_TYPE_RIFLE;
		case WEAPON_USP_SILENCER:          return WEAPON_TYPE_PISTOL;
		case WEAPON_CZ75A:                 return WEAPON_TYPE_PISTOL;
		case WEAPON_REVOLVER:              return WEAPON_TYPE_PISTOL;
		case WEAPON_KNIFE_BAYONET:		   return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_CSS:             return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_FLIP:            return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_GUT:             return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_KARAMBIT:        return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_M9_BAYONET:      return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_TACTICAL:        return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_FALCHION:        return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_SURVIVAL_BOWIE:  return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_BUTTERFLY:       return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_PUSH:            return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_CORD:            return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_CANIS:           return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_URSUS:           return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_GYPSY_JACKKNIFE: return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_OUTDOOR:         return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_STILETTO:        return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_WIDOWMAKER:      return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_SKELETON:        return WEAPON_TYPE_KNIFE;
		default:                           return WEAPON_TYPE_INVALID;
    }
}

bool c_weapon::has_scope() {
    const int weapon_index = this->get_item_definition_index();

    return weapon_index == WEAPON_G3SG1 || weapon_index == WEAPON_SCAR20 || weapon_index == WEAPON_AWP || weapon_index == WEAPON_AUG || weapon_index == WEAPON_SG556 || weapon_index == WEAPON_SSG08;
}

bool c_weapon::is_valid(const bool check_clip) {
switch (this->get_weapon_type()) {
			case WEAPON_TYPE_PISTOL:  break;
			case WEAPON_TYPE_SHOTGUN: break;
			case WEAPON_TYPE_SMG:     break;
			case WEAPON_TYPE_RIFLE:   break;
			case WEAPON_TYPE_MG:      break;
			case WEAPON_TYPE_SNIPER:  break;
			default:                  return false;
		}

		return check_clip ? this->get_ammo1() > 0 : true;
}
