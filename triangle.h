#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "ray.h"
#include "vec3.h"
#include <cmath>

class triangle : public hittable {
public:
  triangle(point3 v0, point3 v1, point3 v2, shared_ptr<material> m)
      : v0(v0), v1(v1), v2(v2), mat_ptr(m) {}

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {

    const vec3 edge1 = v1 - v0;
    const vec3 edge2 = v2 - v0;
    const vec3 pvec = cross(r.direction(), edge2);
    const double det = dot(edge1, pvec);

    // If the ray is parallel to the triangle (culling)
    if (fabs(det) < 1e-8) {
        return false;
    }

    const double inv_det = 1.0 / det;
    const vec3 tvec = r.origin() - v0;
    const double u = dot(tvec, pvec) * inv_det;

    if (u < 0.0 || u > 1.0) {
        return false;
    }

    const vec3 qvec = cross(tvec, edge1);
    const double v = dot(r.direction(), qvec) * inv_det;

    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    const double t = dot(edge2, qvec) * inv_det;

    // Use the interval's `contains` method for the check
    if (!ray_t.contains(t)) {
        return false;
    }

    // A valid hit was found within the required interval.
    rec.t = t;
    rec.p = r.at(t);
    vec3 outward_normal = unit_vector(cross(edge1, edge2));
    rec.set_face_normal(r, outward_normal);
    rec.mat = mat_ptr;
    
    return true;
  };

public:
  point3 v0, v1, v2;
  shared_ptr<material> mat_ptr;
};

#endif