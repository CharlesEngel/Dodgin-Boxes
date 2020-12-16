#pragma once

#include <vector>
#include <string>

#include "al.h"
#include "alc.h"

#include "glm/glm.hpp"

enum SoundType
{
	SOUND_TYPE_DASH = 0,
	SOUND_TYPE_DEATH = 1,
	SOUND_TYPE_ENEMY = 2,
	SOUND_TYPE_MENU = 3,
	SOUND_TYPE_MUSIC = 4
};

struct WAVInfo
{
	ALvoid *data;
	void *buffer;
	ALenum format;
	ALsizei size;
	ALsizei frequency;
};

class SoundManager
{
public:
	~SoundManager();
	static SoundManager &get_instance();
	size_t register_sound(SoundType type);
	void delete_sound(size_t sound);
	void update_sound_position(size_t sound, float x, float y, float z);
	void update_sound_velocity(size_t sound, float x, float y, float z);
	void update_sound_loop(size_t sound, bool loop);
	void update_sound_relative(size_t sound, bool relative);
	void update_sound_max_distance(size_t sound, float max_distance);
	void update_sound_gain(size_t sound, float gain);
	bool is_sound_playing(size_t sound);
	void play_sound(size_t sound);
	void pause_sound(size_t sound);
	void stop_sound(size_t sound);

	void update_listener_position(float x, float y, float z);
	void update_listener_velocity(float x, float y, float z);
	void update_listener_orientation(float at_x, float at_y, float at_z, float up_x, float up_y, float up_z);

	void reset();

	glm::vec3 get_listener_location();
	glm::vec3 get_listener_velocity();

private:
	SoundManager(std::vector<std::string> sound_files);
	void load_wav_file(std::string file_name, WAVInfo &info);
	void free_wav_file(WAVInfo &info);

	std::vector<ALuint> sources;
	std::vector<ALuint> buffers;

	glm::vec3 listener_location;
	glm::vec3 listener_velocity;

	ALCdevice *device;
	ALCcontext *context;
};
