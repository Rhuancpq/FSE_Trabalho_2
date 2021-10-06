#include "Interface.hpp"

set<int> CODES = {};

void Interface::operator()(bool & is_running) {
    // init ncurses
    initscr();
    start_color();
    cbreak();
    nonl();               // Get return key
    keypad(stdscr, 1);    // Fix keypad
    noecho();             // No automatic printing
    curs_set(0);          // Hide real cursor
    timeout(200);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int win_l_y = max_y / 2 - 3;
    int win_l_x = max_x / 2 - 20;
    WINDOW *win_left = newwin(win_l_y, win_l_x, (max_y - win_l_y)/2, 1);
    WINDOW *win_right = newwin(win_l_y, win_l_x, (max_y - win_l_y)/2, max_x - win_l_x - 1);

    int current_server_index = 0;
    int server_num = 0;

    bool first_time_empty = false;

    while(true) {
        Servers * s = Servers::getInstance();
        vector<DistServers> all_states = s->getServers();
        server_num = all_states.size();
        

        wmove(stdscr, 0, 0);
        wclrtoeol(stdscr);
        wmove(stdscr, 1, 0);
        wclrtoeol(stdscr);

        if (server_num == 0) {
            if(!first_time_empty) {
                first_time_empty = true;
                clear();
            }
            mvprintw(0, max_x/2 - 24, "Não há nenhum servidor distribuído conectado");
            mvprintw(1, max_x/2 - 7, "Aguardando...");
            attron(COLOR_PAIR(3));
            mvprintw(max_y - 2, 3, "Use as setas (<- e ->) para alternar entre os servidores");
            mvprintw(max_y - 1, 3, "Use F1 para sair");
            attroff(COLOR_PAIR(3));
            refresh();

            int ch = getch();
            if(ch == KEY_F(1)){
                break;
            }
        } else {
            first_time_empty = false;
            DistServers current_server = all_states[current_server_index%server_num];
            // create menu
            stringstream server_header;
            server_header << current_server.name
            << " (" << current_server_index + 1 << "/" << all_states.size() << ")";

            stringstream info3;
            info3 << fixed;
            info3 << setprecision(1);
            info3 << "T: " << current_server.temperature << " ºC";
            info3 << " H: " << current_server.humidity << " %";

            string info2 = "IP: " + current_server.ip + " | porta: " + to_string(current_server.port);

            mvprintw(0, max_x - info3.str().length(), info3.str().c_str());
            mvprintw(0, 1, info2.c_str());
            mvprintw(0, max_x/2 - server_header.str().length()/2, server_header.str().c_str());
            
            // use instructions
            attron(COLOR_PAIR(3));
            mvprintw(max_y - 3, 3, "Use os números de 1 a 9 para alterar manualmente as saídas do servidor atual...");
            mvprintw(max_y - 2, 3, "Use as setas (<- e ->) para alternar entre os servidores");
            mvprintw(max_y - 1, 3, "Use F1 para sair");
            attroff(COLOR_PAIR(3));
            refresh();

            // TODO position data on vertical center of window
            werase(win_left);
            box(win_left, 0, 0);
            for(int i = 0; i < current_server.in_data.size(); i++){
                
                mvwprintw(win_left, i+2, 2, current_server.in_data[i].tag.c_str());
                if(current_server.in_data[i].value){
                    wattron(win_left, COLOR_PAIR(1));
                    mvwprintw(win_left, i+2, win_l_x - 4, "On");
                    wattroff(win_left, COLOR_PAIR(1));
                } else {
                    wattron(win_left, COLOR_PAIR(2));
                    mvwprintw(win_left, i+2, win_l_x - 5, "Off");
                    wattroff(win_left, COLOR_PAIR(2));
                }
            }
            wrefresh(win_left);

            werase(win_right);
            box(win_right, 0, 0);
            for(int i = 0; i < current_server.out_data.size(); i++){
                stringstream tag_prefix;
                tag_prefix << "(" << i + 1 << ") "
                << current_server.out_data[i].tag;
                mvwprintw(win_right, i+2, 2, tag_prefix.str().c_str());
                if(current_server.out_data[i].value){
                    wattron(win_right, COLOR_PAIR(1));
                    mvwprintw(win_right, i+2, win_l_x - 4, "On");
                    wattroff(win_right, COLOR_PAIR(1));
                } else {
                    wattron(win_right, COLOR_PAIR(2));
                    mvwprintw(win_right, i+2, win_l_x - 5, "Off");
                    wattroff(win_right, COLOR_PAIR(2));
                }
            }
            wrefresh(win_right);

            int ch = getch();
            char c = ch;
            if(ch == KEY_LEFT){
                current_server_index--;
                if(current_server_index < 0)
                    current_server_index = server_num - 1;
                werase(win_left);
            } else if(ch == KEY_RIGHT){
                current_server_index++;
                if(current_server_index >= server_num)
                    current_server_index = 0;
                werase(win_left);
            } else if(ch == KEY_F(1)){
                break;
            } else if(isdigit(c) and (c - '0') < (current_server.out_data.size()+1)) {
                mvprintw(max_y - 4, 3, "Output (%c) alterado manualmente!", c);
                Data current_state = current_server.out_data[c - '1'];
                Data new_state = {"action", current_state.tag, !current_state.value};
                Event e = {new_state, current_server, 0};
                EventQueue * eq = EventQueue::getInstance();
                eq->push(e);
            }
            
        }

        this_thread::sleep_for(10ms);
    } 

    delwin(win_left);
    endwin();
    is_running = false;
}