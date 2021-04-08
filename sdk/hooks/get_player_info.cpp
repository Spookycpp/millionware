#include "../engine/security/xorstr.h"
#include "../core/hooks/hooks.h"

bool __fastcall hooks::get_player_info(c_engine_client *ecx, uintptr_t edx, int ent_index, player_info_t &buffer)
{
	const auto result = get_player_info_original(ecx, edx, ent_index, buffer);

	if (!result || false /* !config::get< bool >( CRC( "visuals, other, privacy filter" ) ) */)
		return false;

	if (!buffer.fake_player && !buffer.hltv)
	{
		buffer.xuid = 0;

		memset(buffer.name, 0, sizeof buffer.name);

		sprintf_s(buffer.name, XORSTR("Player %d"), buffer.user_id);
	}

	return true;
}