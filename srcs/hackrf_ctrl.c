#include <hackrf.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "shared_memory.h"
char* shared_block;
sem_t* sem_cons;
sem_t* sem_prod;

int rx_callback(hackrf_transfer* transfer) {
  size_t byte_size;
  printf("Bytes Arrived: %d\n", transfer->buffer_length);
  printf("Valid Length: %d\n", transfer->valid_length);
  sem_wait(sem_cons);
  memcpy(shared_block, transfer->buffer, BLOCK_SIZE);
  sem_post(sem_prod);
  return 0;
}

int main(int argc, char** argv) {
  int result = HACKRF_SUCCESS;
  hackrf_device* device;
  uint64_t center_freq = 100000000;
  uint32_t sample_rate = 10000000;
  uint32_t baseband_filter = sample_rate * .75;
  uint32_t lna_gain = 32;
  uint32_t vga_gain = 40;
  uint32_t allowed_baseband_filter;
  shared_block = attach_memory_block(FILENAME, BLOCK_SIZE);

  sem_prod = sem_open(SEM_PRODUCER_NAME, 0);
  if (sem_prod == SEM_FAILED) {
    fprintf(stderr, "semaphore creation failed\n");
    return 0;
  }

  sem_cons = sem_open(SEM_CONSUMER_NAME, 0);
  if (sem_prod == SEM_FAILED) {
    fprintf(stderr, "semaphore creation failed\n");
    return 0;
  }

  if (shared_block == NULL) {
    fprintf(stderr, "shared_block creation failed:\n");
  }
  result = hackrf_init();
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_init failed: %s (%d)\n", hackrf_error_name(result),
            result);
    return EXIT_FAILURE;
  }
  hackrf_open(&device);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_open failed: %s (%d)\n", hackrf_error_name(result),
            result);
    return EXIT_FAILURE;
  }
  allowed_baseband_filter =
      hackrf_compute_baseband_filter_bw_round_down_lt(baseband_filter);
  result = hackrf_set_sample_rate(device, sample_rate);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_set_sample_rate failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  result =
      hackrf_set_baseband_filter_bandwidth(device, allowed_baseband_filter);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_set_baseband_filter_bandwidth failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  result = hackrf_set_antenna_enable(device, 0);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_set_antenna_enable failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  result = hackrf_set_freq(device, center_freq);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_set_freq failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  result = hackrf_set_amp_enable(device, 0);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_set_amp_enable failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  result = hackrf_set_lna_gain(device, lna_gain);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_set_lna_gain failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  result = hackrf_set_vga_gain(device, vga_gain);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_set_vga_gain failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  result = hackrf_start_rx(device, rx_callback, NULL);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_start_rx failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  printf("Hackrf Streaming: %d\n", hackrf_is_streaming(device));
  sleep(1);
  result = hackrf_stop_rx(device);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_stop_rx failed: %s (%d)\n",
            hackrf_error_name(result), result);
    return EXIT_FAILURE;
  }
  result = hackrf_close(device);
  if (result != HACKRF_SUCCESS) {
    fprintf(stderr, "hackrf_close failed: %s (%d)\n", hackrf_error_name(result),
            result);
    return EXIT_FAILURE;
  }
  hackrf_exit();
  detach_memory_block(FILENAME);
  destroy_memory_block(FILENAME);
  sem_close(sem_prod);
  sem_close(sem_cons);
}