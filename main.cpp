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
#define MIN_REACTION_TIME 10
#define MAX_REACTION_TIME 50 // Maximum amount of frames before speeds are recalculated

#define SIMULATION_STEPS 5000
#define FPS 40 // 40 fps turn out to be 60 fps with fraps..

void simulation_step(Particle *fluid_particles, int dt, int frame)
{
    /* Randomly update next fluid particle acc */
    for(int i = 0;i < N;i++) {
        Particle *p = &fluid_particles[i];

        if(frame % p->reactionTime == 0) { // Can't modulo by 0
            int step_x = rand() % 2, // random integer between 0 and 1
                step_y = rand() % 2;


            double  factor_x = rand() % (FACTOR * 100) / 100.0, // To get a random double from 0 to FACTOR - 1 instead of an int
                    factor_y = rand() % (FACTOR * 100) / 100.0;

            p->speed = vec2(factor_x * pow(-1, step_x), factor_y * pow(-1, step_y));
        }

        //p->speed = integrate(p->speed, p->acc, dt);
        p->pos = integrate(p->pos, p->speed, dt);

        //cout << p->acc << " " << p->speed << " " << p->pos << endl << endl;

        if(p->pos.x < 0) {
            p->pos.x = 0;
            p->speed.x = -1 * p->speed.x; // bounces off the border
        }
        else if(p->pos.x >= WORLD) {
            p->pos.x = WORLD - 1;
            p->speed.x = -1 * p->speed.x;
        }
        if(p->pos.y < 0) {
            p->pos.y = 0;
            p->speed.y = -1 * p->speed.y;
        }
        else if(p->pos.y >= WORLD) {
            p->pos.y = WORLD - 1;
            p->speed.y = -1 * p->speed.y;
        }
    }
}

int main(int argc, char *argv[])
{
    cout << "Hello world!" << endl;

    /* Graphics */
    Application *app = new Application(WORLD, WORLD, "Brownian motion", FPS);
    app->init();
    app->pen()->setColor(255, 255, 255);
    app->pen()->setBackgroundColor(70, 80, 80);


    Particle fluid_particles[N];

    /* Random start position for the fluid particles */
    /* AND random reaction time */
    srand(time(0));
    for(int i = 0;i < N;i++) {
        int x = rand() % WORLD;
        int y = rand() % WORLD;
        int reaction_time = rand() % (MAX_REACTION_TIME - MIN_REACTION_TIME) + MIN_REACTION_TIME + 1;
        fluid_particles[i].pos = vec2(x, y);
        fluid_particles[i].reactionTime = reaction_time;

        app->pen()->drawSquare(x, y, SIZE, SIZE);
    }

    cout << "Starting simulation of " << SIMULATION_STEPS << " steps for " << N << " fluid particules." << endl;

    Particle *simulation[SIMULATION_STEPS]; // Simulations
    for(int i = 0;i < SIMULATION_STEPS;i++) {
        Particle *particles_array;
        particles_array = (Particle*) malloc(N * sizeof(Particle));
        if(particles_array == NULL) {
            return 0;
        }
        simulation[i] = particles_array;
    }

    // Initial fluid state
    for(int i = 0;i < N;i++) {
        simulation[0][i] = fluid_particles[i];
    }

    for(int n = 1;n < SIMULATION_STEPS;n++) {
        if(n % 100 == 0) {
            cout << "Simulation step : " << n << endl;
        }

        simulation_step(fluid_particles, DT, n); // One simulation step

        for(int i = 0;i < N;i++) {
            simulation[n][i] = fluid_particles[i]; // Copying results into the correct simulation
        }
    }

    cout << "Simulation done." << endl << endl;
    cout << "Starting simulation preview at " << FPS << " fps." << endl;
    for(int n = 0;n < SIMULATION_STEPS;n++) {
        app->pen()->clear();
        for(int i = 0;i < N;i++) {
            Particle *p = &simulation[n][i];
            app->pen()->drawSquare(p->pos.x, p->pos.y, SIZE, SIZE);
        }

        if(!app->frame()) {
            return 0;
        }

        SDL_Delay(1); // For CPU and SDL events
        free(simulation[n]);
    }
    cout << "Done previewing." << endl;

    return 0;
}
