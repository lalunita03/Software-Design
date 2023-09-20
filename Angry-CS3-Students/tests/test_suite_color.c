#include "color.h"
#include "math.h"
#include "test_util.h"
#include <assert.h>

void test_color() {
  // TODO:
}

int main(int argc, char *argv[]) {
  // Run all tests? True if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }
  DO_TEST(test_color);
}
