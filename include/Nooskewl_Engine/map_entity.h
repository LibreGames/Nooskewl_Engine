#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

class Brain;
class Map;
class Sprite;
class Stats;

class NOOSKEWL_ENGINE_EXPORT Map_Entity {
public:
	static void new_game_started();

	enum Type {
		NPC,
		OTHER
	};

	enum Shadow_Type {
		SHADOW_NONE,
		SHADOW_TRANSLUCENT_COPY
	};

	Map_Entity(std::string name);
	~Map_Entity();

	void set_brain(Brain *brain);
	void load_sprite(std::string name);
	void set_sprite(Sprite *sprite);
	void set_position(Point<int> position);
	void set_size(Size<int> size);
	void set_offset(Point<float> offset);
	void set_direction(Direction direction);
	void set_solid(bool solid);
	void set_sitting(bool sitting);
	void set_path(std::list<A_Star::Node *> path, Callback callback = NULL, void *callback_data = NULL);
	void set_shadow_type(Shadow_Type shadow_type);
	void set_input_enabled(bool enabled);
	void set_type(Type type);
	void load_stats(std::string name);
	void set_stats(Stats *stats);
	void set_low(bool low);
	void set_high(bool high);
	void set_z(int z);
	void set_z_add(int z_add);
	void set_pre_sit_position(Point<int> pre_sit_position);
	void lock_sit_direction(bool lock);

	int get_id();
	std::string get_name();
	Brain *get_brain();
	Sprite *get_sprite();
	Direction get_direction();
	Point<int> get_position();
	Point<float> get_offset();
	Size<int> get_size();
	Point<float> get_draw_position();
	bool is_solid();
	bool is_sitting();
	Shadow_Type get_shadow_type();
	bool is_following_path();
	bool is_input_enabled();
	Type get_type();
	Stats *get_stats();
	bool is_low();
	bool is_high();
	int get_z();

	// Positions in pixels
	bool pixels_collide(Point<int> position, Size<int> size);
	// Positions in tiles
	bool tiles_collide(Point<int> position, Size<int> size, Point<int> &collide_pos);
	bool entity_collides(Map_Entity *entity);

	void stop();
	void handle_event(TGUI_Event *event);
	// return false to destroy
	bool update(bool can_move);
	// draws with z values
	void draw(Point<float> draw_pos, bool use_depth_buffer = true);
	void draw_shadows(Point<float> draw_pos);

	bool save(SDL_RWops *file);

private:
	bool maybe_move();
	void stop_now();
	void follow_path();
	void end_a_star();
	void set_next_blink();

	int id;
	std::string name;
	Direction direction;
	Sprite *sprite;
	Brain *brain;
	Point<int> position;
	bool moving;
	float speed;
	Point<float> offset;
	bool solid;
	Size<int> size;
	bool stop_next_tile;
	bool sitting;
	bool input_enabled;

	bool following_path;
	std::list<A_Star::Node *> path;
	Callback path_callback;
	void *path_callback_data;

	Shadow_Type shadow_type;

	bool has_blink;
	float eye_colour[4];
	float blink_colour[4];
	Uint32 next_blink;

	Type type;
	Stats *stats;

	bool low;
	bool high;
	int z;
	int z_add;

	bool pre_sit_position_set;
	Point<int> pre_sit_position;

	bool sit_direction_locked;
};

} // End namespace Nooskewl_Engine

#endif // MAP_ENTITY_H
