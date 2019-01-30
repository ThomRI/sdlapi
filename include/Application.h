#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "Pen.h"

class Application
{
    public:
        Application(int width, int height, std::string title = "App", unsigned int fps = 60);
        virtual ~Application();

        bool init();

        void run(int fps);
        inline bool frame();

        unsigned int getTimeStep();

        Pen *pen();

    protected:

    private:
        /* SDL */
        SDL_Window *m_window = 0;
        SDL_Renderer *m_renderer = 0;
        SDL_Event m_event;
        bool m_run = true;

        /* Window */
        int m_width, m_height;
        std::string m_title;

        /* FPS system */
        unsigned int    m_fps_last, m_fps_current, m_fps_delta, // Attribute to avoid excessive allocations
                        m_msPerFrame;

        /* API */
        Pen *m_pen;
};

#endif // APPLICATION_H
