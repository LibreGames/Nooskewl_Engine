#include "tgui3.h"

TGUI::TGUI(TGUI_Div *main_div, int w, int h) :
	main_div(main_div),
	w(w),
	h(h)
{
}

void TGUI::layout()
{
	layout(main_div);
}

void TGUI::resize(int w, int h)
{
	this->w = w;
	this->h = h;
}

void TGUI::draw()
{
	draw(main_div, 0, 0);
}

void TGUI::layout(TGUI_Div *div)
{
	div->gui = this;
	for (size_t i = 0; i < div->children.size(); i++) {
		layout(div->children[i]);
	}
}

void TGUI::draw(TGUI_Div *div, int x, int y)
{
	div->draw(div->parent, x, y);
	int parent_width, parent_height;
	tgui_get_size(div->parent, div, &parent_width, &parent_height);
	int max_h = 0;
	int dx = x;
	int dy = y;
	for (size_t i = 0; i < div->children.size(); i++) {
		TGUI_Div *d = div->children[i];
		int width, height;
		tgui_get_size(div, d, &width, &height);
		if (dx + width > parent_width) {
			dx = x;
			dy += max_h;
			max_h = 0;
		}
		draw(d, dx+d->padding_left+d->get_right_pos(), dy+d->padding_top);
		if (d->float_right == false) {
			dx += width;
		}
		max_h = height > max_h ? height : max_h;
	}
}

TGUI_Div::TGUI_Div(int w, int h) :
	parent(NULL),
	percent_x(false),
	percent_y(false),
	w(w),
	h(h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false)
{
}

TGUI_Div::TGUI_Div(float percent_w, float percent_h) :
	parent(NULL),
	percent_x(true),
	percent_y(true),
	percent_w(percent_w),
	percent_h(percent_h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false)
{
}

TGUI_Div::TGUI_Div(int w, float percent_h) :
	parent(NULL),
	percent_x(false),
	percent_y(true),
	percent_h(percent_h),
	w(w),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false)
{
}

TGUI_Div::TGUI_Div(float percent_w, int h) :
	parent(NULL),
	percent_x(true),
	percent_y(false),
	percent_w(percent_w),
	h(h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false)
{
}

void TGUI_Div::set_parent(TGUI_Div *div)
{
	parent = div;
	parent->children.push_back(this);
}

void TGUI_Div::set_padding(int padding)
{
	padding_left = padding_right = padding_top = padding_bottom = padding;
}

void TGUI_Div::set_padding(int left, int right, int top, int bottom)
{
	padding_left = left;
	padding_right = right;
	padding_top = top;
	padding_bottom = bottom;
}

void TGUI_Div::set_float_right(bool float_right)
{
	this->float_right = float_right;
}

int TGUI_Div::get_right_pos()
{
	if (float_right == false) {
		return 0;
	}
	int parent_width;
	tgui_get_size(parent->parent, parent, &parent_width, NULL);
	int width;
	tgui_get_size(parent, this, &width, NULL);
	int right = 0;
	for (size_t i = 0; i < parent->children.size(); i++) {
		TGUI_Div *d = parent->children[i];
		if (d == this) {
			break;
		}
		if (d->float_right) {
			int w2;
			tgui_get_size(parent, d, &w2, NULL);
			right += w2;
		}
	}
	return parent_width - (right + width);
}

void tgui_get_size(TGUI_Div *parent, TGUI_Div *div, int *width, int *height)
{
	if (parent == NULL) {
		*width = div->gui->w;
		*height = div->gui->h;
	}
	else {
		int w, h;
		tgui_get_size(parent->parent, parent, &w, &h);
		if (width) {
			if (div->percent_x) {
				if (div->percent_w < 0) {
					int total_w = 0;
					float total_percent = 0.0f;
					for (size_t i = 0; i < parent->children.size(); i++) {
						int this_w = 0;
						TGUI_Div *d = parent->children[i];
						if (d->percent_x) {
							if (d->percent_w < 0) {
								total_percent += -d->percent_w;
							}
							else {
								int w2;
								tgui_get_size(parent, d, &w2, NULL);
								this_w = w2;
							}
						}
						else {
							this_w = d->w + d->padding_left + d->padding_right;
						}
						if (total_w + this_w > w) {
							total_w = 0;
						}
						if (d->float_right == false) {
							total_w += this_w;
						}
						if (d == div) {
							break;
						}
					}
					int remainder = w - total_w;
					if (remainder > 0) {
						*width = remainder * (-div->percent_w / total_percent) - (div->padding_left + div->padding_right);
					}
					else {
						*width = 0;
					}
				}
				else {
					*width = w * div->percent_w;
				}
			}
			else {
				*width = div->w;
			}
		}
		if (height) {
			if (div->percent_y) {
				if (div->percent_h < 0) {
					int total_w = 0;
					int total_h = 0;
					float total_percent = 0.0f;
					int max_h = 0;
					float max_percent = 0.0f;
					for (size_t i = 0; i < parent->children.size(); i++) {
						int this_w = 0;
						int this_h = 0;
						float this_percent = 0.0f;
						TGUI_Div *d = parent->children[i];
						tgui_get_size(parent, d, &this_w, NULL);
						if (d->percent_y) {
							if (d->percent_h < 0) {
								this_percent = -d->percent_h;
							}
							else {
								int h2;
								tgui_get_size(parent, d, NULL, &h2);
								this_h = h2;
							}
						}
						else {
							this_h = d->h + d->padding_top + d->padding_bottom;
						}
						if (total_w + this_w <= w) {
							if (this_h > max_h) {
								max_h = this_h;
							}
							if (this_percent > max_percent) {
								max_percent = this_percent;
							}
						}
						if (total_w + this_w >= w) {
							total_h += max_h;
							total_percent += max_percent;
							if (total_w + this_w > w) {
								max_h = this_h;
								max_percent = this_percent;
								total_w = this_w;
							}
							else {
								max_h = 0;
								max_percent = 0.0f;
								total_w = 0;
							}
						}
						else if (i == parent->children.size()-1) {
							total_percent += max_percent;
						}
						else if (d->float_right == false) {
							total_w += this_w;
						}
					}
					int remainder = h - total_h;
					if (remainder > 0) {
						*height = remainder * (-div->percent_h / total_percent) - (div->padding_top + div->padding_bottom);
					}
					else {
						*height = 0;
					}
				}
				else {
					*height = h * div->percent_h;
				}
			}
			else {
				*height = div->h;
			}
		}
		if (width) {
			*width += div->padding_left + div->padding_right;
		}
		if (height) {
			*height += div->padding_top + div->padding_bottom;
		}
	}
}

#ifdef WITH_SDL
TGUI_Event tgui_event_from_sdl_event(SDL_Event *sdl_event)
{
	TGUI_Event event;

	switch (sdl_event->type) {
		case SDL_KEYDOWN:
			event.type = TGUI_KEY_DOWN;
			event.keyboard.code = sdl_event->key.keysym.sym;
			break;
		case SDL_KEYUP:
			event.type = TGUI_KEY_UP;
			event.keyboard.code = sdl_event->key.keysym.sym;
			break;
		case SDL_MOUSEBUTTONDOWN:
			event.type = TGUI_MOUSE_DOWN;
			event.mouse.button = sdl_event->button.button;
			event.mouse.x = sdl_event->button.x;
			event.mouse.y = sdl_event->button.y;
			break;
		case SDL_MOUSEBUTTONUP:
			event.type = TGUI_MOUSE_UP;
			event.mouse.button = sdl_event->button.button;
			event.mouse.x = sdl_event->button.x;
			event.mouse.y = sdl_event->button.y;
			break;
		case SDL_MOUSEMOTION:
			event.type = TGUI_MOUSE_AXIS;
			event.mouse.button = -1;
			event.mouse.x = sdl_event->motion.x;
			event.mouse.y = sdl_event->motion.y;
			break;
		case SDL_JOYBUTTONDOWN:
			event.type = TGUI_JOY_DOWN;
			event.joystick.button = sdl_event->jbutton.button;
			event.joystick.axis = -1;
			event.joystick.value = 0.0f;
			break;
		case SDL_JOYBUTTONUP:
			event.type = TGUI_JOY_UP;
			event.joystick.button = sdl_event->jbutton.button;
			event.joystick.axis = -1;
			event.joystick.value = 0.0f;
			break;
		case SDL_JOYAXISMOTION:
			event.type = TGUI_JOY_AXIS;
			event.joystick.button = -1;
			event.joystick.axis = sdl_event->jaxis.axis;
			event.joystick.value = float(sdl_event->jaxis.value + 32768) / 65535.0f * 2.0f - 1.0f;
			break;
		default:
			event.type = TGUI_UNKNOWN;
			break;
	}

	return event;
}

TGUI_Event tgui_sdl_handle_event(SDL_Event *sdl_event)
{
	TGUI_Event event = tgui_event_from_sdl_event(sdl_event);

#ifdef TGUI_DEBUG
	switch (event.type) {
		case TGUI_KEY_DOWN:
		case TGUI_KEY_UP:
			printf("[%2d] %d\n", event.type, event.keyboard.code);
			break;
		case TGUI_MOUSE_DOWN:
		case TGUI_MOUSE_UP:
		case TGUI_MOUSE_AXIS:
			printf("[%2d] %d %d,%d\n", event.type, event.mouse.button, event.mouse.x, event.mouse.y);
			break;
		case TGUI_JOY_DOWN:
		case TGUI_JOY_UP:
		case TGUI_JOY_AXIS:
			printf("[%2d] %d %d %f\n", event.type, event.joystick.button, event.joystick.axis, event.joystick.value);
			break;
	}
#endif

	return event;
}
#endif