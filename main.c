#include "parser.h"
#include <curses.h>

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
WINDOW *left_pane = NULL;
WINDOW *right_pane = NULL;
const char *items[3] = {"ELF Header", "Program Headers", "Section Headers"};

int selected = 0;

void draw_content() {
  for (int i = 0; i < 3; i++) {
    if (i == selected) { wattron(left_pane, A_REVERSE); }
    mvwprintw(left_pane, i + 2, 5, "%s", items[i]);
    wattroff(left_pane, A_REVERSE);
  }
}

void draw_right_pane(const Elf_Metadata *meta) {
  werase(right_pane);
  box(right_pane, 0, 0);

  if (selected == 0) {
    mvwprintw(right_pane, 1, 2, "---Elf Header Info---");
    mvwprintw(right_pane, 3, 2, "Class:              %s", meta->class_str);
    mvwprintw(right_pane, 4, 2, "Data:               %s", meta->endian_str);
    mvwprintw(right_pane, 5, 2, "Version:            %d", meta->version);
    mvwprintw(right_pane, 6, 2, "OS/ABI:             %s", meta->os_abi);
    mvwprintw(right_pane, 7, 2, "ABI VERSION:        %d", meta->abi_version);
    mvwprintw(right_pane, 8, 2, "ENTRY POINT:        0x%lx", meta->entry_point);
    mvwprintw(right_pane, 9, 2, "FLAGS:              0x%x", meta->flags);
    mvwprintw(right_pane, 10, 2, "Header Size:        %d", meta->eh_size);
  } else if (selected == 1) {
    mvwprintw(right_pane, 1, 2, "---Program Headers---");
    mvwprintw(right_pane, 3, 2, "Table Offset:       0x%lx", meta->ph_offset);
    mvwprintw(right_pane, 4, 2, "Entry Size:         %d", meta->ph_entry_size);
    mvwprintw(right_pane, 5, 2, "Total Entries:      %d", meta->ph_num);
  } else if (selected == 2) {
    mvwprintw(right_pane, 1, 2, "---Section Headers---");
    mvwprintw(right_pane, 3, 2, "Table Offset:       0x%lx", meta->sh_offset);
    mvwprintw(right_pane, 4, 2, "Entry Size:         %d", meta->sh_entry_size);
    mvwprintw(right_pane, 5, 2, "Total Entries:      %d", meta->sh_num);
    mvwprintw(right_pane, 6, 2, "Name Str Ndx:       %d", meta->sh_str_ndx);
  }

  wrefresh(right_pane);
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
    return 1; // Terminate if it's not a valid ELF
  }

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(FALSE);

  setup_panes();
  refresh();
  draw_content();
  draw_right_pane(&meta);

  wrefresh(left_pane);

  int ch;

  while ((ch = getch()) != 'q') {
    switch (ch) {
    case KEY_RESIZE:
      clear();
      refresh();

      setup_panes();
      draw_content();
      draw_right_pane(&meta);
      break;
    case KEY_UP:
      selected = (selected == 0) ? 3 - 1 : selected - 1;

      draw_content();
      draw_right_pane(&meta);
      break;
    case KEY_DOWN:
      selected = (selected == 3 - 1) ? 0 : selected + 1;
      draw_content();
      draw_right_pane(&meta);
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
