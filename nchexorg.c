// WIP: Use "gcc -lncurses -ltinfo -o nchexorg nchexorg.c && strip -s nchexorg"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ncurses.h>


int key;


void beginSession(void);
void endSession(void);
void initColors(void);
void bottomAbout(void);
void bottomEdit(void);
void bottomFind(void);
void bottomMsg(char *msg, char type);
void bottomQuit(void);
void mainBottom(void);
void mainTop(void);
void mainWindow(void);
void resizeHandler(void);


int main(int argc, char **argv) {
  beginSession();

  mainWindow();
  mainTop();
  mainBottom();

  while (1) {
    key = getch();
    if (key == KEY_RESIZE)
      resizeHandler();
    else if (key == 'o')
      mainBottom();
    else if (key == 'f')
      bottomFind();
    else if (key == 'e')
      bottomEdit();
    else if (key == 's')
      bottomMsg("File saved successfully", NULL);
    else if (key == 'a')
      bottomAbout();
    else if (key == 'q')
      bottomQuit();
    else if (key == 'b')
      bottomMsg("Voce apertou o botao errado", 'e');
  }
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
  bottomMsg(
    "HexOrg - (c) @facmachado [MIT] "
    "https://github.com/facmachado/tools", NULL
  );
}


void bottomEdit(void) {
  attron(COLOR_PAIR(3));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(
    LINES - 1, 0,
    "O/A: 00000070/00000070 "
    "Old code: 72 New code: __ "
    "ESC Cancel ENTER Confirm"
  );
  attroff(COLOR_PAIR(3));
  mvchgat(LINES - 1, 5, 8,  A_REVERSE, COLOR_PAIR(6), NULL);
  mvchgat(LINES - 1, 14, 8, A_REVERSE, COLOR_PAIR(4), NULL);
  mvchgat(LINES - 1, 49, 3, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 60, 5, A_BOLD, COLOR_PAIR(1), NULL);
  while (1) {
    key = getch();
    if (key == 0x1b) {
      mainBottom();
      break;
    }
    else if (key == 0xa) {
      mainBottom();
      break;
    }
  }
}


void bottomFind(void) {
  attron(COLOR_PAIR(3));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(
    LINES - 1, 0,
    "Find: ________ O ORG parameter "
    "A Offset ADDRESS ESC Cancel"
  );
  attroff(COLOR_PAIR(3));
  mvchgat(LINES - 1, 15, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 31, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 48, 3, A_BOLD, COLOR_PAIR(1), NULL);
  while (1) {
    key = getch();
    if (key == 0x1b) {
      mainBottom();
      break;
    }
  }
}


void bottomMsg(char *msg, char type) {
  attron(COLOR_PAIR(type == 'e' ? 9 : 10));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(LINES - 1, 0, "%s ENTER OK", msg);
  attroff(COLOR_PAIR(type == 'e' ? 9 : 10));
  mvchgat(LINES - 1, strlen(msg) + 1, 5, A_BOLD, COLOR_PAIR(1), NULL);
  while (1) {
    key = getch();
    if (key == 0xa) {
      mainBottom();
      break;
    }
  }
}


void bottomQuit(void) {
  attron(COLOR_PAIR(9));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(
    LINES - 1, 0,
    "Save before quit? "
    "Y Yes N No ESC Cancel"
  );
  attroff(COLOR_PAIR(9));
  mvchgat(LINES - 1, 18, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 24, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 29, 3, A_BOLD, COLOR_PAIR(1), NULL);
  while (1) {
    key = getch();
    if (key == 0x1b) {
      mainBottom();
      break;
    } else if (key == 'y') {
      bottomMsg("File saved successfully", NULL);
      endSession();
    } else if (key == 'n')
      endSession();
  }
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
  attron(COLOR_PAIR(3));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(
    LINES - 1, 0,
    "O/A: 00000070/00000070 "
    "O Toggle O/A F Find E Edit "
    "S Save A About Q Quit"
  );
  attroff(COLOR_PAIR(3));
  mvchgat(LINES - 1, 5,  8, A_REVERSE, COLOR_PAIR(6), NULL);
  mvchgat(LINES - 1, 14, 8, A_REVERSE, COLOR_PAIR(4), NULL);
  mvchgat(LINES - 1, 23, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 36, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 43, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 50, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 57, 1, A_BOLD, COLOR_PAIR(1), NULL);
  mvchgat(LINES - 1, 65, 1, A_BOLD, COLOR_PAIR(1), NULL);
}


void mainTop(void) {
  mvprintw(0, 3, " HexOrg - arquivo.ext ");
}


void endSession(void) {
  endwin();
  exit(EXIT_SUCCESS);
}
