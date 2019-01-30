#ifndef PEN_H
#define PEN_H

#include <SDL2/SDL.h>
class Pen
{
    public:
        Pen(SDL_Renderer *renderer);
        virtual ~Pen();

        void setPos(int x, int y);
        void setColor(int r, int g, int b);
        void setBackgroundColor(int r, int g, int b);

        void drawSquare(SDL_Rect &rect);
        void drawSquare(int x, int y, int w, int h);

        void clear();

    protected:

    private:
        int m_x, m_y;
        int m_r, m_g, m_b;

        int m_back_r = 0, m_back_g = 0, m_back_b = 0;

        SDL_Renderer *m_renderer;
};

#endif // PEN_H
