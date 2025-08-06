#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "ray.h"
#include "rtweekend.h"
#include "vec3.h"
#include <cmath>
#include <iostream>
#include "material.h"

#include <chrono>
#include <iostream>

class camera {
public:
  double aspect_ratio = 1.0;
  int image_width = 100;
  int samples_per_pixel = 10;
  int max_depth = 10;

  double vfov = 90;
  point3 lookfrom = point3(0,0,0);
  point3 lookat = point3(0,0,-1);
  vec3 vup = vec3(0,1,0);

  double defocus_angle = 0;
  double focus_dist = 10;

  void render(const hittable &world) {
    initialise();
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int i = 0; i < image_height; i++) {
      std::clog << "\rScanlines remaining: " << (image_height - i) << ' '
                << std::flush;
      for (int j = 0; j < image_width; j++) {
        colour pixel_colour(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++) {
          ray r = get_ray(j, i);
          pixel_colour += ray_colour(r, max_depth, world);
        }

        write_colour(std::cout, pixel_sample_scale * pixel_colour);
      }
    }
    std::clog << '\n' << std::flush;
  }

private:
  int image_height;
  point3 centre;
  double pixel_sample_scale;
  point3 pixel00_loc;
  vec3 pixel_delta_u;
  vec3 pixel_delta_v;
  vec3 u, v, w;
  vec3 defocus_disk_u;
  vec3 defocus_disk_v;


  void initialise() {
    image_height = int(image_width / aspect_ratio);
    // If lower than 1, set as 1
    image_height = (image_height < 1) ? 1 : image_height;

    pixel_sample_scale = 1.0 / samples_per_pixel;

    centre = lookfrom;

    auto theta = deg_to_rad(vfov);
    auto h = std::tan(theta / 2);

    auto viewport_height = 2.0 * h * focus_dist;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    // Calculate the u, v, w,unit basis vectors for the camera coordinate frame
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Caclulate the vectors across the horizontal and down the verticle
    // viewpoint
    // edges
    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;

    // Calculate the horizontal and verticle delta vectors from pixel to pixel
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate upper left pixel location
    auto viewpoint_upper_left =
        centre - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;

    pixel00_loc = viewpoint_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vector
    auto defocus_radius = focus_dist * std::tan(deg_to_rad(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  ray get_ray(int j, int i) const {
    // Construct a cmera ray originating from the defocus diskand directed at randomly
    // samped point around pixel location i, j

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((j + offset.x()) * pixel_delta_u) +
                        ((i + offset.y()) * pixel_delta_v);

    auto ray_orig = (defocus_angle <= 0) ? centre : defocus_disk_sample();
    auto ray_dir = pixel_sample - ray_orig;

    return ray(ray_orig, ray_dir);
  }

  vec3 sample_square() const {
    // returns the vector to a random opint in the [-.5,-.5]-[+.5,+.5] unit
    // square
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  point3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk
    auto p = random_in_unit_disk();
    return centre + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

  colour ray_colour(const ray &r, int depth, const hittable &world) const {
    // If exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0) {
      return colour(0, 0, 0);
    }

    hit_record rec;
    if (world.hit(r, interval(0.001, infinity), rec)) {
      //   vec3 dir = random_on_hemisphere(rec.normal);
      // or
      // vec3 dir = rec.normal + random_unit_vector();

      // return 0.9 * ray_colour(ray(rec.p, dir), depth - 1, world);
      ray scattered;
      colour attenuation;
      if (rec.mat->scatter(r, rec, attenuation,scattered)) {
        return attenuation * ray_colour(scattered, depth-1, world);
      }
      return colour(0,0,0);
    }

    vec3 unit_dir = unit_vector(r.direction());
    auto a = 0.5 * (unit_dir.y() + 1.0);
    return (1.0 - a) * colour(1.0, 1.0, 1.0) + a * colour(0.5, 0.7, 1.0);
  }
};

#endif