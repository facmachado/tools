/**
 * ff.c
 * 0xff - Device/Partition data destruction ("zerofill" with 0xff)
 *        Version 1.0
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
  retval = close(fd);
  handle("close", retval < 0);
  time(&end);
  int interval = (int) (end - start);
  if (count < blocks)
    printf("\n\x1b[1;31mWrite stopped!\x1b[0m But data may be already lost\n");
  else
    printf("\n\x1b[1;32mWrite finished!\x1b[0m\n");
  printf("Total elapsed: %d seconds, %li seeks/s, %.2f ms access time\n",
    interval, count / interval, 1000.0 * interval / count);
  exit(EXIT_SUCCESS);
}


/**
 * Help
 */
void help() {
  printf("\x1b[1;33mUsage: ff <-n|-r|-s> <device|partition>\x1b[0m\n");
  exit(EXIT_SUCCESS);
}


/**
 * Normal write
 */
void normal_write(char *device) {
  printf("Write %s (%lu blocks, %luMB) in NORMAL mode. Please wait...\n",
    device, blocks, blocks / 2048);
  while (count <= blocks) {
    offset = (off64_t) count;
    printf("Writing block %lu\r", offset);
    retval = lseek64(fd, BLOCKSIZE * offset, SEEK_SET);
    handle("lseek64", retval == (off64_t) -1);
    retval = write(fd, memset(buffer, ff, BLOCKSIZE), BLOCKSIZE);
    handle("write", retval < 0);
    count++;
  }
  finish();
}


/**
 * Reverse write
 */
void reverse_write(char *device) {
  unsigned long cdown = blocks;
  printf("Write %s (%lu blocks, %luMB) in REVERSE mode. Please wait...\n",
    device, blocks, blocks / 2048);
  while (cdown > 0) {
    offset = (off64_t) cdown;
    printf("Writing block %lu \r", offset);
    retval = lseek64(fd, BLOCKSIZE * offset, SEEK_SET);
    handle("lseek64", retval == (off64_t) -1);
    retval = write(fd, memset(buffer, ff, BLOCKSIZE), BLOCKSIZE);
    handle("write", retval < 0);
    cdown--;
    count++;
  }
  finish();
}


/**
 * Shuffle write
 */
void shuffle_write(char *device) {
  srand(start);
  printf("Write %s (%lu blocks, %luMB) in SHUFFLE mode. Please wait...\n",
    device, blocks, blocks / 2048);
  while (count <= blocks) {
    offset = (off64_t) rand() % blocks;
    printf("Writing block %lu                                     \r", offset);
    retval = lseek64(fd, BLOCKSIZE * offset, SEEK_SET);
    handle("lseek64", retval == (off64_t) -1);
    retval = write(fd, memset(buffer, ff, BLOCKSIZE), BLOCKSIZE);
    handle("write", retval < 0);
    count++;
  }
  finish();
}


/**
 * Program start
 */
int main(int argc, char **argv) {
  printf("\x1b[1;37m0xff v.1.0 - (c) 2020 Flavio Augusto (@facmachado)\x1b[0m\n");
  if (argc < 3) help();

  setvbuf(stdout, NULL, _IONBF, 0);

  fd = open(argv[2], O_RDWR);
  handle("open", fd < 0);

  retval = ioctl(fd, BLKGETSIZE, &blocks);
  handle("ioctl", retval == -1);

  time(&start);
  signal(SIGINT, &finish);

  switch (argv[1][1]) {
    case 'n':
      normal_write(argv[2]);
      break;
    case 'r':
      reverse_write(argv[2]);
      break;
    case 's':
      shuffle_write(argv[2]);
      break;
    default:
      help();
  }
}
