#include <hackrf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
size_t bytes_to_write = 1000000;
FILE* fptr;

int rx_callback(hackrf_transfer* transfer) {
  size_t byte_size;
  printf("Bytes Arrived: %d\n", transfer->buffer_length);
  printf("Valid Length: %d\n", transfer->valid_length);
  if (transfer->valid_length >= bytes_to_write) {
    byte_size = bytes_to_write;
    bytes_to_write = 0;
  } else {
    byte_size = transfer->valid_length;
    bytes_to_write -= transfer->valid_length;
  }
  fwrite(transfer->buffer, 1, byte_size, fptr);
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
  fptr = fopen("out.iq", "w");
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
  fclose(fptr);
}