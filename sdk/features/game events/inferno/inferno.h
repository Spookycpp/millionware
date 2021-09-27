#pragma once

namespace features::game_events::inferno {
    void on_inferno_startburn(const grenade_detonate_data_t &data);
    void reset();
    void draw();

    std::vector<vector_t> get_points(c_entity *inferno);

    // https://github.com/darshan3105/Convex-Hull---Jarvis-March-Graham-Scan-Kirkpatrick-Seidel-Algorithm/blob/master/classes/JarvisMarch.h
    // read this shit https://en.wikipedia.org/wiki/Gift_wrapping_algorithm
    struct jarvis_march {
        std::vector<point_t> points{};

        static int get_min_y(std::vector<point_t> $points) {
            int index = 0;
            float min_y = $points.at(0).y;

            for (size_t i = 1; i < $points.size(); i++) {
                const point_t p = $points.at(i);

                if (p.y < min_y || p.y == min_y && p.x < $points.at(index).x) {
                    min_y = p.y;
                    index = i;
                }
            }

            return index;
        }

        static int orientation(const point_t &a, const point_t &b, const point_t &c) {
            const auto value = (int) ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));

            if (!value) {
                return 0; // co-linear
            }

            if (value > 0) {
                return 1; // clockwise
            }

            return 2; // counter clockwise
        }

        std::vector<point_t> compute_hull() {
            if (points.size() < 3) {
                return {};
            }

            std::vector<point_t> hull;

            const int l = get_min_y(points);
            int p = l;

            do {
                hull.push_back(points.at(p));
                const int n = points.size();
                int q = (p + 1) % n;

                for (int i = 0; i < n; i++) {
                    if (orientation(points.at(p), points.at(i), points.at(q)) == 2) {
                        q = i;
                    }
                }

                p = q;

            } while (p != l);

            return hull;
        }
    };
} // namespace features::game_events::inferno