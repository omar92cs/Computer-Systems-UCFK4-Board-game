/** @file   defender.h
    @author itb14, aaa113, ran86
    @date   12 October 2017
    @brief  Interface for castle defender game.
*/

#ifndef CASTLE_DEFENDER_H
#define CASTLE_DEFENDER_H

// Driver and Util imports
#include "system.h"
#include "tinygl.h"
#include "ir_uart.h"
#include "../fonts/font3x5_1.h"

#define CAT_DEF_DISPLAY_REFRESH_FREQ 1000
#define CAT_DEF_SCROLL_CHAR_PER_10_SEC 15
#define FLASH_DELAY 8
#define NUM_SPAWN_ROW 6

// Game values
#define SCREEN_X_MAX TINYGL_HEIGHT - 1
#define SCREEN_Y_MAX TINYGL_WIDTH - 1
#define DEFENDER_Y_START 2
#define ATTACKER_X_START 1
#define INTIAL_LIVES SCREEN_Y_MAX

typedef enum {SINGLE, PLAYER_ONE, PLAYER_TWO} player_t;

typedef struct {
    int8_t x;
    int8_t y;
} common_pos_t;

typedef struct {
    common_pos_t pos;
    bool active;
} common_obj_t;

typedef struct {
    player_t player_mode;
    common_pos_t screen_size;
    int8_t lives;
    uint16_t score;

    common_pos_t defender;
    common_obj_t arrow;

    common_obj_t enemy1;
    common_obj_t enemy2;
    common_obj_t enemy3;

    int8_t attacker_input;
    bool ready_to_send;
    common_pos_t attacker;

} common_game_values_t;

static common_game_values_t game_values;
static common_game_values_t* const game = &(game_values);


void cat_def_init (player_t player);

void cat_def_detect_collision (void);

static void cat_def_draw_pixel (common_pos_t pos);

static void cat_def_render_defender (void);

static void cat_def_render_attacker (void);

void cat_def_render (void);


static void defender_move (int8_t direction);

void defender_move_up (void);

void defender_move_down (void);

void defender_shoot (void);


static void attacker_move (int8_t direction);

void attacker_move_left (void);

void attacker_move_right (void);

void attacker_select_spawn (void);


static void arrow_spawn (void);

static void arrow_reset (void);

static void arrow_advance_w_drop (void);

static void arrow_advance (void);

static void arrow_drop (void);

void arrow_update (void);


static void reset_enemy1 (void);

static void reset_enemy2 (void);

static void reset_enemy3 (void);

void attacker_move_enemy (void);

void attacker_spawn_enemy (void);


int8_t cat_def_get_lives (void);

static void cat_def_lose_life (void);

void cat_def_update_player_select (int8_t direction);

void cat_def_ir_send (void);

void cat_def_ir_recv (void);

void cat_def_screen_init (void);


#endif  /* CASTLE_DEFENDER_H  */
