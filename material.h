#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "ray.h"
#include "vec3.h"
#include <cmath>

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
  metal(const colour &albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

  bool scatter(const ray &r_in, const hit_record &rec, colour &attenuation,
               ray &scattered) const override {
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
    scattered = ray(rec.p, reflected);
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

private:
  colour albedo;
  double fuzz;
};

class dielectric : public material {
  public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
      attenuation = colour(1.0, 1.0, 1.0);
      double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

      vec3 unit_dir = unit_vector(r_in.direction());
      vec3 refracted = refract(unit_dir, rec.normal, ri);

      double cos_theta = std::fmin(dot(-unit_dir, rec.normal), 1.0);
      double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
      vec3 dir;

      if(ri * sin_theta > 1.0) {
        // Must reflect
        dir = reflect(unit_dir, rec.normal);
      } else {
        // can reflect
        dir = refract(unit_dir, rec.normal, ri);
      }

      scattered = ray(rec.p, dir);
      return true;
    }

  private:
    // Refreactive index in a vaccume or air or the ratio of material's refreactive
    // index over thr refeactive index of enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
      // Using Schlick's approx

      auto r0 = (1-refraction_index) / (1 + refraction_index);
      r0 = r0 * r0;
      return r0 + (1-r0)*std::pow((1-cosine), 5);
    }
  };

#endif