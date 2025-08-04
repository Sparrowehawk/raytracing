#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rtweekend.h"


class sphere : public hittable {
    public:
        sphere(const point3& centre, double radius, shared_ptr<material> mat) : centre(centre), radius(std::fmax(0,radius)), mat(mat) {
            //TODO: init the material pointer mat
        }
        
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 oc = centre - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius * radius;
            auto discriminant = h*h - a*c;

            if (discriminant < 0){
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
            rec.p= r.at(rec.t);
            vec3 outward_norm = (rec.p - centre) / radius;
            rec.set_face_normal(r, outward_norm);
            rec.mat = mat;

            return true;
        }
    private:
        point3 centre;
        double radius;
        shared_ptr<material> mat;
};

#endif