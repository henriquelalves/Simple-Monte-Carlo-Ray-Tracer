#ifndef GEOMETRIES_H
#define GEOMETRIES_H

#include "algebra.h"
#include <math.h>
#include <cstddef>
#include <stdio.h>
#include "ray.h"

using namespace std;

// Material struct, used in geometries
struct Material {
    Vector3D ambient;
    float kDiffuse;
    float kSpecular;

    float exp;

};


// Super class for all convex geometries
class Geometry
{
public:

    Geometry(){
        Material m;
        m.ambient = Vector3D(100.0f,200.0f,100.0f);
        m.kDiffuse = 0.5f;
        m.kSpecular = 0.3f;
        m.exp = 30.0f;

        mat = m;
    }

    virtual int intersect_ray(Ray intersect, Ray& reflected, Vector3D& pointA, Vector3D& pointB){printf("Warning: Using Geometry INTERSECT_RAY\n");} // Max of two intersected points in convex geometry
    virtual Vector3D get_normal_from_point(Vector3D p){printf("Warning: Using Geometry GET_NORMAL_FROM_POINT\n");}

    Material get_material(){
        return mat;
    }

    void set_material(Material m){
        mat = m;
    }

protected:

    Material mat;

};

// Geometries inherited from Geometry class

// Sphere
class Sphere : public Geometry
{
public:
    Sphere() : Geometry(){
        center_point = Vector3D(0.0f, 0.0f, 5.0f);
        radius = 2.0f;
    }

    Sphere(Vector3D c, double r) : Geometry() {

        center_point = c;
        radius = r;

    }

    int intersect_ray(Ray intersect, Ray &reflected, Vector3D &pointA, Vector3D &pointB){
        // Find quadratic component
        double v = intersect.getDirection().dot(intersect.getOrigin() - center_point);

        double q1 = pow((v),2);
        double q2 = -pow((intersect.getOrigin() - center_point).length(),2) + pow(radius,2);

        double q = q1 + q2;

        // Three cases of intersection
        if(q < 0.0f){ // No intersection
            return 0;
        } else if(q == 0.0f){ // Tangent
            double d = -v;

            if(d < 0){
                return 0;
            }

            pointA = intersect.getOrigin() + d*intersect.getDirection();
            return 1;
        } else { // Intersects
            double d = -v;
            double d1 = d - sqrt(q);
            double d2 = d + sqrt(q);

            if(d1 < 0 && d2 < 0){
                return 0;
            }
            else if (d1 < 0){
                pointA = intersect.getOrigin() + d2*intersect.getDirection();
                return 1;
            }
            else if (d2 < 0){
                pointA = intersect.getOrigin() + d1*intersect.getDirection();
                return 1;
            }
            else {
                pointA = intersect.getOrigin() + d1*intersect.getDirection();
                pointB = intersect.getOrigin() + d2*intersect.getDirection();

                return 2;
            }
        }
    }

    Vector3D get_normal_from_point(Vector3D p){
        Vector3D n = p-center_point;
        n.normalize();
        return n;
    }

private:

    Vector3D center_point;
    double radius;
};

// Box
class Box : public Geometry
{
public:
    Box();
};

// Cilinder
class Cilinder : public Geometry
{
public:
    Cilinder();
};

// Triangle Face
class TriFace : public Geometry
{
public:
    TriFace(){
        p1 = Vector3D(1.0f,0.0f,0.0f);
        p2 = Vector3D(0.0f,1.0f,0.0f);
        p3 = Vector3D(0.0f,0.0f,1.0f);
    }
    TriFace(Vector3D t1,Vector3D t2, Vector3D t3){
        p1 = t1;
        p2 = t2;
        p3 = t3;
    }

    void setPoints(Vector3D t1,Vector3D t2, Vector3D t3){
        p1 = t1;
        p2 = t2;
        p3 = t3;
    }

    int intersect_ray(Ray intersect, Ray &reflected, Vector3D &pointA, Vector3D &pointB){
        // Moller-Trumbore
        Vector3D e1, e2;
        Vector3D P, Q, T;
        double det, inv_det, u, v;
        double t;

        // Vectors for edges
        e1 = p2 - p1;
        e2 = p3 - p1;

        // Determinant
        P = intersect.getDirection().cross(e2);

        // Near 0
        det = e1.dot(P);

        // NOT CULLING
        if(det == 0) return 0;
        inv_det = 1.f / det;

        // distance v1 to origin
        T = intersect.getOrigin() - p1;

        u = (T.dot(P) * inv_det);

        if(u < 0.f || u > 1.f) return 0;

        // Prepare to test v parameter
        Q = T.cross(e1);

        // Calculate V parameter and test bound
        v = inv_det * intersect.getDirection().dot(Q);

        // Intersection lies outside triangle
        if(v < 0.f || u + v > 1.f) return 0;

        t = e2.dot(Q) * inv_det;

        if(t > 0){
            pointA = (intersect.getOrigin() + t * intersect.getDirection());
            return 1;
        }

        return 0;
    }

    Vector3D get_normal_from_point(Vector3D p) {
        Vector3D V = p2 - p1;
        Vector3D W = p3 - p1;

        Vector3D n;
        n[0] = (V[1]*W[2])-(V[2]*W[1]);
        n[1] = (V[2]*W[0])-(V[0]*W[2]);
        n[2] = (V[0]*W[1])-(V[1]*W[0]);
        n.normalize();

        return n;
    }

private:

    Vector3D p1,p2,p3;

};

// Quad Face
class QuadFace : public Geometry
{
public:
    QuadFace(){
        initPoints(Vector3D(2.f,0.f,-2.f),Vector3D(-2.f,0.f,-2.f),Vector3D(-2.f,0.f,2.f),Vector3D(2.f,0.f,2.f));
    }

    QuadFace(Vector3D p1, Vector3D p2, Vector3D p3, Vector3D p4){
        initPoints(p1,p2,p3,p4);
    }

    int intersect_ray(Ray intersect, Ray &reflected, Vector3D &pointA, Vector3D &pointB){
        int t = t1.intersect_ray(intersect,reflected,pointA,pointB);
        if (t == 0)
            t = t2.intersect_ray(intersect,reflected,pointA,pointB);
        return t;
    }

    Vector3D get_normal_from_point(Vector3D p){
        return t1.get_normal_from_point(p);
    }

private:
    void initPoints(Vector3D p1, Vector3D p2, Vector3D p3, Vector3D p4){
        t1 = TriFace(p1,p2,p3);
        t2 = TriFace(p3,p4,p1);
    }

    TriFace t1, t2;
};

#endif // GEOMETRIES_H
