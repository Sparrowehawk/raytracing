#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "tetrahedron.h"
#include "triangle.h"
#include "vec3.h"
#include <memory>

int main() {
  hittable_list world;

  auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  // The loop for small random spheres and tetrahedrons
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<material> object_material;

        if (choose_mat < 0.70) {
          // diffuse sphere
          auto albedo = colour::random() * colour::random();
          object_material = make_shared<lambertian>(albedo);
          world.add(make_shared<sphere>(center, 0.2, object_material));
        } else if (choose_mat < 0.85) {
          // metal sphere
          auto albedo = colour::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          object_material = make_shared<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(center, 0.2, object_material));
        } else if (choose_mat < 0.95){
          // glass sphere
          object_material = make_shared<dielectric>(1.5);
          world.add(make_shared<sphere>(center, 0.2, object_material));
        } else {
          // diffuse tetrahedron  tetrahedron
          auto albedo = colour::random() * colour::random();
          object_material = make_shared<lambertian>(albedo);
          
          // Define tetrahedron vertices centered around the random point
          point3 p0 = center + point3(0,      0.2,    0);      // Apex
          point3 p1 = center + point3(-0.173, -0.1,    0.1);   // Base vertex 1
          point3 p2 = center + point3(0.173,  -0.1,    0.1);   // Base vertex 2
          point3 p3 = center + point3(0,      -0.1,   -0.2);   // Base vertex 3
          
          world.add(make_shared<tetrahedron>(p0, p1, p2, p3, object_material));
        }
      }
    }
  }

  // The three large spheres
  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
  camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 1920;
  cam.samples_per_pixel = 500;
  cam.max_depth = 50;

  cam.vfov = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat = point3(0, 0, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0.8;
  cam.focus_dist = 10.0; // better focus

  cam.render(world);
}
