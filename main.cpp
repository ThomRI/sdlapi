#include <iostream>
#include "Application.h"
#include "utilities.hpp"

#include <time.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

vec2 ux(1, 0);
vec2 uy(0, 1);

/* N particles of mass m */
#define WORLD 480 // The world SIZE (grid is 0 to world-1)
#define N 1000
 // fluid particles
#define MASS 10
#define DT 1

#define SIZE 2 // fluid particle size
#define FACTOR 5 // Speed unit
#define MIN_REACTION_TIME 5
#define MAX_REACTION_TIME 30 // Maximum amount of frames before speeds are recalculated

void simulation_step(Particle *fluid_particles, int dt, int frame)
{
    /* Randomly update next fluid particle acc */
    for(int i = 0;i < N;i++) {
        Particle *p = &fluid_particles[i];

        if(frame % p->reactionTime == 0) { // Can't modulo by 0
            int step_x = rand() % 2, // random integer between 0 and 1
                step_y = rand() % 2;


            double  factor_x = rand() % (FACTOR * 20) / 20.0,
                    factor_y = rand() % (FACTOR * 20) / 20.0;

            p->speed = vec2(factor_x * pow(-1, step_x), factor_y * pow(-1, step_y));
        }

        //p->speed = integrate(p->speed, p->acc, dt);
        p->pos = integrate(p->pos, p->speed, dt);

        //cout << p->acc << " " << p->speed << " " << p->pos << endl << endl;

        if(p->pos.x < 0)           p->pos.x = 0;
        else if(p->pos.x >= WORLD) p->pos.x = WORLD - 1;
        if(p->pos.y < 0)           p->pos.y = 0;
        else if(p->pos.y >= WORLD) p->pos.y = WORLD - 1;
    }
}

int main(int argc, char *argv[])
{
    cout << "Hello world!" << endl;

    /* Graphics */
    Application *app = new Application(WORLD, WORLD, "Brownian motion", 60); // 60 fps
    app->init();
    app->pen()->setColor(255, 255, 255);
    app->pen()->setBackgroundColor(0, 0, 0);


    Particle fluid_particles[N];

    /* Random start positions for the fluid particles */
    srand(time(0));
    for(int i = 0;i < N;i++) {
        int x = rand() % WORLD;
        int y = rand() % WORLD;
        int reaction_time = rand() % (MAX_REACTION_TIME - MIN_REACTION_TIME) + MIN_REACTION_TIME + 1;
        fluid_particles[i].pos = vec2(x, y);
        fluid_particles[i].reactionTime = reaction_time;

        app->pen()->drawSquare(x, y, SIZE, SIZE);
    }

    /*Particle *p = &fluid_particles[0];
    p->speed = vec2(10, 10);
    cout << p->pos << " " << p->speed << endl;
    p->pos = integrate(p->pos, p->speed, 1);
    cout << p->pos << " " << p->speed << endl;
    return 0;*/

    for(int n = 0;n < 10000;n++) {
        simulation_step(fluid_particles, DT, n);

        app->pen()->clear();
        for(int i = 0;i < N;i++) {
            Particle *p = &fluid_particles[i];
            app->pen()->drawSquare(p->pos.x, p->pos.y, SIZE, SIZE);
        }

        if(!app->frame()) {
            return 0;
        }
    }

    return 0;
}
