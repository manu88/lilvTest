#include <lilv/lilv.h>
#include <stdio.h>

int main() {
  printf("Test Lilv\n");
  LilvWorld *world = lilv_world_new();
  lilv_world_load_all(world);
  return 0;
}