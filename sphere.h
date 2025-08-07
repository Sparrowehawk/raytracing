#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rtweekend.h"

class sphere : public hittable {
public:
  // Stationary
  sphere(const point3 &static_centre, double radius, shared_ptr<material> mat)
      : centre(static_centre, vec3(0, 0, 0)), radius(std::fmax(0, radius)),
        mat(mat) {
            auto rvec = vec3(radius, radius, radius);
            bbox = aabb(static_centre - rvec, static_centre + rvec);
        }

  // Moving
  sphere(const point3 &centre, const point3 &centre2, double radius,
         shared_ptr<material> mat)
      : centre(centre, centre2 - centre), radius(std::fmax(0, radius)),
        mat(mat) {
            auto rvec = vec3(radius, radius, radius);
            // IF there is an error, check here
            aabb box1(this->centre.at(0) - rvec, this->centre.at(0) + rvec);
            aabb box2(this->centre.at(1) - rvec, this->centre.at(1) + rvec);
            bbox = aabb(box1, box2);
        }


    aabb bounding_box() const override {
        return bbox;
    }

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
    point3 curr_centre = centre.at(r.time());
    vec3 oc = curr_centre - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = h * h - a * c;

    if (discriminant < 0) {
      return false;
    }

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in acceptable range
    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
      root = (h + sqrtd) / a;
      if (!ray_t.surrounds(root)) {
        return false;
      }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_norm = (rec.p - curr_centre) / radius;
    rec.set_face_normal(r, outward_norm);
    rec.mat = mat;

    return true;
  }

private:
  ray centre;
  double radius;
  shared_ptr<material> mat;
  aabb bbox;
};

#endif