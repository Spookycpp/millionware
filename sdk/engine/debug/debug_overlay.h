#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#ifdef _DEBUG
#define PROFILE_WITH(timer) debug_timer_t timer_{(debug_overlay::timer).get()};
#else
#define PROFILE_WITH(timer) ;
#endif

struct debug_overlay_t {
    uint64_t calls;
    uint64_t avg_per_sec;

    std::string name;
    std::vector<uint64_t> times;
    std::mutex mutex;

    debug_overlay_t(std::string name);

    void add(uint64_t time);
    void draw(float pos_x, float pos_y);
};

struct debug_timer_t {
    debug_overlay_t *overlay;
    std::chrono::steady_clock::time_point start;

    debug_timer_t(debug_overlay_t *overlay);
    ~debug_timer_t();
};

namespace debug_overlay {

    inline std::shared_ptr<debug_overlay_t> create_move;
    inline std::shared_ptr<debug_overlay_t> do_psfx;
    inline std::shared_ptr<debug_overlay_t> dme;
    inline std::shared_ptr<debug_overlay_t> fsn[8];
    inline std::shared_ptr<debug_overlay_t> present[3];
    inline std::shared_ptr<debug_overlay_t> push_notice;
    inline std::shared_ptr<debug_overlay_t> send_datagram;
    inline std::shared_ptr<debug_overlay_t> write_user_cmd;

    void init();
    void draw();

} // namespace debug_overlay
