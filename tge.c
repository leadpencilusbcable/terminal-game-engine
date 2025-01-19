#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#include "tge.h"

//TODO handle potential error codes from functions like tcsetattr

static struct termios term_init_flags;
static struct termios term_cur_flags;

static tge_resize_callback resize_callback;

unsigned short tge_rows;
unsigned short tge_cols;

unsigned short tge_cursor_x;
unsigned short tge_cursor_y;

static int kbhit(void){
  int k;
  ioctl(STDIN_FILENO, FIONREAD, &k);
  return k;
}

static void set_window_size(void){
  struct winsize winsize;
  ioctl(1, TIOCGWINSZ, &winsize);

  tge_rows = winsize.ws_row;
  tge_cols = winsize.ws_col;
}

static void handle_terminal_resize(int sig){
  set_window_size();

  if(resize_callback != NULL){
    resize_callback(tge_rows, tge_cols);
  }
}

inline void tge_flush(void){
  fflush(stdout);
}

inline void tge_clear(void){
  fputs(TGE_CLEAR, stdout);
}

inline void tge_cursor_off(void){
  fputs(TGE_CURSOR_OFF, stdout);
}

inline void tge_cursor_on(void){
  fputs(TGE_CURSOR_ON, stdout);
}

void tge_echo_off(void){
  term_cur_flags.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term_cur_flags);
}

void tge_echo_on(void){
  term_cur_flags.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term_cur_flags);
}

void tge_canonical_mode_off(void){
  term_cur_flags.c_lflag &= ~ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &term_cur_flags);
}

void tge_canonical_mode_on(void){
  term_cur_flags.c_lflag |= ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &term_cur_flags);
}

void tge_raw_mode(void){
  term_cur_flags.c_iflag &= ~(ICRNL | IXON);
  term_cur_flags.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  tcsetattr(STDIN_FILENO, TCSANOW, &term_cur_flags);
}

inline void tge_cursor_move_reset(void){
  fputs(TGE_CURSOR_HOME, stdout);
}

void tge_cursor_move_xy(int x, int y){
  printf("\x1B[%d;%dH", y, x);
  tge_cursor_x = x;
  tge_cursor_y = y;
}

bool tge_cursor_move_left(unsigned short n){
  if(n >= tge_cursor_x + 1){
    return false;
  }

  unsigned short new_x = tge_cursor_x - n;

  printf("\x1B[%huD", n);
  tge_cursor_x = new_x;

  return true;
}

bool tge_cursor_move_right(unsigned short n){
  unsigned short new_x = tge_cursor_x + n;

  if(new_x >= tge_cols){
    return false;
  }

  printf("\x1B[%huC", n);
  tge_cursor_x = new_x;

  return true;
}

bool tge_cursor_move_up(unsigned short n){
  if(n >= tge_cursor_y + 1){
    return false;
  }

  unsigned short new_y = tge_cursor_y - n;

  printf("\x1B[%huA", n);
  tge_cursor_y = new_y;

  return true;
}

bool tge_cursor_move_down(unsigned short n){
  unsigned short new_y = tge_cursor_y + n;

  if(new_y >= tge_rows){
    return false;
  }

  printf("\x1B[%huB", n);
  tge_cursor_y = new_y;

  return true;
}

void tge_init_term_flags(void){
  tcgetattr(STDIN_FILENO, &term_cur_flags);
  term_init_flags = term_cur_flags;
}

void tge_init(void){
  tge_init_term_flags();
  tge_raw_mode();

  tge_cursor_off();
  tge_clear();
  tge_cursor_move_reset();

  tge_flush();

  set_window_size();

  struct sigaction sigact;
  sigact.sa_handler = &handle_terminal_resize;
  sigaction(SIGWINCH, &sigact, NULL);
}

void tge_clean(void){
  tcsetattr(1, TCSANOW, &term_init_flags);
  tge_cursor_on();
  tge_flush();
}

void tge_set_resize_callback(tge_resize_callback callback){
  resize_callback = callback;
}

int tge_get_key(void){
  if(kbhit()){
    char c;
    read(STDIN_FILENO, &c, 1);

    switch(c){
      case 'a':
      case 'A':
        return TGE_KEY_A;
      case 'b':
      case 'B':
        return TGE_KEY_B;
      case 'c':
      case 'C':
        return TGE_KEY_C;
      case 'd':
      case 'D':
        return TGE_KEY_D;
      case 'e':
      case 'E':
        return TGE_KEY_E;
      case 'f':
      case 'F':
        return TGE_KEY_F;
      case 'g':
      case 'G':
        return TGE_KEY_G;
      case 'h':
      case 'H':
        return TGE_KEY_H;
      case 'i':
      case 'I':
        return TGE_KEY_I;
      case 'j':
      case 'J':
        return TGE_KEY_J;
      case 'k':
      case 'K':
        return TGE_KEY_K;
      case 'l':
      case 'L':
        return TGE_KEY_L;
      case 'm':
      case 'M':
        return TGE_KEY_M;
      case 'n':
      case 'N':
        return TGE_KEY_N;
      case 'o':
      case 'O':
        return TGE_KEY_O;
      case 'p':
      case 'P':
        return TGE_KEY_P;
      case 'q':
      case 'Q':
        return TGE_KEY_Q;
      case 'r':
      case 'R':
        return TGE_KEY_R;
      case 's':
      case 'S':
        return TGE_KEY_S;
      case 't':
      case 'T':
        return TGE_KEY_T;
      case 'u':
      case 'U':
        return TGE_KEY_U;
      case 'v':
      case 'V':
        return TGE_KEY_V;
      case 'w':
      case 'W':
        return TGE_KEY_W;
      case 'x':
      case 'X':
        return TGE_KEY_X;
      case 'y':
      case 'Y':
        return TGE_KEY_Y;
      case 'z':
      case 'Z':
        return TGE_KEY_Z;
      case ' ':
        return TGE_KEY_SPACE;
      case '\x1B': {
        char esc[3];
        ssize_t bytes_read = read(STDIN_FILENO, esc, 3);

        switch(bytes_read){
          case 0:
            return TGE_KEY_ESC;
          case 2:
            if(esc[0] == '['){
              switch(esc[1]){
                case 'A': return TGE_KEY_UP;
                case 'B': return TGE_KEY_DOWN;
                case 'C': return TGE_KEY_RIGHT;
                case 'D': return TGE_KEY_LEFT;
              }
            }
        }
      }
    }
  }

  return TGE_KEY_NONE;
}

static bool cursor_move_valid(int x, int y){
  if(x >= tge_cols || y >= tge_rows){
    return false;
  }
  if(x <= 0 || y <= 0){
    return false;
  }

  return true;
}

void tge_draw_game_object(struct tge_game_object game_object){
  char* itr = game_object.text;

  int cur_x = game_object.pos.x;
  int cur_y = game_object.pos.y;

  //TODO: maybe pass in string struct that specifies length
  while(*itr != '\0'){
    tge_cursor_move_xy(cur_x, cur_y);

    if(cursor_move_valid(cur_x, cur_y)){
      putchar(*itr);
    }

    if(*itr == '\n'){
      cur_x = game_object.pos.x;
      cur_y++;
    } else {
      cur_x++;
    }

    itr++;
  }
}

void tge_clear_game_object(struct tge_game_object game_object){
  char* itr = game_object.text;

  int cur_x = game_object.pos.x;
  int cur_y = game_object.pos.y;

  while(*itr != '\0'){
    tge_cursor_move_xy(cur_x, cur_y);

    if(cursor_move_valid(cur_x, cur_y)){
      putchar(' ');
    }

    if(*itr == '\n'){
      cur_x = game_object.pos.x;
      cur_y++;
    } else {
      cur_x++;
    }

    itr++;
  }
}

