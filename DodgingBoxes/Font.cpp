#include "Font.h"

#include <fstream>
#include <string>

Font::Font(FontType type)
{
	if (type == FONT_ARIAL)
	{
		// This could be read from the file, but I decided typing it would be easier
		total_width = 512;
		total_height = 512;

		// Open .fnt file (Note: I modified the file by adding spaces to make the next part easier)
		std::ifstream file_stream;
		file_stream.open("Resources/ARIAL.fnt");
		std::string line;

		while (!file_stream.eof())
		{
			// Read line of file
			std::getline(file_stream, line);

			// If the line is describing a character
			if (line.substr(0, 4) == "char" && line.substr(0, 5) != "chars")
			{
				std::stringstream ss;
				ss << line;

				// Read which character it is
				int char_index = 0;
				get_next_int(&ss, char_index);

				// Read the character's x-value
				int char_x = 0;
				get_next_int(&ss, char_x);

				// Read the character's y-value
				int char_y = 0;
				get_next_int(&ss, char_y);

				// Read the character's width
				int char_width = 0;
				get_next_int(&ss, char_width);

				// Read the character's height
				int char_height = 0;
				get_next_int(&ss, char_height);

				// Read the amount to offset the character in the x-direction
				int char_x_off = 0;
				get_next_int(&ss, char_x_off);

				// Read the amount to offset the character in the y-direction
				int char_y_off = 0;
				get_next_int(&ss, char_y_off);

				// Read how much to advance after writing the character
				int advance = 0;
				get_next_int(&ss, advance);

				// Fill out struct
				chars[char_index].x = char_x;
				chars[char_index].y = char_y;
				chars[char_index].x_offset = char_x_off;
				chars[char_index].y_offset = char_y_off;
				chars[char_index].width = char_width;
				chars[char_index].height = char_height;
				chars[char_index].advance = advance;
			}
			else if (line.substr(0, 7) == "kerning" && line.substr(0, 8) != "kernings")
			{
				std::stringstream ss;
				ss << line;

				// Read the first character this kerning information is describing
				int char_index_1 = 0;
				get_next_int(&ss, char_index_1);

				// Read the second character this kerning information is describing
				int char_index_2 = 0;
				get_next_int(&ss, char_index_2);

				// Read how much to modify the second value along the x-axis
				int amount = 0;
				get_next_int(&ss, amount);

				chars[char_index_2].kerning_details[char_index_1] = amount;
			}
		}

		file_stream.close();
	}
}

void get_next_int(std::stringstream *stream, int &value)
{
	while (true)
	{
		// Find word
		std::string word;
		*stream >> word;

		// check if word contains a number
		if (std::stringstream(word) >> value)
		{
			break;
		}
	}
}