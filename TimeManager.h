#pragma once

#include <chrono>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Sleep.h"

using namespace std;
using namespace chrono;

class TimeManager 
{
public:
	unsigned int MAX_UPDATES_PER_FRAME;
	int UPDATES_PER_SECOND;

	microseconds DELTA_TIME;
	milliseconds MIN_SMOOTH_DURATION;

	system_clock::time_point start_time;

	unsigned int frame_count;
	system_clock::time_point last_frame_time;
	duration<double> last_frame_duration;

	unsigned int update_count;
	system_clock::time_point last_update_time;
	duration<double> last_update_duration;

	double smoothed_frames_per_second;
	double smoothed_updates_per_second;

	system_clock::time_point next_update_time;

	system_clock::time_point time_at_pause;
	bool is_game_paused;

public:
	TimeManager();
	TimeManager(int updates_per_second_in, unsigned int max_updates_per_frame_in);
	void incremenetUpdateRate();
	void incrementFrameRate();
	double getUpdateDeltaTime() const;
	unsigned int getMaxUpdatesFrame() const;
	bool  isUpdateWaiting() const;
	float getGameDuration() const;
	int   getUpdateCount() const;
	int   getFrameCount() const;
	float getUpdateRateAverage() const;
	float getFrameRateAverage() const;
	float getUpdateRateInstantaneous() const;
	float getFrameRateInstantaneous() const;
	float getUpdateRateSmoothed() const;
	float getFrameRateSmoothed() const;
	float getLastUpdateDuration() const;
	float getLastFrameDuration() const;
	void  sleepUntilNextUpdate() const;
	void  markNextUpdate();
	void  markNextFrame();
	double getSmoothedFrameRate() const;
	double getSmoothedUpdateRate() const;
	void setUpdatesPerSecond(int updates_per_second_in, int max_updates_per_frame_in);
	void pauseGame();
	void playGame();
};