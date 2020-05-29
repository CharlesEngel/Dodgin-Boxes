#pragma once

#include <map>
#include <sstream>

struct CharacterDetails
{
	int x;
	int y;
	int x_offset;
	int y_offset;
	uint32_t width;
	uint32_t height;
	uint32_t advance;

	std::map<char, int> kerning_details;
};

enum FontType
{
	FONT_ARIAL = 0
};

class Font
{
public:
	Font(FontType type);
	CharacterDetails chars[127];
	uint32_t total_width;
	uint32_t total_height;
};

// Extracts an int from a string. Used in font loading
void get_next_int(std::stringstream *stream, int &value);