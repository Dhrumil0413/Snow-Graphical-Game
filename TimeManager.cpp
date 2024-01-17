#include "TimeManager.h"
#include <iostream>

using namespace std;



TimeManager::TimeManager()
{
    MAX_UPDATES_PER_FRAME = 10;
    UPDATES_PER_SECOND = 60;

    DELTA_TIME = microseconds(1000000 / UPDATES_PER_SECOND);
    MIN_SMOOTH_DURATION = milliseconds(5);

    start_time = system_clock::now();

    frame_count = 0;
    last_frame_time = start_time;
    last_frame_duration = DELTA_TIME;

    update_count = 0;
    last_update_time = start_time;
    last_update_duration = DELTA_TIME;

    next_update_time = start_time;

    smoothed_frames_per_second = UPDATES_PER_SECOND;
    smoothed_updates_per_second = UPDATES_PER_SECOND;

    time_at_pause = start_time;
    is_game_paused = false;
}

TimeManager::TimeManager(int updates_per_second_in, unsigned int max_updates_per_frame_in)
{
    UPDATES_PER_SECOND = updates_per_second_in;
    MAX_UPDATES_PER_FRAME = max_updates_per_frame_in;

    DELTA_TIME = microseconds(1000000 / UPDATES_PER_SECOND);
    MIN_SMOOTH_DURATION = milliseconds(5);

    start_time = system_clock::now();

    frame_count = 0;
    last_frame_time = start_time;
    last_frame_duration = DELTA_TIME;

    update_count = 0;
    last_update_time = start_time;
    last_update_duration = DELTA_TIME;

    next_update_time = start_time;

    smoothed_frames_per_second = UPDATES_PER_SECOND;
    smoothed_updates_per_second = UPDATES_PER_SECOND;

    time_at_pause = start_time;
    is_game_paused = false;
}

void TimeManager::incremenetUpdateRate()
{
    update_count++;
}

void TimeManager::incrementFrameRate()
{
    frame_count++;
}

double TimeManager::getUpdateDeltaTime() const
{
    return duration<float>(DELTA_TIME).count();
}

unsigned int TimeManager::getMaxUpdatesFrame() const
{
    return MAX_UPDATES_PER_FRAME;
}

bool TimeManager::isUpdateWaiting() const
{
    system_clock::time_point current_time = system_clock::now();
    if (current_time < next_update_time) {  
        double second = duration<float>(next_update_time - current_time).count();
        std::cout << "CurrentTime: " << second << std::endl;
        //std::cout << "NextUpdateTime: " << next_update_time << std::endl;
        return true;
    }
    return false;
}

float TimeManager::getGameDuration() const
{
    if (is_game_paused) {
        return duration<float>(time_at_pause - start_time).count();
    }
    system_clock::time_point current_time = system_clock::now();
    return duration<float>(current_time - start_time).count();
}

int TimeManager::getUpdateCount() const
{
    return update_count;
}

int TimeManager::getFrameCount() const
{
    return frame_count;
}

float TimeManager::getUpdateRateAverage() const
{
    return update_count / getGameDuration();
}

float TimeManager::getFrameRateAverage() const
{
    return frame_count / getGameDuration();
}

float TimeManager::getUpdateRateInstantaneous() const
{
    double last_update_seconds = duration<double>(last_update_duration).count();

    return 1.0f / last_update_seconds;
}

float TimeManager::getFrameRateInstantaneous() const
{
    double last_frame_seconds = duration<double>(last_frame_duration).count();

    return 1.0f / last_frame_seconds;
}

float TimeManager::getUpdateRateSmoothed() const
{
    return 0.95f * smoothed_updates_per_second + 0.05f * getUpdateRateInstantaneous();
}

float TimeManager::getFrameRateSmoothed() const
{
    return 0.95f * smoothed_frames_per_second + 0.05f * getFrameRateInstantaneous();
}

float TimeManager::getLastUpdateDuration() const
{
    return duration<float>(last_update_duration).count();
}

float TimeManager::getLastFrameDuration() const
{
    return duration<float>(last_frame_duration).count();
}

void TimeManager::sleepUntilNextUpdate() const
{
    system_clock::time_point current_time = system_clock::now();
    if (current_time < next_update_time) {
        sleep(duration<double>(next_update_time - current_time).count());
    }
}

void  TimeManager::markNextUpdate()
{
    system_clock::time_point current_time = system_clock::now();
    last_update_duration = current_time - last_update_time;

    if (last_update_duration <  MIN_SMOOTH_DURATION) {
        last_update_duration = MIN_SMOOTH_DURATION;
    }

    last_update_time = current_time;
    smoothed_updates_per_second = getUpdateRateSmoothed();

    next_update_time += DELTA_TIME;
}
	
void  TimeManager::markNextFrame()
{
    system_clock::time_point current_time = system_clock::now();
    last_frame_duration = current_time - last_frame_time;

    if (last_frame_duration < MIN_SMOOTH_DURATION)
        last_frame_duration = MIN_SMOOTH_DURATION;

    last_frame_time = current_time;
    
    smoothed_frames_per_second = getFrameRateSmoothed();
}

double TimeManager::getSmoothedFrameRate() const 
{
    return smoothed_frames_per_second;
}

double TimeManager::getSmoothedUpdateRate() const
{
    return smoothed_updates_per_second;
}

void TimeManager::setUpdatesPerSecond(int updates_per_second_in, int max_updates_per_frame_in)
{
    UPDATES_PER_SECOND = updates_per_second_in;
    MAX_UPDATES_PER_FRAME = max_updates_per_frame_in;
    DELTA_TIME = microseconds(1000000 / UPDATES_PER_SECOND);

}

void TimeManager::pauseGame()
{
    is_game_paused = true;
    system_clock::time_point current_time = system_clock::now();
    time_at_pause = current_time;
}

void TimeManager::playGame()
{
    is_game_paused = false;
    system_clock::time_point current_time = system_clock::now();
    start_time += current_time - time_at_pause;
}