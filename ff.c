/**
 * ff.c - Device/Partition data destruction ("zerofill" with 0xff)
 *
 * Copyright (c) 2020 Flavio Augusto (@facmachado)
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 *
 * Based on SeekTest (check seektest.c)
 *
 * Usage: ff <-n|-r|-s> <device|partition>
 */


#define _LARGEFILE64_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
unsigned char ff = 0xff;
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
  printf("\n\x1b[1;31mStopping, please wait... ");
  retval = close(fd);
  handle("close", retval < 0);
  time(&end);
  int interval = (int) (end - start);
  printf(
    "\x1b[1;32mDone!\x1b[0m\n"
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
void write_block(void) {
  retval = lseek64(fd, BLOCKSIZE * offset, SEEK_SET);
  handle("lseek64", retval == (off64_t) -1);
  retval = write(fd, memset(buffer, ff, BLOCKSIZE), BLOCKSIZE);
  handle("write", retval < 0);
}


/**
 * Normal operation
 */
void normal_w(char *device) {
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
    write_block();
  }
  finish();
}


/**
 * Reverse operation
 */
void reverse_w(char *device) {
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
    write_block();
    count++;
  }
  finish();
}


/**
 * Shuffle operation
 */
void shuffle_w(char *device) {
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
    write_block();
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

  fd = open(argv[2], O_WRONLY);
  handle("open", fd < 0);

  retval = ioctl(fd, BLKGETSIZE, &blocks);
  handle("ioctl", retval == -1);

  time(&start);
  signal(SIGINT, &finish);

  switch (argv[1][1]) {
    case 'n':
      normal_w(argv[2]);
      break;
    case 'r':
      reverse_w(argv[2]);
      break;
    case 's':
      shuffle_w(argv[2]);
      break;
    default:
      help(argv[0]);
  }
}
