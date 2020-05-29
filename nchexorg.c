// WIP: Use "gcc -lncurses -ltinfo -o nchexorg nchexorg.c && strip -s nchexorg"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ncurses.h>


int end, key;


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
  end = 0;
  beginSession();

  mainWindow();
  mainTop();
  mainBottom();

  while (!end) {
    key = getch();
    if (key == KEY_RESIZE)
      resizeHandler();
    else if (key == 't')
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
   * 1: ASCII view, 2: bottom bar
   */
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_GREEN);

  /**
   * 3: Help color (about)
   *    Offset address mode colors
   *    (A_NORMAL: list, A_REVERSE: cursor)
   */
  init_pair(3, COLOR_WHITE, COLOR_BLUE);

  /**
   * 4: Dialog color (question/error)
   *    Origin parameter mode colors
   *    (A_NORMAL: list, A_REVERSE: cursor)
   */
  init_pair(4, COLOR_WHITE, COLOR_RED);
}


void bottomAbout(void) {
  bottomMsg(
    "HexOrg - (c) @facmachado [MIT] "
    "https://github.com/facmachado/tools", NULL
  );
}


void bottomEdit(void) {
  attron(COLOR_PAIR(2));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(
    LINES - 1, 0,
    "O/A: 00000000/00000000 "
    "Old code: 72 New code: __ "
    "ESC=Cancel ENTER=Confirm"
  );
  attroff(COLOR_PAIR(2));
  mvchgat(LINES - 1, 5, 8,  A_NORMAL, 4, NULL);
  mvchgat(LINES - 1, 14, 8, A_NORMAL, 3, NULL);
  mvchgat(LINES - 1, 46, 2, A_REVERSE, 0, NULL);
  mvchgat(LINES - 1, 49, 3, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 60, 5, A_BOLD, 0, NULL);
  while (1) {
    key = getch();
    if (key == 0x1b) {
      mainBottom();
      break;
    } else if (key == 0xa) {
      mainBottom();
      break;
    }
  }
}


void bottomFind(void) {
  attron(COLOR_PAIR(2));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(
    LINES - 1, 0,
    "Find: ________ ENTER=Offset Address "
    "Shift+ENTER=Origin Parameter ESC=Cancel"
  );
  attroff(COLOR_PAIR(2));
  mvchgat(LINES - 1, 6, 8, A_REVERSE, 0, NULL);
  mvchgat(LINES - 1, 15, 5, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 36, 11, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 65, 3, A_BOLD, 0, NULL);
  while (1) {
    key = getch();
    if (key == 0xa) {
      mainBottom();
      break;
    // } else if (key == 0xa) {
    //   mainBottom();
    //   break;
    } else if (key == 0x1b) {
      mainBottom();
      break;
    }
  }
}


void bottomMsg(char *msg, char type) {
  attron(COLOR_PAIR(type == 'e' ? 4 : 3));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(LINES - 1, 0, "%s ENTER=OK", msg);
  attroff(COLOR_PAIR(type == 'e' ? 4 : 3));
  mvchgat(LINES - 1, strlen(msg) + 1, 5, A_BOLD, 0, NULL);
  while (1) {
    key = getch();
    if (key == 0xa) {
      mainBottom();
      break;
    }
  }
}


void bottomQuit(void) {
  attron(COLOR_PAIR(4));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(
    LINES - 1, 0,
    "Save before quit? "
    "Yes No ESC=Cancel"
  );
  attroff(COLOR_PAIR(4));
  mvchgat(LINES - 1, 18, 1, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 22, 1, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 25, 3, A_BOLD, 0, NULL);
  while (1) {
    key = getch();
    if (key == 0x1b) {
      mainBottom();
      break;
    } else if (key == 'y') {
      bottomMsg("File saved successfully", NULL);
      end = 1;
      break;
    } else if (key == 'n') {
      end = 1;
      break;
    }
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
  attron(COLOR_PAIR(2));
  mvhline(LINES - 1, 0, ' ', COLS);
  mvprintw(
    LINES - 1, 0,
    "O/A: 00000000/00000000 "
    "Toggle O/A Find Edit "
    "Save About Quit"
  );
  attroff(COLOR_PAIR(2));
  mvchgat(LINES - 1, 5,  8, A_NORMAL, 4, NULL);
  mvchgat(LINES - 1, 14, 8, A_NORMAL, 3, NULL);
  mvchgat(LINES - 1, 23, 1, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 34, 1, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 39, 1, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 44, 1, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 49, 1, A_BOLD, 0, NULL);
  mvchgat(LINES - 1, 55, 1, A_BOLD, 0, NULL);
}


void mainTop(void) {
  mvprintw(0, 3, " HexOrg - arquivo.ext ");
}


void endSession(void) {
  endwin();
  exit(EXIT_SUCCESS);
}
