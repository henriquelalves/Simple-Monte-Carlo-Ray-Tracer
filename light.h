#ifndef LIGHT
#define LIGHT

#include "algebra.h"

class Light{
public:
    Light(){
        position = Vector3D(-4.0f,3.0f,1.0f);
        color = Vector3D(150.0f,150.0f,150.0f);
        intensity = 1.0f;
    }

    Light(Vector3D p, Vector3D c, float i){
        position = p;
        color = c;
        intensity = i;
    }

    Vector3D get_position(){
        return position;
    }

    Vector3D get_color(){
        return color;
    }

    float get_intensity(){
        return intensity;
    }


private:
    Vector3D position;
    Vector3D color;
    float intensity;
};

#endif // LIGHT

