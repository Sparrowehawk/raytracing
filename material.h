#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "vec3.h"

class material {
public:
  virtual ~material() = default;

  virtual bool scatter(const ray &r, const hit_record &rec, colour &attenuation,
                       ray &scattered) const {
    return false;
  }
};

class lambertian : public material {
public:
  lambertian(const colour &albedo) : albedo(albedo) {}

  bool scatter(const ray &r_in, const hit_record &rec, colour &attenuation,
               ray &scattered) const override {
    auto scatter_dir = rec.normal + random_unit_vector();

    if (scatter_dir.near_zero()) {
      scatter_dir = rec.normal;
    }

    scattered = ray(rec.p, scatter_dir);
    attenuation = albedo;
    return true;
  }

private:
  colour albedo;
};

class metal : public material {
public:
  metal(const colour &albedo) : albedo(albedo) {}

  bool scatter(const ray &r_in, const hit_record &rec, colour &attenuation,
               ray &scattered) const override {
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    scattered = ray(rec.p, reflected);
    attenuation = albedo;
    return true;
  }

private:
  colour albedo;
};


#endif