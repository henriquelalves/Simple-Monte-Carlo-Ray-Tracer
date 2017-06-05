#ifndef RAY_H
#define RAY_H

#include "algebra.h"

class Ray
{
public:
    static const long int MAX_RAY_DISTANCE = 999999;
    static const int MAX_RAY_DEPTH = 8;

    // Constructors
    Ray(){
        origin = Vector3D(0.0f,0.0f,0.0f);
        direction = Vector3D(1.0f,0.0f,0.0f);
    }
    Ray(Vector3D o, Vector3D d){
        origin = o;
        direction = d;
        direction.normalize(); // Ensure that direction is normal
    }

    // Getters and Setters
    Vector3D getOrigin(){
        return origin;
    }
    void setOrigin(Vector3D o){
        origin = o;
    }
    Vector3D getDirection(){
        return direction;
    }
    void setDirection(Vector3D d){
        direction = d;
        direction.normalize();
    }

    Vector3D get_point_in_ray(double d){
        return origin + (d * direction);
    }

private: // Ray properties
    Vector3D origin;
    Vector3D direction;
};

Vector3D reflect_ray(Vector3D v, Vector3D n){
    return v -(2 * v.dot(n) * n);
}

#endif // RAY_H
