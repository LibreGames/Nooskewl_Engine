#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/internal.h"

using namespace Nooskewl_Engine;

static Uint8 *safe_find_char(Uint8 *haystack, char needle, Uint8 *end)
{
	Uint8 *p = haystack;
	while (p != end) {
		if (*p == needle) {
			return p;
		}
		p++;
	}
	return 0;
}

SDL_RWops *CPA::open(std::string filename)
{
#if !defined LOAD_FROM_FILESYSTEM
	if (load_from_filesystem) {
#endif
	// Only do this part if LOAD_FROM_FILESYSTEM is defined
		filename = std::string(SDL_GetBasePath()) + "/data/" + filename;
		SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "rb");
		return file;
#if !defined LOAD_FROM_FILESYSTEM
	}
	if (!exists(filename)) {
		return 0;
	}
	return SDL_RWFromMem(bytes+info[filename].first, info[filename].second);
#endif
}

bool CPA::exists(std::string filename)
{
	return info.find(filename) != info.end();
}

std::vector<std::string> CPA::get_all_filenames()
{
	std::vector<std::string> v;

	if (load_from_filesystem) {
		// Read directory listing from filesystem

		std::vector< std::vector<std::string> > stack;
		std::vector<std::string> name_stack;
		std::vector<std::string> curr;
		List_Directory l(std::string(SDL_GetBasePath()) + "/data/*");
		std::string s;

		while ((s = l.next()) != "") {
			if (s.c_str()[0] != '.') {
				curr.push_back(s);
			}
		}

		stack.push_back(curr);
		name_stack.push_back(std::string(SDL_GetBasePath()) + "/data");

		while (stack.size() > 0) {
			while (stack[0].size() > 0) {
				s = stack[0][0];
				stack[0].erase(stack[0].begin());

				// we only want the filename
				if (s.rfind("/") != std::string::npos) {
					s = s.substr(s.rfind("/") + 1);
				}

				std::string dir_name;

				for (int i = name_stack.size()-1; i >= 0; i--) {
					dir_name += name_stack[i] + "/";
				}

				List_Directory l(dir_name + s + "/*");

				std::string s2;

				curr.clear();

				while ((s2 = l.next()) != "") {
					if (s2.c_str()[0] != '.') {
						curr.push_back(s2);
					}
				}

				if (curr.size() > 0) {
					stack.insert(stack.begin(), curr);
					name_stack.insert(name_stack.begin(), s);
				}
				else {
					std::string filename = dir_name + s;
					filename = filename.substr(filename.rfind("data/") + 5); // chop path leading up to and including data/
					v.push_back(filename);
				}
			}
			stack.erase(stack.begin());
			name_stack.erase(name_stack.begin());
		}
	}
	else {
		std::map< std::string, std::pair<int, int> >::iterator it;

		for (it = info.begin(); it != info.end(); it++) {
			std::pair< std::string, std::pair<int, int> > p = *it;
			v.push_back(p.first);
		}
	}

	return v;
}

CPA::CPA() :
	load_from_filesystem(false)
{
#ifndef LOAD_FROM_FILESYSTEM
	try {

#ifdef __APPLE__
		std::string filename = std::string(SDL_GetBasePath()) + "data.cpa";
		SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "rb");
#else
		List_Directory ld("*.cpa");
		std::string filename;
		SDL_RWops *file = 0;

		while ((filename = ld.next()) != "") {
			file = SDL_RWFromFile(filename.c_str(), "rb");
			if (file != 0) {
				infomsg("Using %s\n", filename.c_str());
				break;
			}
		}
#endif

		if (file) {
			// The last 4 bytes of every gzipped file is the size of the uncompressed data as a 32 bit little endian number
			SDL_RWseek(file, -4, RW_SEEK_END);
			int size = SDL_ReadLE32(file);
			SDL_RWclose(file);

			gzFile f = gzopen(filename.c_str(), "rb");
			bytes = new Uint8[size];
			int read = gzread(f, bytes, size);
			gzclose(f);

			if (read != size) {
				throw Error("Invalid CPA: corrupt gzip file");
			}

			// Read the size of the data (ascii text followed by newline first thing in the file)
			Uint8 *header_end = (Uint8 *)safe_find_char(bytes, '\n', bytes+size);
			if (header_end == 0) {
				throw Error("Invalid CPA: header not present");
			}
			int header_size = (header_end - bytes) + 1;
			int data_size = atoi((char *)bytes);
			if (data_size >= size) {
				throw Error("Invalid CPA: data size > archive size");
			}
			// Skip to the info section at the end
			Uint8 *p = bytes + header_size + data_size;
			// Keep track of the byte offset of each file
			int count = header_size;

			int total_size = 0;

			char line[1000];

			while (p < bytes+size) {
				Uint8 *end = (Uint8 *)safe_find_char(p, '\n', bytes+size);
				if (end == 0) {
					throw Error("Invalid CPA: corrupt info section");
				}
				int len = end-p;
				if (len < 1000) {
					memcpy(line, p, len);
					line[len] = 0;
					char size_text[1000];
					char name_text[1000];
					Uint8 *size_text_end = (Uint8 *)safe_find_char(p, '\t', bytes+size);
					if (size_text_end == 0 || size_text_end - p > 999) {
						throw Error("Invalid CPA: corrupt info section");
					}
					memcpy(size_text, p, size_text_end-p);
					size_text[size_text_end-p] = 0;
					memcpy(name_text, size_text_end+1, end-size_text_end-1);
					name_text[end-size_text_end-1] = 0;
					int file_size = atoi(size_text);
					std::pair<int, int> pair(count, file_size);
					info[name_text] = pair;
					count += file_size;
					total_size += file_size;
				}
				p += len + 1;
			}

			if (total_size > data_size) {
				throw Error("Invalid CPA: total file sizes > data size");
			}
		}
		else {
			throw FileNotFoundError("No CPA archive found");
		}
	}
	catch (Error e) {
		infomsg("Couldn't load CPA, trying to load from the filesystem...\n");
		load_from_filesystem = true;
	}
#endif
}

CPA::~CPA()
{
#ifndef LOAD_FROM_FILESYSTEM
	if (load_from_filesystem == false) {
		delete[] bytes;
	}
#endif
}
