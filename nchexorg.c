// WIP: Use "gcc -ltinfo -lncurses -o nchexorg nchexorg.c && strip -s nchexorg"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ncurses.h>


// #ifndef CTRL
// #define CTRL(c) ((c) & 0x1f)
// #endif


void beginSession(void);
void initColors(void);
void bottomAbout(void);
void bottomEdit(void);
void bottomError(char *msg);
void bottomFind(void);
void bottomQuit(void);
void resizeHandler(void);
void mainWindow(void);
void mainBottom(void);
void mainTop(void);
void endSession(void);


int main(int argc, char **argv) {
  char *key;
  beginSession();

  mainWindow();
  mainTop();
  mainBottom();

  while (key = getch()) {
    if (key == 'q')
      break;
    else if (key == 'a')
      bottomAbout();
    else if (key == 'b')
      bottomError("Voce apertou o botao errado");
    else if (key == 'e')
      bottomEdit();
    else if (key == 'f')
      bottomFind();
    else if (key == 'n')
      mainBottom();
    else if (key == 's')
      bottomQuit();
    else if (key == KEY_RESIZE)
      resizeHandler();
  }

  endSession();
}


void beginSession(void) {
  initscr();
  keypad(stdscr, TRUE);
  curs_set(0);
  noecho();
  initColors();
}


void initColors(void) {
  start_color();

  /**
   * Main window colors
   * 1: window/key, 2: ASCII view, 3: bottom bar
   */
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_BLACK, COLOR_GREEN);

  /**
   * Offset address mode colors
   * 4: list, 5: cursor
   */
  init_pair(4, COLOR_YELLOW, COLOR_BLUE);
  init_pair(5, COLOR_BLUE, COLOR_YELLOW);

  /**
   * Origin parameter mode colors
   * 6: list, 7: cursor
   */
  init_pair(6, COLOR_YELLOW, COLOR_RED);
  init_pair(7, COLOR_RED, COLOR_YELLOW);

  /**
   * Edit mode color
   * 8: field
   */
  init_pair(8, COLOR_BLACK, COLOR_YELLOW);

  /**
   * Dialog color
   * 9: question/error
   */
  init_pair(9, COLOR_WHITE, COLOR_RED);

  /**
   * Help color
   * 10: about
   */
  init_pair(10, COLOR_WHITE, COLOR_BLUE);
}


void bottomAbout(void) {
  move(LINES - 1, 0);
  attron(COLOR_PAIR(10));
  whline(stdscr, ' ', COLS);
  move(LINES - 1, 0);
  printw("HexOrg - (c) @facmachado https://github.com/facmachado/tools [MIT] ");
  attroff(COLOR_PAIR(10));
  attron(A_BOLD);
  printw("ENTER");
  attroff(A_BOLD);
  attron(COLOR_PAIR(10));
  printw(" OK");
  attroff(COLOR_PAIR(10));
}


void bottomEdit(void) {
  move(LINES - 1, 0);
  attron(COLOR_PAIR(3));
  whline(stdscr, ' ', COLS);
  move(LINES - 1, 0);
  printw("O/A: 00000070/00000070 Old code: 72 New code: ");
  attron(COLOR_PAIR(8));
  printw("__");
  attron(COLOR_PAIR(3));
  printw(" ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("ESC");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Cancel ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("ENTER");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Confirm");
  attroff(COLOR_PAIR(3));
}


void bottomError(char *msg) {
  move(LINES - 1, 0);
  attron(COLOR_PAIR(9));
  whline(stdscr, ' ', COLS);
  move(LINES - 1, 0);
  printw("%s ", msg);
  attroff(COLOR_PAIR(9));
  attron(A_BOLD);
  printw("ENTER");
  attroff(A_BOLD);
  attron(COLOR_PAIR(9));
  printw(" OK");
  attroff(COLOR_PAIR(9));
}


void bottomFind(void) {
  move(LINES - 1, 0);
  attron(COLOR_PAIR(3));
  whline(stdscr, ' ', COLS);
  move(LINES - 1, 0);
  printw("Find: ");
  attron(COLOR_PAIR(8));
  printw("________");
  attron(COLOR_PAIR(3));
  printw(" ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("O");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" ORG parameter ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("A");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Offset ADDRESS ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("ESC");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Cancel ");
  attroff(COLOR_PAIR(3));
}


void bottomQuit(void) {
  move(LINES - 1, 0);
  attron(COLOR_PAIR(9));
  whline(stdscr, ' ', COLS);
  move(LINES - 1, 0);
  printw("Quit without saving? ");
  attroff(COLOR_PAIR(9));
  attron(A_BOLD);
  printw("Y");
  attroff(A_BOLD);
  attron(COLOR_PAIR(9));
  printw(" Yes ");
  attroff(COLOR_PAIR(9));
  attron(A_BOLD);
  printw("N");
  attroff(A_BOLD);
  attron(COLOR_PAIR(9));
  printw(" No, cancel ");
  attroff(COLOR_PAIR(9));
  attron(A_BOLD);
  printw("S");
  attroff(A_BOLD);
  attron(COLOR_PAIR(9));
  printw(" Save and quit");
  attroff(COLOR_PAIR(9));
}


void resizeHandler(void) {
  clear();
  wresize(stdscr, getmaxy, getmaxx);
  refresh();

  mainWindow();
  mainTop();
  mainBottom();
}


void mainWindow(void) {
  box(stdscr, ACS_VLINE, ACS_HLINE);
}


void mainBottom(void) {
  move(LINES - 1, 0);
  attron(COLOR_PAIR(3));
  whline(stdscr, ' ', COLS);
  move(LINES - 1, 0);
  printw("O/A: 00000070/00000070 ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("O");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Toggle O/A ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("^F");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Find ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("F2");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Edit ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("^S");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Save ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("F1");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" About ");
  attroff(COLOR_PAIR(3));
  attron(A_BOLD);
  printw("^Q");
  attroff(A_BOLD);
  attron(COLOR_PAIR(3));
  printw(" Quit");
  attroff(COLOR_PAIR(3));
}


void mainTop(void) {
  move(0, 3);
  printw(" HexOrg - arquivo.ext ");
}


void endSession(void) {
  endwin();
  exit(EXIT_SUCCESS);
}
