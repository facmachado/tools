/**
 * seektest.c - Device/Partition seek test (mini benchmark)
 *
 * Copyright (c) 2020 Flavio Augusto (@facmachado)
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 *
 * Based on the original "Seeker"
 * (http://www.linuxinsight.com/how_fast_is_your_disk.html)
 *
 * Usage: seektest <-n|-r|-s> <device|partition>
 */


#define _LARGEFILE64_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fs.h>

#define BLOCKSIZE 512

int fd, retval;
char buffer[BLOCKSIZE];
unsigned long count, blocks;
time_t start, end;
off64_t offset;


/**
 * Help
 */
void help(const char *arg0) {
  printf(
    "\x1b[1;33mUsage: "
    "%s <-n|-r|-s> "
    "<device|partition>\x1b[0m\n",
    arg0
  );
  exit(EXIT_SUCCESS);
}


/**
 * Error handling
 */
void handle(const char *string, int error) {
  if (error) {
    perror(string);
    exit(EXIT_FAILURE);
  }
}


/**
 * Program finish
 */
void finish() {
  time(&end);
  int interval = (int) (end - start);
  if (count < blocks)
    printf("\n\x1b[1;31mStopped!\x1b[0m\n");
  else
    printf("\n\x1b[1;32mDone!\x1b[0m\n");
  printf(
    "Total elapsed: "
    "%d seconds, "
    "%li seeks/s, "
    "%.2f ms access time\n",
    interval,
    count / interval,
    1000.0 * interval / count
  );
  exit(EXIT_SUCCESS);
}


/**
 * Common to all block functions
 */
void read_block(void) {
  retval = lseek64(fd, BLOCKSIZE * offset, SEEK_SET);
  handle("lseek64", retval == (off64_t) -1);
  retval = read(fd, buffer, BLOCKSIZE);
  handle("read", retval < 0);
}


/**
 * Normal operation
 */
void normal_s(char *device) {
  printf(
    "Running in NORMAL mode "
    "(%s, %lu blocks, %lu bytes)...\n",
    device,
    blocks,
    blocks * BLOCKSIZE
  );
  for (count = 0; count < blocks; count++) {
    offset = (off64_t) count;
    printf(
      "   > Block ID: %lu"
      "                                \r",
      offset
    );
    read_block();
  }
  finish();
}


/**
 * Reverse operation
 */
void reverse_s(char *device) {
  unsigned long cdown;
  count = 0;
  printf(
    "Running in REVERSE mode "
    "(%s, %lu blocks, %lu bytes)...\n",
    device,
    blocks,
    blocks * BLOCKSIZE
  );
  for (cdown = blocks; cdown > 0; cdown--) {
    offset = (off64_t) cdown - 1;
    printf(
      "   > Block ID: %lu"
      "                                \r",
      offset
    );
    read_block();
    count++;
  }
  finish();
}


/**
 * Shuffle operation
 */
void shuffle_s(char *device) {
  srand(start);
  printf(
    "Running in SHUFFLE mode "
    "(%s, %lu blocks, %lu bytes)...\n",
    device,
    blocks,
    blocks * BLOCKSIZE
  );
  for (count = 0; count < blocks; count++) {
    offset = (off64_t) rand() % blocks;
    printf(
      "   > Block ID: %lu"
      "                                \r",
      offset
    );
    read_block();
  }
  finish();
}


/**
 * main()
 */
int main(int argc, char **argv) {
  printf(
    "\x1b[1;37m(c) 2020 "
    "Flavio Augusto (@facmachado) [MIT License]\n"
    "https://github.com/facmachado/tools\x1b[0m\n"
  );
  if (argc < 3) help(argv[0]);

  setvbuf(stdout, NULL, _IONBF, 0);

  fd = open(argv[2], O_RDONLY);
  handle("open", fd < 0);

  retval = ioctl(fd, BLKGETSIZE, &blocks);
  handle("ioctl", retval == -1);

  time(&start);
  signal(SIGINT, &finish);

  switch (argv[1][1]) {
    case 'n':
      normal_s(argv[2]);
      break;
    case 'r':
      reverse_s(argv[2]);
      break;
    case 's':
      shuffle_s(argv[2]);
      break;
    default:
      help(argv[0]);
  }
}
