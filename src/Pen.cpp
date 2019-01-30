#include "Pen.h"

Pen::Pen(SDL_Renderer *renderer) : m_renderer(renderer)
{
    //ctor
}

void Pen::setPos(int x, int y)
{
    m_x = x;
    m_y = y;
}

void Pen::setColor(int r, int g, int b)
{
    m_r = r; m_g = g; m_b = b;
    SDL_SetRenderDrawColor(m_renderer, r, g, b, 255); // alpha not supported
}

void Pen::setBackgroundColor(int r, int g, int b)
{
    m_back_r = r; m_back_g = g; m_back_b = b;
}

void Pen::drawSquare(SDL_Rect &rect)
{
    SDL_RenderFillRect(m_renderer, &rect);
}

void Pen::drawSquare(int x, int y, int w, int h)
{
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;

    drawSquare(r);
}

void Pen::clear()
{
    SDL_SetRenderDrawColor(m_renderer, m_back_r, m_back_g, m_back_b, 255);
    SDL_RenderClear(m_renderer);
    SDL_SetRenderDrawColor(m_renderer, m_r, m_g, m_b, 255);
}

Pen::~Pen()
{
    //dtor
}
