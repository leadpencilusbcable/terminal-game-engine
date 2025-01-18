#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <termios.h>

//TODO: atomics don't work in c++
#ifdef __cplusplus
extern "C" {
#endif

#define TGE_CLEAR "\x1B[2J"
#define TGE_CURSOR_OFF "\x1B[?25l"
#define TGE_CURSOR_ON "\x1B[?25h"
#define TGE_CURSOR_HOME "\x1B[H"

#define TGE_KEY_A 0
#define TGE_KEY_B 1
#define TGE_KEY_C 2
#define TGE_KEY_D 3
#define TGE_KEY_E 4
#define TGE_KEY_F 5
#define TGE_KEY_G 6
#define TGE_KEY_H 7
#define TGE_KEY_I 8
#define TGE_KEY_J 9
#define TGE_KEY_K 10
#define TGE_KEY_L 11
#define TGE_KEY_M 12
#define TGE_KEY_N 13
#define TGE_KEY_O 14
#define TGE_KEY_P 15
#define TGE_KEY_Q 16
#define TGE_KEY_R 17
#define TGE_KEY_S 18
#define TGE_KEY_T 19
#define TGE_KEY_U 20
#define TGE_KEY_V 21
#define TGE_KEY_W 22
#define TGE_KEY_X 23
#define TGE_KEY_Y 24
#define TGE_KEY_Z 25

/*The default amount of time after last key press where a key will count as pressed*/
#define TGE_KEY_PRESS_MILLIS 200

extern unsigned short tge_rows;
extern unsigned short tge_cols;

extern unsigned short tge_cursor_x;
extern unsigned short tge_cursor_y;

#define TGE_KEYS_SIZE 26

/*Output all values written to stdout*/
void tge_flush(void);
/*Clear the terminal screen*/
void tge_clear(void);
/*Turn the terminal cursor off*/
void tge_cursor_off(void);
/*Turn the terminal cursor on*/
void tge_cursor_on(void);
/*Turn the echo off. Terminal will NOT display user's input*/
void tge_echo_off(void);
/*Turn the echo on. Terminal will display user's input*/
void tge_echo_on(void);
/*Turn canonical mode on. Terminal will buffer input as normal*/
void tge_canonical_mode_off(void);
/*Turn canonical mode off. Terminal will not buffer input until enter is pressed*/
void tge_canonical_mode_on(void);
/*Disable canonical mode, echo and a few other things.
  This will also disable user commands that control the terminal, such as ctrl c to exit.*/
void tge_raw_mode(void);
/*Reset cursor to 0 0*/
void tge_cursor_move_reset(void);
/*Move the cursor to specified location*/
bool tge_cursor_move_xy(unsigned short x, unsigned short y);
/*Move cursor left n positions*/
bool tge_cursor_move_left(unsigned short n);
/*Move cursor right n positions*/
bool tge_cursor_move_right(unsigned short n);
/*Move cursor up n positions*/
bool tge_cursor_move_up(unsigned short n);
/*Move cursor down n positions*/
bool tge_cursor_move_down(unsigned short n);

struct tge_vec3 {
  int x;
  int y;
  int z;
};

struct tge_game_object {
  struct tge_vec3 pos;
  char* text;
};

/*Initialise thread that listens for key presses and sets them in tge_keys*/
void tge_init_key_events(void);
/*Retrieve initial terminal flags to be used in subsequent calls to tcsetattr*/
void tge_init_term_flags(void);
/*Initialise terminal. Can be done manually to customise behaviour.
  Take a look at the source and copy the parts that suit your needs. */
void tge_init(void);
/*Cleans up terminal. Attempts to reset to state before running program*/
void tge_clean(void);
/*Check if a key has been pressed since last reset.
  Keys in range 0-TGE_KEYS_SIZE. Macros defined for all keys, eg TGE_KEY_A*/
bool tge_key_pressed(unsigned int key_number);
/*Reset all keys to false*/
void tge_reset_all_keys(void);

typedef void (*tge_resize_callback) (unsigned short rows, unsigned short cols);
/*Set a callback that is executed whenever terminal window is resized*/
void tge_set_resize_callback(tge_resize_callback callback);
/*Draw a game object to the screen*/
void tge_draw_game_object(struct tge_game_object game_object);
/*Clear a game object from the screen*/
void tge_clear_game_object(struct tge_game_object game_object);

#ifdef __cplusplus
}
#endif

