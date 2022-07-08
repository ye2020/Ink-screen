# ifndef _JSONWEATHER_H
# define _JSONWEATHER_H

bool ParseActualWeather(String content, struct ActualWeather* jgt);
bool ParseFutureWeather(String content, struct FutureWeather* jgt);
bool ParseLifeIndex(String content, struct LifeIndex* jgt);


# endif