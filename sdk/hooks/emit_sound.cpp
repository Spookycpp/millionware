#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"
#include "../core/util/util.h"

#include "../engine/input/input.h"
#include "../engine/logging/logging.h"
#include "../engine/render/render.h"
#include "../engine/security/xorstr.h"

void __fastcall hooks::emit_sound(uintptr_t ecx, uintptr_t edx, uintptr_t filter, int entity_index, int channel, const char *sound_entry,
                                  int sound_entry_hash, const char *sample_name, float volume, float attenuation, int seed, int flags,
                                  int pitch, const vector_t *origin, const vector_t *direction, vector_t *utl_vec_origins,
                                  bool update_positions, float sound_time, int speaker_entity, uintptr_t fds) {

    const std::string beep_entry = xs("UIPanorama.popup_accept_match_beep");
    if (!interfaces::engine_client->is_in_game() && settings.miscellaneous.auto_accept && sound_entry == beep_entry) {
        util::auto_accept();

        const std::string accept_beep = sample_name;
        if (accept_beep.compare(xs("competitive_accept_beep")) != std::string::npos) {
            volume = 0.0f;
        }
    }

    if (interfaces::engine_client->is_in_game() && entity_index == interfaces::engine_client->get_local_player()) {
        const std::string land_sound = sample_name;
        if (land_sound.compare(xs("land")) != std::string::npos && cheat::impact_sound) {
            cheat::impact_sound = false;
            cheat::landed = true;
            return;
        }

        const std::string suit_sound = sample_name;
        if (suit_sound.compare(xs("suit")) != std::string::npos && cheat::landed) {
            cheat::landed = false;
            return;
        }
    }

    if (cheat::predicting)
        volume = 0.f;

    emit_sound_original(ecx, edx, filter, entity_index, channel, sound_entry, sound_entry_hash, sample_name, volume, attenuation, seed,
                        flags, pitch, origin, direction, utl_vec_origins, update_positions, sound_time, speaker_entity, fds);
}