#include "starsquatters.h"

Font::Font() :
	font(NULL)
{
}

Font::~Font()
{
	if (font) {
		TTF_CloseFont(font);
	}
}

bool Font::load_ttf(SDL_RWops *file, int size)
{
	font = TTF_OpenFontRW(file, true, size);
	if (font == NULL) {
		return false;
	}
	return true;
}

void Font::clear_cache()
{
	std::multimap<int, Glyph *>::iterator it = glyphs.begin();
	while (it != glyphs.end()) {
		std::pair<int, Glyph *> p = *it;
		Glyph *g = p.second;
		delete g->image;
		delete g;
		it = glyphs.erase(it);
	}
}

void Font::draw(std::string text, float x, float y, SDL_Color color)
{
	cache(text, color);
	const char *p = text.c_str();
	while (*p) {
		std::pair<std::multimap<int, Glyph *>::const_iterator, std::multimap<int, Glyph *>::const_iterator> matches = glyphs.equal_range(*p);
		std::multimap<int, Glyph *>::const_iterator it = matches.first;
		Glyph *found = NULL;
		while (it != matches.second) {
			const std::pair<int, Glyph *> pair = *it;
			Glyph *g = pair.second;
			if (memcmp(&g->color, &color, sizeof(SDL_Color)) == 0) {
				found = g;
				break;
			}
			it++;
		}
		if (found != NULL) {
			found->image->bind();
			found->image->draw(x, y, Image::FLIP_V);
			x += found->image->width;
		}

		p++;
	}
}

void Font::cache(int ch, SDL_Color color)
{
	Glyph *g = new Glyph;
	if (g == NULL) {
		logmsg("Error caching glyph");
		return;
	}

	SDL_Surface *surface = TTF_RenderGlyph_Solid(font, ch, color);
	if (surface == NULL) {
		logmsg("Error rendering glyph");
		delete g;
		return;
	}

	g->image = new Image();
	if (g->image == NULL || g->image->from_surface(surface) == false) {
		logmsg("Error converting glyph from surface to image");
		SDL_FreeSurface(surface);
		delete g;
		return;
	}

	g->color = color;

	std::pair<int, Glyph *> p;
	p.first = ch;
	p.second = g;

	glyphs.insert(p);
}

void Font::cache(std::string text, SDL_Color color)
{
	const char *p = text.c_str();
	while (*p) {
		std::pair<std::multimap<int, Glyph *>::const_iterator, std::multimap<int, Glyph *>::const_iterator> matches = glyphs.equal_range(*p);
		std::multimap<int, Glyph *>::const_iterator it = matches.first;
		bool found = false;
		while (it != matches.second) {
			const std::pair<int, Glyph *> g = *it;
			if (memcmp(&g.second->color, &color, sizeof(SDL_Color)) == 0) {
					found = true;
				break;
			}
			it++;
		}
		if (found == false) {
			cache(*p, color);
		}
		p++;
	}
}

bool init_font()
{
	if (TTF_Init() == -1) {
		return false;
	}

	return true;
}

void shutdown_font()
{
	TTF_Quit();
}