#include <curses.h>

int main() {
  int max_y, max_x;

  initscr();
  cbreak();
  curs_set(FALSE);
  noecho();

  getmaxyx(stdscr, max_y, max_x);

  int height = max_y;
  int width = max_x / 2;
  int left_start = 0;

  int right_start = width;

  WINDOW *left_pane = newwin(height, width, 0, left_start);
  WINDOW *right_pane = newwin(height, width, 0, right_start);

  box(left_pane, 0, 0);
  box(right_pane, 0, 0);

  mvwprintw(left_pane, 1, 2, "Left Pane");
  mvwprintw(right_pane, 1, 2, "Right Pane");

  refresh();
  wrefresh(left_pane);
  wrefresh(right_pane);

  getch();

  delwin(left_pane);
  delwin(right_pane);
  endwin();

  return 0;
}
