#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <iostream>
using namespace std;

typedef struct vec2{
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
} vec2;

ostream& operator<<(ostream &out, const vec2 &vec)
{
    out << "(" << vec.x << ";" << vec.y << ")";
    return out;
}

typedef struct {
    vec2 pos = vec2(0, 0), speed = vec2(0, 0);//, acc = vec2(0, 0);
    int m = 0; // mass in arbitrary unit. 0 for an empty space.
    int reactionTime = 1;
} Particle;

vec2 integrate(vec2 a, vec2 da, const int dt)
{
    //cout << "Integrating " << a << " with speed " << da << " and timestep " << dt << endl;
    return a + da*dt;
}

#endif // UTILITIES_HPP_INCLUDED
