/**
 * seektest.c
 * SeekTest - Device/Partition seek test (mini benchmark)
 *            Version 1.3
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
 * Program finish
 */
void finish() {
  time(&end);
  int interval = (int) (end - start);
  if (count < blocks)
    printf("\n\x1b[1;31mTest canceled!\x1b[0m\n");
  else
    printf("\n\x1b[1;32mTest finished!\x1b[0m\n");
  printf("Total elapsed: %d seconds, %li seeks/s, %.2f ms access time\n",
    interval, count / interval, 1000.0 * interval / count);
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
 * Help
 */
void help() {
  printf("\x1b[1;33mUsage: devseek <-n|-r|-s> <device|partition>\x1b[0m\n");
  exit(EXIT_SUCCESS);
}


/**
 * Normal seek
 */
void normal_seek(char *device) {
  printf("Testing %s (%lu blocks, %luMB) in NORMAL mode. Please wait...\n",
    device, blocks, blocks / 2048);
  while (count <= blocks) {
    offset = (off64_t) count;
    printf("Checking block %lu\r", offset);
    retval = lseek64(fd, BLOCKSIZE *offset, SEEK_SET);
    handle("lseek64", retval == (off64_t) -1);
    retval = read(fd, buffer, BLOCKSIZE);
    handle("read", retval < 0);
    count++;
  }
  finish();
}


/**
 * Reverse seek
 */
void reverse_seek(char *device) {
  unsigned long cdown = blocks;
  printf("Testing %s (%lu blocks, %luMB) in REVERSE mode. Please wait...\n",
    device, blocks, blocks / 2048);
  while (cdown > 0) {
    offset = (off64_t) cdown;
    printf("Checking block %lu \r", offset);
    retval = lseek64(fd, BLOCKSIZE *offset, SEEK_SET);
    handle("lseek64", retval == (off64_t) -1);
    retval = read(fd, buffer, BLOCKSIZE);
    handle("read", retval < 0);
    cdown--;
    count++;
  }
  finish();
}


/**
 * Shuffle seek
 */
void shuffle_seek(char *device) {
  srand(start);
  printf("Testing %s (%lu blocks, %luMB) in SHUFFLE mode. Please wait...\n",
    device, blocks, blocks / 2048);
  while (count <= blocks) {
    offset = (off64_t) rand() % blocks;
    printf("Checking block %lu          \r", offset);
    retval = lseek64(fd, BLOCKSIZE *offset, SEEK_SET);
    handle("lseek64", retval == (off64_t) -1);
    retval = read(fd, buffer, BLOCKSIZE);
    handle("read", retval < 0);
    count++;
  }
  finish();
}


/**
 * Program start
 */
int main(int argc, char **argv) {
  printf("\x1b[1;37mDevSeek v.1.2 - (C) 2017 Flavio Augusto (@facmachado)\x1b[0m\n");
  if (argc < 3) help();

  setvbuf(stdout, NULL, _IONBF, 0);

  fd = open(argv[2], O_RDONLY);
  handle("open", fd < 0);

  retval = ioctl(fd, BLKGETSIZE, &blocks);
  handle("ioctl", retval == -1);

  time(&start);
  signal(SIGINT, &finish);

  switch (argv[1][1]) {
    case 'n':
      normal_seek(argv[2]);
      break;
    case 'r':
      reverse_seek(argv[2]);
      break;
    case 's':
      shuffle_seek(argv[2]);
      break;
    default:
      help();
  }
}
