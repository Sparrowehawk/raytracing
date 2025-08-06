#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include "hittable.h"

// A helper function to test for an intersection with a single triangle.
// It's placed outside the class as a static utility since it doesn't depend on instance state.
static bool hit_one_triangle(const ray& r, interval ray_t, hit_record& rec, const point3& v0,
                             const point3& v1, const point3& v2, shared_ptr<material> mat) {
    const vec3 edge1 = v1 - v0;
    const vec3 edge2 = v2 - v0;
    const vec3 pvec = cross(r.direction(), edge2);
    const double det = dot(edge1, pvec);

    if (fabs(det) < 1e-8) {
        return false; // Ray is parallel to the triangle
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
    if (!ray_t.contains(t)) {
        return false;
    }

    // A valid hit was found, record the details.
    rec.t = t;
    rec.p = r.at(t);
    rec.mat = mat;
    vec3 outward_normal = unit_vector(cross(edge1, edge2));
    rec.set_face_normal(r, outward_normal);

    return true;
}

class tetrahedron : public hittable {
  public:
    // Constructor now takes four points to define the tetrahedron
    tetrahedron(const point3& p0, const point3& p1, const point3& p2, const point3& p3,
                shared_ptr<material> m)
        : v0(p0), v1(p1), v2(p2), v3(p3), mat_ptr(m) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;

        // The interval is shrunk each time a closer intersection is found.
        interval current_interval = ray_t;

        // Define the four faces with consistent counter-clockwise winding for outward normals.
        // 

        // Face 1: v0, v1, v2
        if (hit_one_triangle(r, current_interval, temp_rec, v0, v1, v2, mat_ptr)) {
            hit_anything = true;
            current_interval.max = temp_rec.t;
            rec = temp_rec;
        }

        // Face 2: v0, v2, v3
        if (hit_one_triangle(r, current_interval, temp_rec, v0, v2, v3, mat_ptr)) {
            hit_anything = true;
            current_interval.max = temp_rec.t;
            rec = temp_rec;
        }

        // Face 3: v0, v3, v1
        if (hit_one_triangle(r, current_interval, temp_rec, v0, v3, v1, mat_ptr)) {
            hit_anything = true;
            current_interval.max = temp_rec.t;
            rec = temp_rec;
        }

        // Face 4: v1, v3, v2
        if (hit_one_triangle(r, current_interval, temp_rec, v1, v3, v2, mat_ptr)) {
            hit_anything = true;
            rec = temp_rec;
        }

        return hit_anything;
    }

  private:
    // Stores the four vertices of the tetrahedron
    point3 v0, v1, v2, v3;
    shared_ptr<material> mat_ptr;
};

#endif