#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define KEY_q    113
#define KEY_r    114

using namespace std;

bool terminated = false;
int selected = 0;
string editor = "nano +%d %s";

int launch_editor(pair<string,int> location) {
	char buffer[255];
	
	newwin(0, 0, 0, 0);
	sprintf(buffer, editor.c_str(), location.second, location.first.c_str());
	system(buffer);
	endwin();

	return 0;
}

void clear_line(int y, int l) {
  move(y, 1);
  string str;
  for (int i = 0; i < l; i++) str.push_back(' ');
  mvaddstr(y, 0, str.c_str());
}

pair<string, int> parse_line(string line) {
	// filename:lineno:content
	char *token = strtok(strdup(line.c_str()), ":");
	string filename;
	int lineno;
	
	if (token) {
		filename = token;
		token = strtok(nullptr, ":");
		
		if (tokens) {
			lineno = atoi(token);
		}
	}
	
	return make_pair(filename, lineno);
}

void refresh_screen(WINDOW *win, vector<string> *lines) {
	int x, y;
	getmaxyx(win, y, x);
	
	for (int i = 0; i <= y; i++) {
		clear_line(i, x);
	}
	
	int i = 0;
	for (auto it = lines->begin(); it != lines->end(); ++it, i++) {
		if (selected == i) {
			attrset(COLOR_PAIR(2));
		} else {
			attrset(COLOR_PAIR(1));
		}
		
		clear_line(i, x);
		mvaddstr(i, 0, it->c_str());
	}
	
	attrset(COLOR_PAIR(1));
	refresh();
	
	int key = getch();
	
	switch (key) {
		case KEY_q: terminated = true; break;
		case KEY_UP: 
			if (selected > 0) {
				selected--; 
			}
			
			break;
		case KEY_DOWN: 
			if (selected < i - 1) {
				selected++;
			}
			
			break;
			
		case 13:
		case KEY_ENTER:
			launch_editor(parse_line(lines->at(selected)));
			break;
	}
}

auto main() -> int {
	initscr();
	keypad(stdscr, true);			// To make the getch() function work with single key values
	nonl();							// To make return/enter work
	noecho();
	cbreak();
	
	string line;
	vector<string> lines;
	WINDOW *win = newwin(0, 0, 0, 0);
	
	if (!isatty(fileno(stdin))) {
		while (getline(cin, line)) {
			lines.push_back(line);
		}
	
		freopen(ttyname(fileno(stdout)), "r", stdin);
	}
	
	if (has_colors()) {
		start_color();
		
		init_pair(1, COLOR_WHITE,   COLOR_BLACK);
		init_pair(2, COLOR_GREEN,   COLOR_BLUE);
		init_pair(3, COLOR_BLACK,   COLOR_CYAN);
	}

	if(const char* env_editor = getenv("GREDITOR")) {
		editor = strdup(env_editor);
	}

	while (!terminated) {	
		refresh_screen(win, &lines);
		usleep(1000);
	}
	
	endwin();
	
	return 0;
}