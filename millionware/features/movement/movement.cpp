#include "movement.hpp"

#include "../../core/cheat.hpp"
#include "../../core/config.hpp"
#include "../../core/interfaces.hpp"
#include "../../core/patterns.hpp"

#include "../../utils/hash/hash.hpp"
#include "../../utils/xorstr/xorstr.hpp"
#include "../../utils/input/input.hpp"

void features::movement::pre_prediction(user_cmd_t* user_cmd) {

	if (config::get<bool>(FNV_CT("misc.movement.infinite_duck"))) {
		user_cmd->buttons |= BUTTON_IN_BULLRUSH;
	}

	if (config::get<bool>(FNV_CT("misc.movement.bunny_hop")))
	{
		if (cheat::local_player->move_type() == MOVE_TYPE_LADDER || input::is_hotkey_active(FNV_CT("misc.movement.jump_bug.hotkey")))
			return;

		if (!(cheat::local_player->flags() & ENTITY_FLAG_ONGROUND))
			user_cmd->buttons &= ~BUTTON_IN_JUMP;
	}
}

void features::movement::post_prediction(user_cmd_t* user_cmd, int pre_flags, int post_flags) {

	if (config::get<bool>(FNV_CT("misc.movement.jump_bug")) && input::is_hotkey_active(FNV_CT("misc.movement.jump_bug.hotkey"))) {
		cheat::b_predicting = true;

		if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND) {
			user_cmd->buttons |= BUTTON_IN_DUCK;
		}

		if (post_flags & ENTITY_FLAG_ONGROUND) {
			user_cmd->buttons &= ~BUTTON_IN_JUMP;
		}
	}

	if (config::get<bool>(FNV_CT("misc.movement.edge_bug")) && input::is_hotkey_active(FNV_CT("misc.movement.edge_bug.hotkey"))) {
		if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND)
			user_cmd->buttons |= BUTTON_IN_DUCK;
	}

	if (config::get<bool>(FNV_CT("misc.movement.edge_jump")) && input::is_hotkey_active(FNV_CT("misc.movement.edge_jump.hotkey"))) {
		if (pre_flags & ENTITY_FLAG_ONGROUND && !(post_flags & ENTITY_FLAG_ONGROUND))
			user_cmd->buttons |= BUTTON_IN_JUMP;
	}

}

void features::movement::strafe_optimizer(user_cmd_t* user_cmd, int pre_flags, int post_flags) {

	// if menu element isn't enabled and the hotkey isn't being pressed, exit out.
	if (!config::get<bool>(FNV_CT("misc.movement.strafe_optimizer")) && !input::is_hotkey_active(FNV_CT("misc.movement.strafe_optimizer.key")))
		return;

	// save old yaw for next call.
	static float old_yaw = user_cmd->view_angles.y;

	// if the player's dead, exit out.
	if (cheat::local_player->life_state() != LIFE_STATE_ALIVE)
		return;

	// self explanatory.
	static auto sensitivity = interfaces::convar_system->find(STR_ENC("sensitivity"));

	// get networked velocity.
	vector3_t velocity = cheat::local_player->velocity();

	// get the velocity length 2d (speed).
	float speed = velocity.length_2d();

	// compute the ideal strafe angle for our velocity.
	float ideal = (speed > 0.f) ? math::rad_to_deg(std::asin(config::get<float>(FNV_CT("misc.movement.strafe_optimizer.max_gain")) / 100.f) * 30.f) / speed : 0.f;

	// get our viewangle change.
	auto yaw_delta = math::strafe_opti_normalize_angle(user_cmd->view_angles.y - old_yaw, 180);

	// convert to absolute change.
	auto absolute_yaw_delta = fabsf(yaw_delta);

	// making the ideal strafe grab correct values.
	auto ideal_strafe = std::copysignf(ideal, yaw_delta);

	// only strafe in air.
	if (!(pre_flags & ENTITY_FLAG_ONGROUND) && !(post_flags & ENTITY_FLAG_ONGROUND) && fabsf(yaw_delta) > 0.f && speed > 150.f && absolute_yaw_delta > (fabsf(ideal) / 10.f)) {

		// if the local players velocity breaches
		// the set velocity limit, exit out.
		if (speed >= config::get<float>(FNV_CT("misc.movement.strafe_optimizer.velocity_max")))
			return;

		// cbf to comment more, rest is self explanatory.
		auto error_margin = ideal_strafe - yaw_delta;

		float target_angle = old_yaw + yaw_delta + (error_margin * config::get<float>(FNV_CT("misc.movement.strafe_optimizer.pull_amount")) / 100.f);

		yaw_delta = target_angle - old_yaw;

		auto moused_x = floor(floor((yaw_delta / sensitivity->get_float()) / 0.022));
		auto humanized_angle = moused_x * sensitivity->get_float() * 0.022;

		user_cmd->view_angles.y = humanized_angle + old_yaw;
	}
	old_yaw = user_cmd->view_angles.y;

	interfaces::engine_client->set_view_angles(user_cmd->view_angles);
}
