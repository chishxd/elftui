#include <curses.h>

WINDOW *left_pane = NULL;
WINDOW *right_pane = NULL;
const char *items[3] = {"Item 1", "Item 2", "Item 3"};

int selected = 0;

void draw_content() {
  for (int i = 0; i < 3; i++) {
    if (i == selected) { wattron(left_pane, A_REVERSE); }
    mvwprintw(left_pane, i + 2, 5, "%s", items[i]);
    wattroff(left_pane, A_REVERSE);
  }
}

void setup_panes() {
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
}

int main() {

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(FALSE);

  setup_panes();
  draw_content();

  refresh();
  wrefresh(left_pane);
  wrefresh(right_pane);

  int ch;

  while ((ch = getch()) != 'q') {
    switch (ch) {
    case KEY_RESIZE:
      clear();
      refresh();

      setup_panes();
      break;
    case KEY_UP:
      selected = (selected == 0) ? 3 - 1 : selected - 1;

      draw_content();
      break;
    case KEY_DOWN:
      selected = (selected == 3 - 1) ? 0 : selected + 1;
      draw_content();
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
