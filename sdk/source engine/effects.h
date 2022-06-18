#pragma once
#include "vector.h"

class c_effects {
public:
    virtual ~c_effects(){};

    virtual void beam(const vector_t &Start, const vector_t &End, int model_index, int halo_index, unsigned char frame_start,
                      unsigned char frame_rate, float life, unsigned char width, unsigned char end_width, unsigned char fade_length,
                      unsigned char noise, unsigned char red, unsigned char green, unsigned char blue, unsigned char brightness,
                      unsigned char speed) = 0;

    //-----------------------------------------------------------------------------
    // Purpose: Emits smoke sprites.
    // Input  : origin - Where to emit the sprites.
    //			scale - Sprite scale * 10.
    //			framerate - Framerate at which to animate the smoke sprites.
    //-----------------------------------------------------------------------------
    virtual void smoke(const vector_t &origin, int model_index, float scale, float framerate) = 0;

    virtual void sparks(const vector_t &position, int magnitude = 1, int trail_length = 1, const vector_t *vec_dir = NULL) = 0;

    virtual void dust(const vector_t &pos, const vector_t &dir, float size, float speed) = 0;

    virtual void muzzle_flash(const vector_t &vec_origin, const vector_t &vec_angles, float scale, int type) = 0;

    // like ricochet, but no sound
    virtual void metal_sparks(const vector_t &position, const vector_t &direction) = 0;

    virtual void energy_splash(const vector_t &position, const vector_t &direction, bool explosive = false) = 0;

    virtual void ricochet(const vector_t &position, const vector_t &direction) = 0;

    // FIXME: Should these methods remain in this interface? Or go in some
    // other client-server neutral interface?
    virtual float time() = 0;
    virtual bool is_server() = 0;

    // Used by the playback system to suppress sounds
    virtual void supress_sound_effects(bool suppress) = 0;
};