#ifndef CAMERA_H
#define CAMERA_H

#include "algebra.h"

class Camera{
public:
    Camera(float w, float h){
        origin = Vector3D(0.0f,0.0f,-1.0f);
        up = Vector3D(0.0f,1.0f,0.0f);
        lookat = Vector3D(0.0f,0.0f,1.0f);

        lens_distance = 1.0f;

        float ratiow = w/h;
        float ratioh = 1.f;

        lens_xmin = -ratiow;
        lens_xmax = ratiow;
        lens_ymin = -ratioh;
        lens_ymax = ratioh;

    }

    float get_lens_xmin(){
        return lens_xmin;
    }
    float get_lens_xmax(){
        return lens_xmax;
    }
    float get_lens_ymin(){
        return lens_ymin;
    }
    float get_lens_ymax(){
        return lens_ymax;
    }

    Vector3D get_origin(){
        return origin;
    }

private:

    Vector3D origin;
    Vector3D up;
    Vector3D lookat;

    float lens_distance;

    float lens_xmin;
    float lens_xmax;
    float lens_ymin;
    float lens_ymax;

};

#endif // CAMERA_H
