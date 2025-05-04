#include <stdio.h>

#include "shared_memory.h"
int main(int argc, char** argv) {
  if (destroy_memory_block(FILENAME)) {
    printf("mem block destroyed\n");
  }
}
