#include <curses.h>

WINDOW *left_pane = NULL;
WINDOW *right_pane = NULL;

void create_panes() {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  if (left_pane) delwin(left_pane);
  if (right_pane) delwin(right_pane);

  int height = max_y;
  int lwidth = max_x / 4;

  if (lwidth < 18) lwidth = 18;

  if (lwidth > max_x - 10) lwidth = max_x - 10;

  int left_start = 0;
  int rwidth = max_x - lwidth;
  int right_start = lwidth;

  left_pane = newwin(height, lwidth, 0, left_start);
  right_pane = newwin(height, rwidth, 0, right_start);

  box(left_pane, 0, 0);
  box(right_pane, 0, 0);

  mvwprintw(left_pane, 1, 2, "Left Pane");
  mvwprintw(right_pane, 1, 2, "Right Pane");
}

int main() {

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(FALSE);

  create_panes();
    
    refresh();
    wrefresh(left_pane);
    wrefresh(right_pane);

  int ch;

  while ((ch = getch()) != 'q') {
    switch (ch) {
    case KEY_RESIZE:
      clear();
      refresh();

      create_panes();
      break;
    default:
      break;
    }
    wrefresh(left_pane);
    wrefresh(right_pane);
  }

  if (left_pane) delwin(left_pane);
  if (right_pane) delwin(right_pane);
  endwin();

  return 0;
}
