/**
 * ffbyte.c - Same as ffblock.c, but byte-by-byte
 *
 * Copyright (c) 2020 Flavio Augusto (@facmachado)
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 *
 * Based on seektest.c
 *
 * Usage: ffbyte <-n|-r|-s> <device|partition>
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

#define BLOCKSIZE 1

int fd, retval;
char buffer[BLOCKSIZE];
unsigned char ff = 0xff;
unsigned long count, blocks, bytes;
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
 * Warning (users everywhere)
 */
void disclaimer(void) {
  char ans[4];
  printf(
    "\n"
    "############################################################\n"
    "#                                                          #\n"
    "#                     W A R N I N G !                      #\n"
    "#              PLEASE READ THIS BEFORE START               #\n"
    "#                                                          #\n"
    "#  It is important for you to know:                        #\n"
    "#                                                          #\n"
    "#  1. This app WILL DESTROY your data,                     #\n"
    "#     and YOU, AND ONLY YOU, are responsible for it.       #\n"
    "#                                                          #\n"
    "#  2. This operation can take MUCH TIME (hours or days),   #\n"
    "#     even on NORMAL mode (depends on the target).         #\n"
    "#                                                          #\n"
    "#  If you do not agree with the disclaimer above,          #\n"
    "#  just stop this app by pressing ENTER.                   #\n"
    "#                                                          #\n"
    "############################################################\n"
    "\nDo you want to continue (yes/NO)? "
  );
  fgets(ans, 4, stdin);
  if (strcmp(ans, "yes")) {
    printf("Cancelled\n");
    exit(EXIT_SUCCESS);
  }
}


/**
 * Program finish
 */
void finish(void) {
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
void write_byte(void) {
  retval = lseek64(fd, offset, SEEK_SET);
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
    "(%s, %lu bytes)...\n",
    device,
    bytes
  );
  for (count = 0; count < bytes; count++) {
    offset = (off64_t) count;
    printf("   > Byte ID: %lu\r", offset);
    write_byte();
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
    "(%s, %lu bytes)...\n",
    device,
    bytes
  );
  for (cdown = bytes; cdown > 0; cdown--) {
    offset = (off64_t) cdown - 1;
    printf("   > Byte ID: %lu \r", offset);
    write_byte();
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
    "(%s, %lu bytes)...\n",
    device,
    bytes
  );
  for (count = 0; count < bytes; count++) {
    offset = (off64_t) rand() % bytes;
    printf(
      "   > Byte ID: %lu"
      "                                \r",
      offset
    );
    write_byte();
  }
  finish();
}


/**
 * main()
 */
int main(int argc, char **argv) {
  printf(
    "\x1b[1;37m%s - (c) 2020 "
    "Flavio Augusto (@facmachado) [MIT License]\n"
    "https://github.com/facmachado/tools\x1b[0m\n",
    argv[0]
  );
  if (argc < 3) help(argv[0]);
  disclaimer();

  setvbuf(stdout, NULL, _IONBF, 0);

  fd = open(argv[2], O_WRONLY);
  handle("open", fd < 0);

  retval = ioctl(fd, BLKGETSIZE, &blocks);
  handle("ioctl", retval == -1);

  bytes = blocks * 512;

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
