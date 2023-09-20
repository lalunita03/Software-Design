#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "forces.h"
#include "test_util.h"

list_t *make_shape() {
  list_t *shape = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){-1, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, +1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, +1};
  list_add(shape, v);
  return shape;
}

// mass doesn't exceed the amplitutde
void test_amplitude() {
  const double MASS = 4.5;
  const double K = 2.0;
  const double AMP = 1.0;
  const double DT = 1e-6;
  const int STEPS = 1000000;

  scene_t *scene = scene_init();
  body_t *mass = body_init(make_shape(), MASS, (rgb_color_t){0, 0, 0});
  body_t *anchor = body_init(make_shape(), INFINITY, (rgb_color_t){0, 0, 0});

  create_spring(scene, K, mass, anchor);
  body_set_centroid(mass, (vector_t){0, AMP});

  scene_add_body(scene, mass);
  scene_add_body(scene, anchor);

  for (size_t i = 0; i < STEPS; i++) {
    assert(body_get_centroid(mass).y <= AMP);
    scene_tick(scene, DT);
  }
  scene_free(scene);
}

double spring_potential(double k, body_t *body1, body_t *body2) {
  vector_t r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
  return 0.5 * k * vec_dot(r, r);
}

double kinetic_energy(body_t *body) {
  vector_t v = body_get_velocity(body);
  return body_get_mass(body) * vec_dot(v, v) / 2;
}

void test_spring() {
  const double MASS = 4.5;
  const double K = 0.25;
  const double DT = 1e-6;
  const int STEPS = 1000000;

  scene_t *scene = scene_init();
  body_t *mass1 = body_init(make_shape(), MASS, (rgb_color_t){0, 0, 0});
  body_t *mass2 = body_init(make_shape(), MASS, (rgb_color_t){0, 0, 0});

  body_set_centroid(mass1, (vector_t){100, 100});
  body_set_centroid(mass2, (vector_t){50, 50});

  scene_add_body(scene, mass1);
  scene_add_body(scene, mass2);

  create_spring(scene, K, mass1, mass2);
  double initial_energy = spring_potential(K, mass1, mass2);

  for (size_t i = 0; i < STEPS; i++) {
    double energy = spring_potential(K, mass1, mass2) + kinetic_energy(mass1) +
                    kinetic_energy(mass2);

    assert(within(1e-4, energy / initial_energy, 1));
    scene_tick(scene, DT);
  }

  scene_free(scene);
}

// velocity slows down due to drag
// v = v0 * e^(t / (m*b))
void test_drag_force() {
  const double MASS = 4.5;
  const double GAMMA = 0.25;
  const double DT = 1e-6;
  const int STEPS = 1000000;

  vector_t velocity = {10, 0};
  scene_t *scene = scene_init();
  body_t *mass = body_init(make_shape(), MASS, (rgb_color_t){0, 0, 0});

  body_set_velocity(mass, velocity);
  scene_add_body(scene, mass);
  create_drag(scene, GAMMA, mass);

  for (size_t i = 0; i < STEPS; i++) {
    assert(within(1e-4, body_get_velocity(mass).x,
                  10 * exp(-GAMMA * i * DT / MASS)));
    scene_tick(scene, DT);
  }
  scene_free(scene);
}

int main(int argc, char *argv[]) {
  // Run all tests if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_amplitude)
  DO_TEST(test_spring)
  DO_TEST(test_drag_force)

  puts("student_forces_test PASS");
}
