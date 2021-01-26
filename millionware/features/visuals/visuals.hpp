#include <functional>
#include <mutex>
#include <optional>

#include "../../sdk/entity.hpp"
#include "../../sdk/vector.hpp"

namespace features::visuals {
	struct bounding_box_t {
		point_t min = -1;
		point_t max = -1;

		constexpr bounding_box_t(const int32_t left, const int32_t top, const int32_t right, const int32_t bottom)
			: min(left, top), max(right, bottom)
		{
		}
	};

	std::optional<bounding_box_t> calculate_box(c_entity* entity);

	void draw_esp_box(const bounding_box_t& box, int32_t box_type, color_t color);
	void draw_esp_name(const bounding_box_t& box, c_entity* entity, color_t color);
	void draw_esp_health(const bounding_box_t& box, c_player* player, bool draw_text, color_t color, color_t text_color);
	void draw_esp_armor(const bounding_box_t& box, c_player* player, bool draw_text, color_t color, color_t text_color);
	void draw_esp_ammo(const bounding_box_t& box, c_entity* entity, bool draw_text, color_t color, color_t text_color);
	void draw_esp_flags(const bounding_box_t& box, c_player* player, int32_t flags, int32_t offset, color_t color);
	void draw_player_esp(c_player* local_player, c_player* player);
}