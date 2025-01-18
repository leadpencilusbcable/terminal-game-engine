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
//TODO I don't know if the current way of detecting keyboard input with atomics will work in actual situation
//also it doesn't handle multiple keys concurrently which I'm not sure is possible by reading terminal stdin

static struct termios term_init_flags;
static struct termios term_cur_flags;

static tge_resize_callback resize_callback;

static pthread_t keyboard_thread;

unsigned short tge_rows;
unsigned short tge_cols;

unsigned short tge_cursor_x;
unsigned short tge_cursor_y;

static atomic_llong tge_keys_pressed[TGE_KEYS_SIZE];

static struct timeval tv;

static long long current_time_ms(void){
  gettimeofday(&tv, NULL);
  return (((long long) tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
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

bool tge_cursor_move_xy(unsigned short x, unsigned short y){
  if(x >= tge_cols || y >= tge_rows){
    return false;
  }

  printf("\x1B[%hi;%hiH", y, x);
  tge_cursor_x = x;
  tge_cursor_y = y;

  return true;
}

bool tge_cursor_move_left(unsigned short n){
  unsigned short new_x = tge_cursor_x - n;

  if(new_x < 0){
    return false;
  }

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
  unsigned short new_y = tge_cursor_y - n;

  if(new_y < 0){
    return false;
  }

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

void* update_keys(void* args){
  while(true){
    char c = getchar();

    switch(c){
      case 'a':
      case 'A':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_A], current_time_ms(), memory_order_relaxed);
        break;
      case 'b':
      case 'B':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_B], current_time_ms(), memory_order_relaxed);
        break;
      case 'c':
      case 'C':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_C], current_time_ms(), memory_order_relaxed);
        break;
      case 'd':
      case 'D':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_D], current_time_ms(), memory_order_relaxed);
        break;
      case 'e':
      case 'E':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_E], current_time_ms(), memory_order_relaxed);
        break;
      case 'f':
      case 'F':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_F], current_time_ms(), memory_order_relaxed);
        break;
      case 'g':
      case 'G':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_G], current_time_ms(), memory_order_relaxed);
        break;
      case 'h':
      case 'H':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_H], current_time_ms(), memory_order_relaxed);
        break;
      case 'i':
      case 'I':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_I], current_time_ms(), memory_order_relaxed);
        break;
      case 'j':
      case 'J':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_J], current_time_ms(), memory_order_relaxed);
        break;
      case 'k':
      case 'K':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_K], current_time_ms(), memory_order_relaxed);
        break;
      case 'l':
      case 'L':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_L], current_time_ms(), memory_order_relaxed);
        break;
      case 'm':
      case 'M':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_M], current_time_ms(), memory_order_relaxed);
        break;
      case 'n':
      case 'N':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_N], current_time_ms(), memory_order_relaxed);
        break;
      case 'o':
      case 'O':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_O], current_time_ms(), memory_order_relaxed);
        break;
      case 'p':
      case 'P':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_P], current_time_ms(), memory_order_relaxed);
        break;
      case 'q':
      case 'Q':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_Q], current_time_ms(), memory_order_relaxed);
        break;
      case 'r':
      case 'R':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_R], current_time_ms(), memory_order_relaxed);
        break;
      case 's':
      case 'S':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_S], current_time_ms(), memory_order_relaxed);
        break;
      case 't':
      case 'T':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_T], current_time_ms(), memory_order_relaxed);
        break;
      case 'u':
      case 'U':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_U], current_time_ms(), memory_order_relaxed);
        break;
      case 'v':
      case 'V':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_V], current_time_ms(), memory_order_relaxed);
        break;
      case 'w':
      case 'W':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_W], current_time_ms(), memory_order_relaxed);
        break;
      case 'x':
      case 'X':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_X], current_time_ms(), memory_order_relaxed);
        break;
      case 'y':
      case 'Y':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_Y], current_time_ms(), memory_order_relaxed);
        break;
      case 'z':
      case 'Z':
        atomic_store_explicit(&tge_keys_pressed[TGE_KEY_Z], current_time_ms(), memory_order_relaxed);
        break;
    }
  }

  return NULL;
}

bool tge_key_pressed(unsigned int key_number){
  long long pressed_at_ms = atomic_load_explicit(&tge_keys_pressed[key_number], memory_order_relaxed);
  long long current_ms = current_time_ms();

  return (current_ms - pressed_at_ms) < TGE_KEY_PRESS_MILLIS;
}

void tge_reset_all_keys(void){
  for(size_t i = 0; i < TGE_KEYS_SIZE; i++){
    atomic_store_explicit(&tge_keys_pressed[i], false, memory_order_relaxed);
  }
}

void tge_init_key_events(void){
  pthread_create(&keyboard_thread, NULL, &update_keys, NULL);
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

  if(keyboard_thread != 0){
    pthread_cancel(keyboard_thread);
  }
}

void tge_set_resize_callback(tge_resize_callback callback){
  resize_callback = callback;
}

void tge_draw_game_object(struct tge_game_object game_object){
  bool cursor_move_valid = tge_cursor_move_xy(game_object.pos.x, game_object.pos.y);

  char* itr = game_object.text;

  while(*itr != '\0'){
    if(*itr == '\n'){
      cursor_move_valid = tge_cursor_move_xy(game_object.pos.x, tge_cursor_y + 1);
    } else if(cursor_move_valid){
      putchar(*itr);
    }

    itr++;
  }
}

void tge_clear_game_object(struct tge_game_object game_object){
  bool cursor_move_valid = tge_cursor_move_xy(game_object.pos.x, game_object.pos.y);

  unsigned short init_x_pos = tge_cursor_x;
  char* itr = game_object.text;

  while(*itr != '\0'){
    if(*itr == '\n'){
      cursor_move_valid = tge_cursor_move_xy(game_object.pos.x, tge_cursor_y + 1);
    } else if(cursor_move_valid){
      putchar(' ');
    }

    itr++;
  }
}

