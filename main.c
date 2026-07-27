#include "parser.h"
#include <curses.h>

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
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

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
    return 1;
  }

  const char *filename = argv[1];

  // 2. Open and read the file into your Elf_Header
  int file = open(filename, O_RDONLY);
  if (file == -1) {
    perror("Failed to open file");
    return 1;
  }

  Elf_Header header;
  Elf_Metadata meta = {0};

  if (read(file, &header, sizeof(header)) != sizeof(header)) {
    perror("Error reading ELF file");
    close(file);
    return 1;
  }
  close(file);

  // 3. Extract the metadata
  if (parse_elf_header(&header, &meta) != 0) {
    return 1; // Terminate if it's not a valid ELF [1.1.5]
  }

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
    default: break;
    }
    wrefresh(left_pane);
    wrefresh(right_pane);
  }

  if (left_pane) delwin(left_pane);
  if (right_pane) delwin(right_pane);
  endwin();

  return 0;
}
