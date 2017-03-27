#include <algorithm>
#include <ncurses.h>
#include <termios.h>
#include <dirent.h>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
	initscr();
	clear();
	cbreak();
	noecho();
	refresh();
    WINDOW *w[3] = {newwin(LINES -5, COLS/2, 0, 0), newwin(LINES - 5, COLS/2, 0, COLS/2),
                    newwin(5, COLS, LINES-5, 0)};
    int ch;
    mvwprintw(w[2], 1, 1, "HELP:\n press 'ENTER' to choose a dirent \t press 'TAB' to switch the window \n press F10 to exit");
    box(w[2], 0, 0);
    wrefresh(w[2]);

    struct dirent *readdir(DIR *pathname);
    std::string str[2];
    DIR *path[2];
    path[0] = opendir("/");
    path[1] = opendir("/");
    dirent *pathn;
    std::map<DIR*, std::vector<dirent*> > menu;
    
    wrefresh(w[0]);
    wrefresh(w[1]);
    keypad( w[0], TRUE );
    keypad( w[1], FALSE );
    curs_set( 0 );
    mvwprintw(w[0], 0, 2, "/");
    mvwprintw(w[1], 0, 2, "/");
    int i = 0;
        while ((pathn = readdir(path[0])) != NULL) {
            std::string dn(pathn->d_name);
            if (dn != "." && dn != "..") {
                menu[path[0]].push_back(pathn);
                menu[path[1]].push_back(pathn);
                mvwaddstr(w[0], i+1, 2, menu[path[0]][i]->d_name);
                mvwaddstr(w[1], i+1, 2, menu[path[1]][i]->d_name);
                waddch(w[0], '\n');
                waddch(w[1], '\n');
                i++;
            }
        }
    i = 0;
    int curr = 0;
    wrefresh(w[0]);
    wattron( w[0], A_STANDOUT );
    mvwprintw( w[0], i+1, 2, menu[path[curr]][i]->d_name);
    wattroff( w[0], A_STANDOUT );
    box(w[0], 0, 0);
    box(w[1], 0, 0);
    mvwprintw(w[0], 0, 2, "/");
    mvwprintw(w[1], 0, 2, "/");
    wrefresh(w[1]);
   
    bool change = false;
    while((ch = wgetch(w[curr])) != KEY_F(10)) {
        
        mvwprintw( w[curr], i+1, 2, menu[path[curr]][i]->d_name);
        switch( ch ) {
                case KEY_UP:
                    i--;
                    i = ( i < 0 ) ? menu[path[curr]].size() - 1 : i;
                    break;
                case KEY_DOWN:
                    i++;
                    i = ( i > menu[path[curr]].size() - 1) ? 0 : i;
                    break;
                case '\t':
				    keypad(w[curr], FALSE);
				    curr = 1 - curr;
				    keypad(w[curr], TRUE);
				    i = 0;
				    wattron( w[curr], A_STANDOUT );
                    mvwprintw( w[curr], i+1, 2, menu[path[curr]][i]->d_name);
                    wattroff( w[curr], A_STANDOUT );
				    break;
                case 10:
                    wclear(w[curr]);
                    if (str[curr] == "/")
                        str[curr].clear();
                    str[curr] += "/";
                    str[curr] += menu[path[curr]][i]->d_name;
                    if (str[curr][str[curr].size()-1] == '.' && str[curr][str[curr].size()-2] != '.') {
                        str[curr].erase(str[curr].rfind('/'), 2);
                    }
                    char p[200];
                    strcpy(p, str[curr].c_str());
                    mvwaddstr(w[curr], 1, 2, p);
                    path[curr] = opendir(p);
                    int j = 0;
                    while ((pathn = readdir(path[curr])) != NULL) {
                        std::string dn(pathn->d_name);
                        if (dn != ".") {
                            menu[path[curr]].push_back(pathn);
                            mvwaddstr(w[curr], j+1, 2, menu[path[curr]][j]->d_name);
                            waddch(w[curr], '\n');
                            j++;
                        }
                    }
                    i = 0;
                    box(w[curr], 0, 0);
                    char c = '.';
                    if (str[curr][str[curr].size()-2] == c) {
                        str[curr].erase(str[curr].rfind('/'), 3);
                        str[curr].erase(str[curr].rfind('/'), str[curr].size() - (str[curr].size() - str[curr].rfind('/')));
                        std::string gg(menu[path[curr]][0]->d_name);
                        if (str[curr].rfind('/') == 0 && gg == "var")
                            str[curr] = '/';
                        strcpy(p, str[curr].c_str());
                    }
                    if (str[curr][str[curr].size()-1] == c) {
                        str[curr].erase(str[curr].rfind('/'), 2);
                    }
                    wrefresh(w[curr]);
                    mvwprintw(w[curr], 0, 2, p);
                    break;
        }
        
        wattron( w[curr], A_STANDOUT );
        mvwprintw( w[curr], i+1, 2, menu[path[curr]][i]->d_name);
        wattroff( w[curr], A_STANDOUT );
        
    }
    delwin(w[0]);
    delwin(w[1]);
    endwin();
}
