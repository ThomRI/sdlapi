#include <iostream>
#include "Application.h"
#include "utilities.hpp"

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <windows.h>

using namespace std;

/* Use mode : Preview or simulation mode. Define PREVIEW_MODE for preview, nothing for simulation. */
//#define PREVIEW_MODE

/* N particles of mass m */
#define WORLD 480 // The world SIZE (grid is 0 to world-1)
#define N 800
#define C 1 // Number of colliding particles

// Particles
#define MASS_FLUID 1.0
#define SIZE_FLUID 2

#define MASS_COLLIDING 10.0
#define SIZE_COLLIDING 10

#define MASS_RATIO (MASS_COLLIDING / MASS_FLUID)

#define DT 1

#define SIZE 2 // fluid particle size
#define FACTOR 5 // Speed unit
#define MIN_REACTION_TIME 10
#define MAX_REACTION_TIME 50 // Maximum amount of frames before speeds are recalculated

#define SIMULATION_STEPS 1000
#define FPS 40 // 40 fps turn out to be 60 fps with fraps..

/* Macros */
#define IS_IN_SQUARE(x, y, targetx, targety, targetsize) ( (x-targetx) >= 0 && (x-targetx) <= targetsize ) && ( (y-targety >= 0 && y-targety <= targetsize) )
#define WHITE 255, 255, 255
#define RED 255, 0, 0
#define BLUE 0, 0, 255

inline void border_correction(Particle *p)
{
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

void simulation_step(Particle *fluid_particles, Particle *colliding_particles, int dt, int frame)
{
    /* Updating fluid particles */
    for(int i = 0;i < N;i++) {
        Particle *p = &fluid_particles[i];

        if(frame % p->reactionTime == 0) { // Can't modulo by 0
            int step_x = rand() % 2, // random integer between 0 and 1
                step_y = rand() % 2;

            double  factor_x = rand() % (FACTOR * 100) / 100.0, // To get a random double from 0 to FACTOR - 1 instead of an int
                    factor_y = rand() % (FACTOR * 100) / 100.0;

            p->speed = vec2(factor_x * pow(-1, step_x), factor_y * pow(-1, step_y));
        }
        p->pos = integrate(p->pos, p->speed, dt);

        // Border correction
        border_correction(p);

        // After position correction
        for(int j = 0; j < C;j++) {
            Particle *c = &colliding_particles[j];
            bool isinsq = IS_IN_SQUARE(p->pos.x, p->pos.y, c->pos.x, c->pos.y, SIZE_COLLIDING);
            if(isinsq && !p->collided) {
                c->next_colliding_particles.push_back(&fluid_particles[i]); // Adding [i]-th particle to the ones colliding with [j]
                p->collided = true;
            } else if(p->collided && !isinsq) {
                p->collided = false;
            }
        }
    }

    /* Updating colliding particles */
    for(int j = 0;j < C;j++) {
        Particle *c = &colliding_particles[j];
        /* We already detected which fluid particles are going to collide with c from here */

        /* Updating c speed if necessary */
        if(c->next_colliding_particles.size() > 0) {
            vec2 *speeds; // Speeds of the next colliding particles
            speeds = (vec2*) malloc(sizeof(vec2) * c->next_colliding_particles.size());
            for(int i = 0;i < c->next_colliding_particles.size();i++) {
                speeds[i] = c->next_colliding_particles.at(i)->speed;
            }

            // equivalent speed as if there was only one particle colliding with c
            vec2 equivalent_colliding_speed = average(speeds, c->next_colliding_particles.size());
            c->speed = (equivalent_colliding_speed*2.0 + c->speed * (MASS_RATIO - 1)) / (MASS_RATIO + 1);
        }
        //cout << c->speed << endl;

        /* Integrating position */
        c->pos = integrate(c->pos, c->speed, dt);

        /* Border correction */
        border_correction(c);

        /* Collision were calculated, we can clear the collision queue */
        c->next_colliding_particles.clear();
    }
}

int main(int argc, char *argv[])
{
    /* IF SIMULATION MODE
        Arg 0 is sdlapi.exe
        Arg 1 is srand seed
        next are start positions for colliding particles ("x1 y1 x2 y2...")
    */

    #ifdef PREVIEW_MODE
    cout << "Hello world!" << endl;

    /* Graphics */
    Application *app = new Application(WORLD, WORLD, "Brownian motion", FPS);
    app->init();
    app->pen()->setColor(WHITE);
    app->pen()->setBackgroundColor(70, 80, 80);
    #endif

    Particle fluid_particles[N];
    Particle col_particles[C];

    #ifdef PREVIEW_MODE
    srand(time(0));
    #else
    string seed(argv[1]);
    srand(stoi(seed));
    #endif // PREVIEW_MODE

    /* Initialising fluid particles */
    for(int i = 0;i < N;i++) {
        int x = rand() % WORLD;
        int y = rand() % WORLD;
        int reaction_time = rand() % (MAX_REACTION_TIME - MIN_REACTION_TIME) + MIN_REACTION_TIME + 1;
        fluid_particles[i].pos = vec2(x, y);
        fluid_particles[i].reactionTime = reaction_time;

        #ifdef PREVIEW_MODE
        app->pen()->drawSquare(x, y, SIZE_FLUID, SIZE_FLUID);
        #endif
    }

    /* Initialising colliding particles */
    if(argc == 1) { // No initial position provided
        for(int j = 0;j < C;j++) {
            int x = rand() % WORLD;
            int y = rand() % WORLD;
            col_particles[j].pos = vec2(x, y);

            #ifdef PREVIEW_MODE
            app->pen()->setColor(RED);
            app->pen()->drawSquare(x, y, SIZE_COLLIDING, SIZE_COLLIDING);

            cout << "Created colliding particle at " << col_particles[j].pos << endl;
            #endif
        }
    } else {
        if(argc - 2 != 2 * C) {
            return -1; // Must be provided an initial position for each colliding particle
        }

        for(int j = 0;j < C;j++) {
            string x_str(argv[2*j + 2]), y_str(argv[2*j + 3]);
            col_particles[j].pos = vec2(stod(x_str), stod(y_str));
        }
    }

    #ifdef PREVIEW_MODE
    cout << "Starting simulation of " << SIMULATION_STEPS << " steps for " << N << " fluid particles and " << C << " colliding particles." << endl;

    /* ONLY FOR PREVIEW */
    Particle *simulation_fluid[SIMULATION_STEPS]; // Simulations
    Particle *simulation_colliding[SIMULATION_STEPS];
    for(int i = 0;i < SIMULATION_STEPS;i++) {
        Particle *fluid_particles_array;
        Particle *colliding_particles_array;

        fluid_particles_array = (Particle*) malloc(N * sizeof(Particle));
        colliding_particles_array = (Particle*) malloc(C * sizeof(Particle));
        if(fluid_particles_array == NULL || colliding_particles_array == NULL) {
            return -1;
        }
        simulation_fluid[i] = fluid_particles_array;
        simulation_colliding[i] = colliding_particles_array;
    }

    // Initial fluid state
    for(int i = 0;i < N;i++) {
        simulation_fluid[0][i] = fluid_particles[i];
    }

    // Initial colliding particles state
    for(int j = 0;j < C;j++) {
        simulation_colliding[0][j] = col_particles[j];
    }

    /* Simulating */
    for(int n = 1;n < SIMULATION_STEPS;n++) {
        if(n % 100 == 0) {
            cout << "Simulation step : " << n << endl;
        }

        simulation_step(fluid_particles, col_particles, DT, n); // One simulation step

        for(int i = 0;i < N;i++) {
            simulation_fluid[n][i] = fluid_particles[i]; // Copying results into the correct simulation
        }

        for(int j = 0;j < C;j++) { // same for colliding particles
            simulation_colliding[n][j] = col_particles[j];
        }
    }


    vector<SDL_Point>* path_tracer_colliding[C]; // Stores pixels that have been visited by colliding particles at element [i] (array of (*vector))
    for(int z = 0;z < C;z++) {
        path_tracer_colliding[z] = new vector<SDL_Point>;
    }

    cout << "Simulation done." << endl << endl;
    cout << "Starting simulation preview at " << FPS << " fps." << endl;
    for(int n = 0;n < SIMULATION_STEPS;n++) {
        app->pen()->clear();

        // Drawing fluid particles
        app->pen()->setColor(WHITE);
        for(int i = 0;i < N;i++) {
            Particle *p = &simulation_fluid[n][i];
            app->pen()->drawSquare(p->pos.x, p->pos.y, SIZE_FLUID, SIZE_FLUID);
        }

        // Drawing colliding particles
        app->pen()->setColor(RED);
        for(int j = 0;j < C;j++) {
            Particle *c = &simulation_colliding[n][j];
            path_tracer_colliding[j]->push_back({c->pos.x + SIZE_COLLIDING / 2, c->pos.y + SIZE_COLLIDING / 2}); // Path centered
            app->pen()->drawSquare(c->pos.x, c->pos.y, SIZE_COLLIDING, SIZE_COLLIDING);
        }

        // Drawing colliding particles path
        app->pen()->setColor(BLUE);
        for(int z = 0;z < C;z++) {
            app->pen()->drawLines(*path_tracer_colliding[z]);
        }


        if(!app->frame()) {
            return 0;
        }

        SDL_Delay(1); // For CPU and SDL events
        //free(simulation[n]);
        if(n == SIMULATION_STEPS - 1) {
            cout << "Preview reset." << endl;
            for(int z = 0;z < C;z++) {
                path_tracer_colliding[z]->clear();
            }
            n = 0;
        }
    }
    cout << "Done previewing." << endl;

    #else
    /* Simulation mode */
    for(int n = 1;n < SIMULATION_STEPS;n++) {
        simulation_step(fluid_particles, col_particles, DT, n);
    }

    /* Writing final positions */
    for(int j = 0;j < C;j++) {
        cout << col_particles[j].pos.x << " " << col_particles[j].pos.y << endl;
    }
    #endif

    return 0;
}
