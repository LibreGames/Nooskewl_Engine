#ifndef MAIN_H
#define MAIN_H

#ifdef NOOSKEWL_ENGINE_WINDOWS
// Disable warnings about dll-interface
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#ifdef NOOSKEWL_ENGINE_BUILD
#define NOOSKEWL_ENGINE_EXPORT __declspec(dllexport)
#else
#define NOOSKEWL_ENGINE_EXPORT __declspec(dllimport)
#endif

#define GLEW_STATIC
#else
#define NOOSKEWL_ENGINE_EXPORT
#endif

#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined NOOSKEWL_ENGINE_WINDOWS
#include <direct.h>
#include <d3d9.h>
#include <d3dx9.h>
#else
#include <dlfcn.h>
#include <glob.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef __linux__
// X11 pollutes the global namespace and conflicts with some of our names
namespace X11 {
#include <X11/Xcursor/Xcursor.h>
}
#endif
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#ifdef __linux__
namespace X11 {
#endif
#include <SDL2/SDL_syswm.h>
#ifdef __linux__
}
#endif

#include <zlib.h>

#include <tgui3.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// To extract strings to translate
#define TRANSLATE(text) std::string(text)
#define END

namespace Nooskewl_Engine {

typedef void (*Callback)(void *data);

enum Direction {
	DIRECTION_UNKNOWN,
	N,
	S,
	E,
	W,
};

} // End namespace Nooskewl_Engine

#endif // MAIN_H
