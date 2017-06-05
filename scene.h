#ifndef SCENE
#define SCENE

#include <vector>
#include <stdio.h>
#include <cstddef>
#include <memory>

#include "geometries.h"
#include "ray.h"
#include "camera.h"
#include "light.h"
#include "bitmap/bitmap_image.hpp"

using namespace std;

class Scene{
public:
    static const int RENDERER_NO_CAMERA_ID = -1;
    static const int RENDERER_OK = 0;

    static const float PHONG_AMBIENT_LIGHT = 0.8f;

    Scene(int w, int h){
        Iw = w;
        Ih = h;

        cameras.push_back(Camera(Iw, Ih));

        geometries.push_back(new Sphere(Vector3D(-1.0f,2.f, 8.0f), 2.0f));
        geometries.push_back(new Sphere(Vector3D(4.0f,-6.0f,16.0f),3.0f));
        geometries.push_back(new TriFace(Vector3D(-5.f,-2.f,7.f), Vector3D(-5.f,2.f,6.f), Vector3D(1.f,-1.f,6.f)));
        geometries.push_back(new QuadFace(Vector3D(-40.f,-10.f,0.f),Vector3D(-40.f,-10.f,80.f),Vector3D(40.f,-10.f,80.f),Vector3D(40.f,-10.f,0.f)));

        Material m = geometries[1]->get_material();
        m.ambient = Vector3D(200.0f,100.0f,50.0f);
        geometries[1]->set_material(m);

        m = geometries[2]->get_material();
        m.ambient = Vector3D(50.0f,100.0f,200.0f);
        geometries[2]->set_material(m);

        m = geometries[3]->get_material();
        m.ambient = Vector3D(200.0f,140.0f,200.0f);
        geometries[3]->set_material(m);

        lights.push_back(Light());
        lights.push_back(Light(Vector3D(4.f,0.f,1.f),Vector3D(150.f,150.f,150.f),1.f));


    }

    int render_scene(bitmap_image* image, int camera_id, int drunk_mode){
        // Safe-check
        if(camera_id < 0 || camera_id >= cameras.size()){
            return RENDERER_NO_CAMERA_ID;
        }

        Camera c = cameras[camera_id];

        // Find dx and dy (distance between pixels on camera lens)
        float dx = (c.get_lens_xmax() - c.get_lens_xmin()) / Iw;
        float dy = (c.get_lens_ymax() - c.get_lens_ymin()) / Ih;

        // For each pixel!
        for(int i = 0; i < Ih; i++){
            for (int j = 0; j < Iw; j++){
                // Find the ray from the camera origin
                float delta = 0.003f;
                if(drunk_mode == 1)
                    delta = 0.1f;

                Vector3D lens_point1 = Vector3D(c.get_lens_xmin() + j*dx,
                                               c.get_lens_ymax() - i*dy,
                                               0);
                Vector3D lens_point2 = Vector3D(c.get_lens_xmin() + j*dx + delta,
                                               c.get_lens_ymax() - i*dy,
                                               0);
                Vector3D lens_point3 = Vector3D(c.get_lens_xmin() + j*dx,
                                               c.get_lens_ymax() - i*dy + delta,
                                               0);
                Vector3D lens_point4 = Vector3D(c.get_lens_xmin() + j*dx + delta,
                                               c.get_lens_ymax() - i*dy + delta,
                                               0);

                Ray lens_ray1 = Ray(c.get_origin(), lens_point1 - c.get_origin());
                Ray lens_ray2 = Ray(c.get_origin(), lens_point2 - c.get_origin());
                Ray lens_ray3 = Ray(c.get_origin(), lens_point3 - c.get_origin());
                Ray lens_ray4 = Ray(c.get_origin(), lens_point4 - c.get_origin());

                // Trace ray
                Vector3D c = Vector3D();
                Vector3D temp;
                trace_ray(lens_ray1, temp, 0);
                c = c + temp;
                trace_ray(lens_ray2, temp, 0);
                c = c + temp;
                trace_ray(lens_ray3, temp, 0);
                c = c + temp;
                trace_ray(lens_ray4, temp, 0);
                c = c + temp;

                c = 0.25 * c;

                // Set pixel on image
		image->set_pixel(j,i,(char) c[0], (char) c[1], (char) c[2]);
            }
        }
    }

private:
    // Trace ray method!
    bool trace_ray(Ray r, Vector3D& c, int depth){
        // ------------------------------------
        // Find CLOSEST intersection:
        Vector3D p;
        int geo_id;
        bool intersected = closest_intersection(r,p,geo_id);

        // ------------------------------------
        // PHONG shading:

        if(intersected){
            Geometry* g = geometries[geo_id];
            Vector3D normal = g->get_normal_from_point(p);
            p = p + (0.1 * normal);

            c = phong(p, normal, g->get_material());

            // -------------------------------------
            // Specular Mirror stuff yaay
            if(depth < Ray::MAX_RAY_DEPTH){
                Vector3D newc;
                Ray ref = Ray(p, reflect_ray(r.getDirection(),normal));
                bool t = trace_ray(ref,newc,depth+1);
                if(t == true){
                    newc = g->get_material().kSpecular * PHONG_AMBIENT_LIGHT * 1/(depth+2) * newc ;

                    c = c + newc;
                }

                c.color_clamp();
            }
        } else {
            // Nice, smooth background color
            c = Vector3D(200.0f,200.0f,255.0f);
            return false;
        }

        // ------------------------------------

        return true;
    }

    // -----------------------------------------
    // Phong shading
    Vector3D phong(Vector3D p, Vector3D n, Material c){
        // Initial color with ambient light
        Vector3D cVec = Vector3D(c.ambient[0] * PHONG_AMBIENT_LIGHT,
                                 c.ambient[1] * PHONG_AMBIENT_LIGHT,
                                 c.ambient[2] * PHONG_AMBIENT_LIGHT);

        // Shadow ray for each light
        for(int i = 0; i < lights.size(); i++){
            Light l = lights[i];

            Vector3D v = (l.get_position() - p);
            Ray shadowRay = Ray(p, v);

            Vector3D np;
            int geo_id;

            // Find closest intersection point
            bool intersected = closest_intersection(shadowRay,np,geo_id);

            // If no intersection or the intersection happens after the light
            if(intersected == false || (l.get_position()-p).length() < (np-p).length()){
                // Diffuse
                Vector3D dotProduct = n;
                float d = dotProduct.dot(shadowRay.getDirection());
                //if(d > 0)
                cVec = cVec + c.kDiffuse * (max(d,0.0f)) * (l.get_color());

                // Specular
                Vector3D R = (2 * (n.dot(shadowRay.getDirection()) * n) - shadowRay.getDirection());
                Vector3D V = cameras[0].get_origin() - p;
                R.normalize();
                V.normalize();
                cVec = cVec + (c.kSpecular * pow(max(R.dot(V), (double)0.0f), c.exp) * l.get_color());

            }


        }
        // Clamping
        cVec.color_clamp();
        return cVec;
    }
    // -----------------------------------------
    // Closest intersection helper function
    bool closest_intersection(Ray r, Vector3D& p, int& geo_id){
        Vector3D p1 = r.get_point_in_ray(Ray::MAX_RAY_DISTANCE);
        Vector3D temp;
        Vector3D p2;
        Ray refl;

        bool intersected = false;
        for (int i = 0; i < geometries.size(); i++){
            Geometry* g = geometries[i];

            int n = g->intersect_ray(r, refl, temp, p2);

            if (n == 2 && (temp-r.getOrigin()).length() > (p2-r.getOrigin()).length()){
                temp = p2;
            }

            if(n > 0 && ((temp-r.getOrigin()).length() < (p1-r.getOrigin()).length())){
                p1 = temp;
                intersected = true;
                geo_id = i;
            }
        }

        p = p1;
        return intersected;
    }

    // ===================================================
    // Member variables
    vector<Geometry*> geometries;
    vector<Light> lights;
    vector<Camera> cameras;

    int Iw, Ih;


};

#endif // SCENE

