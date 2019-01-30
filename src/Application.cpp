#include "Application.h"

using namespace std;

Application::Application(int width, int height, string title, unsigned int fps) : m_width(width), m_height(height), m_title(title), m_msPerFrame(1000 / fps)
{
    m_fps_last = SDL_GetTicks();
}

bool Application::init()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "Error initializing SDL." << endl;
        return false;
    }

    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_SHOWN);
    if(!m_window) {
        cout << "Error creating the window." << endl;
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(!m_renderer) {
        cout << "Error creating the renderer." << endl;
        return false;
    }

    m_pen = new Pen(m_renderer);
}

void Application::run(int fps)
{
    while(m_run)
    {
        frame();
    }
}

bool Application::frame()
{
    SDL_RenderPresent(m_renderer);

    // To be able to quit
    while(SDL_PollEvent(&m_event))
    {
        if(m_event.type == SDL_QUIT)
            return false;
    }

    /* FPS system */
    m_fps_current = SDL_GetTicks();
    m_fps_delta = m_fps_current - m_fps_last;
    if(m_fps_delta < m_msPerFrame) {
        //cout << m_fps_delta << endl;
        SDL_Delay(m_msPerFrame - m_fps_delta); // ms
    }
    m_fps_last = m_fps_current;

    return true;
}

unsigned int Application::getTimeStep()
{
    return m_msPerFrame;
}

Pen *Application::pen()
{
    return m_pen;
}

Application::~Application()
{
    //dtor
}
