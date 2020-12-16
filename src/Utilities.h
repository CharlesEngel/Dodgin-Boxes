#pragma once

#include <random>
#include <chrono>

#include "AL/al.h"

constexpr auto PI = 3.141592f;

int random_int(int low, int high);

int num_digits(int value);

void read_wav_file(std::string file_name, ALenum &format, ALvoid **data, ALsizei &size, ALsizei &frequency);
