#pragma once

struct SDL_Window;

#ifdef ARTS_HAVE_FFMPEG
bool PlayIntroVideo(SDL_Window* window, const char* filepath);
#else
inline bool PlayIntroVideo(SDL_Window*, const char*)
{
    return false;
}
#endif
