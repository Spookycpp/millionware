#pragma once

#include "../engine/debug/debug_overlay.h"

#include "../core/cheat/cheat.h"
#include "../core/interfaces/interfaces.h"
#include "../core/hooks/hooks.h"

bool __fastcall hooks::write_user_cmd_delta_to_buffer(uintptr_t ecx, uintptr_t edx, int slot, bf_write *buf, int from, int to, bool new_user_cmd) {

	PROFILE_WITH(write_user_cmd);

	if (!cheat::tick_base_shift)
		return write_user_cmd_delta_to_buffer_original(ecx, edx, slot, buf, from, to, new_user_cmd);

	if (from != -1)
		return true;

	const int to_shift     = std::abs(cheat::tick_base_shift);
	cheat::tick_base_shift = 0;

	// CCLMsg_Move
	int *new_cmds        = reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(buf) - 0x2C);
	int *backup_commands = reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(buf) - 0x30);

	const int new_commands       = *new_cmds;
	const int total_new_commands = std::min(new_commands + to_shift, 62);

	*new_cmds = total_new_commands;
	*backup_commands = 0;

	const int next_command_number = interfaces::client_state->last_command + interfaces::client_state->choked_commands + 1;

	from = -1;
	to   = next_command_number - new_commands + 1;

	for (; to <= next_command_number; to++) {
		if (!write_user_cmd_delta_to_buffer_original(ecx, edx, slot, buf, from, to, true)) {
			return false;
		}

		from = to;
	}

	c_user_cmd *last_real_command = interfaces::input->get_user_cmd(slot, from);

	if (last_real_command) {

		c_user_cmd from_command = *last_real_command;
		c_user_cmd to_command   = from_command;

		to_command.command_number++;
		to_command.tick_count += 200;

		for (int i = new_commands; i <= total_new_commands; i++) {

			static auto write_user_cmd = patterns::get_write_user_command();

			__asm {
				mov     ecx, buf
				mov     edx, to_command
				push	from_command
				call    write_user_cmd
				add     esp, 4
			}

			from_command = to_command;

			to_command.command_number++;
			to_command.tick_count++;
		}
	}

	return true;
}