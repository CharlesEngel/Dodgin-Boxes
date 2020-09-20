#include "SoundManager.h"

SoundManager::~SoundManager()
{
	alDeleteBuffers(static_cast<ALsizei>(buffers.size()), buffers.data());
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

SoundManager &SoundManager::get_instance()
{
	static SoundManager instance({ "Resources/dash_sound.wav", "Resources/death_sound.wav", "Resources/enemy_sound.wav", "Resources/menu_sound.wav", "Resources/music.wav" });
	return instance;
}

size_t SoundManager::register_sound(SoundType type)
{
	ALuint source;
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, buffers[type]);
	sources.push_back(source);

	return sources.size() - 1;
}

void SoundManager::delete_sound(size_t sound)
{
	alDeleteSources(1, &sources[sound]);
}

void SoundManager::update_sound_position(size_t sound, float x, float y, float z)
{
	ALfloat source_pos[] = { x, y, z };
	alSourcefv(sources[sound], AL_POSITION, source_pos);
}

void SoundManager::update_sound_velocity(size_t sound, float x, float y, float z)
{
	ALfloat source_vel[] = { x, y, z };
	alSourcefv(sources[sound], AL_VELOCITY, source_vel);
}

void SoundManager::update_sound_loop(size_t sound, bool loop)
{
	alSourcei(sources[sound], AL_LOOPING, ALboolean(loop));
}

void SoundManager::update_sound_relative(size_t sound, bool relative)
{
	alSourcei(sources[sound], AL_SOURCE_RELATIVE, relative);
}

void SoundManager::update_sound_max_distance(size_t sound, float max_distance)
{
	alSourcef(sources[sound], AL_REFERENCE_DISTANCE, max_distance);
}

void SoundManager::update_sound_gain(size_t sound, float gain)
{
	alSourcef(sources[sound], AL_GAIN, gain);
}

bool SoundManager::is_sound_playing(size_t sound)
{
	ALint state;
	alGetSourcei(sources[sound], AL_SOURCE_STATE, &state);

	return state == AL_PLAYING;
}

void SoundManager::play_sound(size_t sound)
{
	ALint state;
	alGetSourcei(sources[sound], AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING)
	{
		alSourceStop(sources[sound]);
	}

	alSourcePlay(sources[sound]);
}

void SoundManager::pause_sound(size_t sound)
{
	alSourcePause(sources[sound]);
}

void SoundManager::stop_sound(size_t sound)
{
	alSourceStop(sources[sound]);
}

void SoundManager::update_listener_position(float x, float y, float z)
{
	listener_location = { x, y, z };

	ALfloat listener_pos[] = { x, y, z };
	alListenerfv(AL_POSITION, listener_pos);
}

void SoundManager::update_listener_velocity(float x, float y, float z)
{
	listener_velocity = { x, y, z };

	ALfloat listener_vel[] = { x, y, z };
	alListenerfv(AL_VELOCITY, listener_vel);
}

void SoundManager::update_listener_orientation(float at_x, float at_y, float at_z, float up_x, float up_y, float up_z)
{
	ALfloat listener_ori[] = { at_x, at_y, at_z, up_x, up_y, up_z };
	alListenerfv(AL_ORIENTATION, listener_ori);
}

glm::vec3 SoundManager::get_listener_location()
{
	return listener_location;
}

glm::vec3 SoundManager::get_listener_velocity()
{
	return listener_velocity;
}

void SoundManager::reset()
{
	for (auto source : sources)
	{
		ALenum source_state;
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);

		while (source_state == AL_PLAYING)
		{
			alGetSourcei(source, AL_SOURCE_STATE, &source_state);
		}

		alDeleteSources(1, &source);
	}
}

SoundManager::SoundManager(std::vector<std::string> sound_files)
{
	// Setup audio device and context
	device = alcOpenDevice(NULL);

	if (device == NULL)
	{
		throw std::runtime_error("Failed to initialize audio device!");
	}

	context = alcCreateContext(device, NULL);

	if (context == NULL || !alcMakeContextCurrent(context))
	{
		throw std::runtime_error("Failed to create audio context!");
	}

	WAVInfo info = {};

	buffers.resize(sound_files.size());
	alGenBuffers(static_cast<ALsizei>(buffers.size()), buffers.data());

	for (size_t i = 0; i < sound_files.size(); i++)
	{
		load_wav_file(sound_files[i], info);

		alBufferData(buffers[i], info.format, info.data, info.size, info.frequency);

		free_wav_file(info);
	}
}

void SoundManager::load_wav_file(std::string file_name, WAVInfo &info)
{
	unsigned char* buffer = 0;

	// open and read file
	FILE* f = 0;
	fopen_s(&f, file_name.c_str(), "rb");

	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = new unsigned char[file_size];

	fread(buffer, 1, file_size, f);
	fclose(f);

	// check for RIFF
	if (memcmp(buffer, "RIFF", 4) != 0)
	{
		throw std::runtime_error("Invalid wave file!");
	}

	// retrieve data
	unsigned int channels = buffer[22] | (buffer[23] << 8);
	unsigned int sample_rate = buffer[24] | (buffer[25] << 8) | (buffer[26] << 16) | (buffer[27] << 24);
	unsigned int bps = buffer[34] | (buffer[35] << 8);
	info.size = buffer[40] | (buffer[41] << 8) | (buffer[42] << 16) | (buffer[43] << 24);

	// skip chunks
	size_t chunk_offset = 36;
	while (memcmp(buffer + chunk_offset, "data", 4) != 0) {
		chunk_offset += 4;
		chunk_offset += buffer[chunk_offset] | (buffer[chunk_offset + 1] << 8) | (buffer[chunk_offset + 2] << 16) | (buffer[chunk_offset + 3] << 24);
		chunk_offset += 4;
	}
	unsigned char* b_data = &(buffer[chunk_offset + 8]);

	info.format = bps == 8 ? (channels == 1 ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8) : (channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
	info.frequency = sample_rate;
	info.data = b_data;
	info.buffer = buffer;
}

void SoundManager::free_wav_file(WAVInfo &info)
{
	delete info.buffer;

	info.format = 0;
	info.data = nullptr;
	info.buffer = nullptr;
	info.frequency = 0;
	info.size = 0;
}