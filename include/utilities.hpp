#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <iostream>
#include <vector>

using namespace std;

struct vec2{
    double x, y;

    vec2(double x = 0, double y = 0) : x(x), y(y) {}

    double operator*(const vec2& a) const // dot product
    {
        return x * a.x + y * a.y;
    }


    vec2& operator*(const double& n) const // scalar product
    {
        return *(new vec2(x*n, y*n));
    }

    vec2& operator/(const double& n) const
    {
        return operator*(1.0/n);
    }

    vec2& operator*(const int& n) const
    {
        return *(new vec2(x*n, y*n));
    }

    vec2& operator+(const vec2& a) const
    {
        return *(new vec2(a.x + x, a.y + y));
    }

    void operator+=(const vec2& a)
    {
        x += a.x;
        y += a.y;
    }

    bool operator==(const vec2& a) const
    {
        return (a.x == x && a.y == y);
    }
};

inline ostream& operator<<(ostream &out, const vec2 &vec)
{
    out << "(" << vec.x << ";" << vec.y << ")";
    return out;
}

typedef struct Particle{
    vec2 pos = vec2(0, 0), speed = vec2(0, 0);//, acc = vec2(0, 0);
    double m = 0; // mass in arbitrary unit. 0 for an empty space.
    int reactionTime = 1;

    vector<Particle *> next_colliding_particles;
    bool collided = false; // true if collided last frame
} Particle;

inline vec2 integrate(vec2 a, vec2 da, const int dt)
{
    //cout << "Integrating " << a << " with speed " << da << " and timestep " << dt << endl;
    return a + da*dt;
}

inline double average(double *array, int length)
{
    double sum = 0.0;
    for(int i = 0;i < length;i++) {
        sum += array[i];
    }

    return sum / length;
}

inline vec2 average(vec2 *array, int length)
{
    if(length == 0) {
        return vec2(0.0, 0.0);
    }
    double av_x = 0.0, av_y = 0.0;
    for(int i = 0;i < length;i++) {
        av_x += array[i].x;
        av_y += array[i].y;
    }

    return vec2(av_x / length, av_y / length);
}

#endif // UTILITIES_HPP_INCLUDED
