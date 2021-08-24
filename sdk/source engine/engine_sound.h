#pragma once

struct c_snd_info {
	int guid;
	void *file_handle;
	int sound_source;
	int channel;
	int speaker_entity;
	float volume;
	float last_spatilized_volume;
	float radius;
	int pitch;
	vector_t *origin;
	vector_t *direction;
	bool update_positions;
	bool is_sentence;
	bool disable_dsp_processing;
	bool from_speaker_entity;
	bool server_sound;
};

class c_engine_sound {
public:
	DECLARE_VFUNC(19, get_active_sounds(CUtlVector<c_snd_info> &list), void(__thiscall *)(void *, CUtlVector<c_snd_info> &))(list);
};

