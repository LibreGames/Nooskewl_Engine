#ifndef STATS_H
#define STATS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Item;
class Inventory;

class Stats {
public:
	enum Alignment {
		GOOD,
		NEUTRAL,
		EVIL
	};

	std::string name;

	Alignment alignment;

	uint16_t hp, max_hp;
	uint16_t mp, max_mp;
	uint16_t attack;
	uint16_t defense;
	uint16_t agility;
	uint16_t karma;
	uint16_t luck;
	uint16_t speed;
	uint16_t strength;
	uint32_t experience;

	Item *weapon;
	Item *armour;

	Inventory *inventory;

	Stats(std::string name);

	void defaults();
	bool load(std::string name);

private:
	void handle_tag(XML *xml);
};

} // End namespace Nooskewl_Engine

#endif // STATS_H