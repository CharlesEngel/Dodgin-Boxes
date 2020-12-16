#include "Utilities.h"

#include <stdio.h>

int random_int(int low, int high)
{
	static std::mt19937 generator(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
	std::uniform_int_distribution<int> distribution(low, high);

	return distribution(generator);
}

int num_digits(int value)
{
	int num = 0;
	while (value != 0)
	{
		num++;
		value /= 10;
	}

	return num;
}

/*void read_wav_file(std::string file_name, ALenum &format, ALvoid **data, ALsizei &size, ALsizei &frequency)
{
	unsigned char* buffer = 0;

	// open and read all file contents
	FILE* f = 0;
	fopen_s(&f, file_name.c_str(), "rb");

	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = new unsigned char[file_size];

	fread(buffer, 1, file_size, f);
	fclose(f);

	// ensure the file type is a wave file, identified by RIFF
	if (memcmp(buffer, "RIFF", 4) != 0)
	{
		throw std::runtime_error("Invalid wave file!");
	}

	// pull out different useful data parameters from buffer
	unsigned int channels = buffer[22] | (buffer[23] << 8);
	unsigned int sample_rate = buffer[24] | (buffer[25] << 8) | (buffer[26] << 16) | (buffer[27] << 24);
	unsigned int bps = buffer[34] | (buffer[35] << 8);

	// skip `useless to us` chunks
	size_t chunk_offset = 36;
	while (memcmp(buffer + chunk_offset, "data", 4) != 0) {
		chunk_offset += 4;
		chunk_offset += buffer[chunk_offset] | (buffer[chunk_offset + 1] << 8) | (buffer[chunk_offset + 2] << 16) | (buffer[chunk_offset + 3] << 24);
		chunk_offset += 4;
	}
	unsigned char* b_data = &(buffer[chunk_offset + 8]);

	format = bps == 8 ? (channels == 1 ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8) : (channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
	frequency = sample_rate;
	*data = buffer;
	size = static_cast<ALsizei>(file_size - chunk_offset);
}*/
