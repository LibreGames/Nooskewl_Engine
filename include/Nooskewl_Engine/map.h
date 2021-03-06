#ifndef MAP_H
#define MAP_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

class Map_Entity;
class Map_Logic;
class Speech;
class Tilemap;

class NOOSKEWL_ENGINE_EXPORT Map {
public:
	static const float PAN_BACK_SPEED;

	static void new_game_started();
	static void sit_sleep_callback(void *data);

	Map(std::string map_name, bool been_here_before, int last_visited_time);
	~Map();

	void start_audio();
	void start();
	void end();

	void handle_event(TGUI_Event *event);
	void update_camera();
	bool update();
	void draw(bool use_depth_buffer = true);

	// Try to activate as entity, returns true on success
	bool activate(Map_Entity *entity);
	void choose_action();
	void schedule_destroy(Map_Entity *entity);

	void add_entity(Map_Entity *entity);
	void add_speech(Speech *speech);
	void add_speech(std::string text, Callback callback = NULL, void *callback_data = NULL);
	void change_map(std::string map_name, Point<int> position, Direction direction);
	void set_panning(bool panning);
	void set_pan(Point<float> pan);

	std::vector<Map_Entity *> get_colliding_entities(int layer, Point<int> position, Size<int> size);
	bool is_solid(int layer, Map_Entity *collide_with, Point<int> position, Size<int> size, bool check_entities = true, bool check_tiles = true);
	void check_triggers(Map_Entity *entity);
	void get_new_map_details(std::string &map_name, Point<int> &position, Direction &direction);
	Map_Entity *get_entity(int id);
	Map_Entity *find_entity(std::string name);
	std::string get_map_name();
	Tilemap *get_tilemap();
	Point<float> get_offset();
	Point<float> get_pan();
	std::list<A_Star::Node *> find_path(Point<int> start, Point<int> goal, bool check_solids = true);
	bool is_speech_active();
	Map_Logic *get_map_logic();
	std::vector<Map_Entity *> &get_entities();

	bool save(std::string &out, bool save_player);

private:
	Tilemap *tilemap;
	Point<float> offset;
	bool panning;
	Point<float> pan;
	float pan_angle;
	std::vector<Map_Entity *> entities;

	std::vector<Speech *> speeches;
	Speech *speech;

	std::string map_name;
	std::string new_map_name;
	Point<int> new_map_position;
	Direction new_map_direction;

	Map_Logic *ml;

	A_Star *a_star;

	std::vector<Map_Entity *> entities_to_destroy;

	// Collided with, collider
	std::vector< std::pair<Map_Entity *, Map_Entity *> > collisions;

	bool been_here_before;
};

} // End namespace Nooskewl_Engine

#endif // MAP_H
