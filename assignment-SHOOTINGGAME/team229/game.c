/** @file   defender.h
    @author itb14, aaa113, ran86
    @date   12 October 2017
    @brief  Interface for castle defender game.
**/

// Standard imports
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Driver and Util imports
#include "system.h"
#include "tinygl.h"
#include "task.h"
#include "navswitch.h"
#include "../fonts/font3x5_1.h"

// Game imports
#include "common.h"
#include "castle_defender.h"

// Define polling rates in Hz.
enum {DISPLAY_TASK_RATE = 1000};
enum {BUTTON_TASK_RATE = 20};
enum {ARROW_UPDATE_TASK_RATE = 10};
enum {ATTACKER_TASK_RATE = 2};
enum {COLLISION_TASK_RATE = 20};
enum {GAME_TASK_RATE = 500};
enum {IR_TASK_RATE = 30};

// Various game and menu states used to determine mode of operation
typedef enum {INIT_GAME_SELECT, GAME_SELECT, PLAYING_RPSLV,
              PLAYER_SELECT_CAT_DEF, INIT_CAT_DEF, PLAYING_CAT_DEF,
              END
             } state_t;
static state_t state;

// All available games
typedef enum {CAT_DEF, RPSLV} game_t;
static game_t selected_game;
#define FIRST_GAME CAT_DEF
#define LAST_GAME RPSLV
#define UP 1
#define DOWN -1


/* Calls the castle defender collison function at a predefined rate. */
static void collision_task (__unused__ void *data)
{
    if(state == PLAYING_CAT_DEF) {
        cat_def_detect_collision ();
    }
}

/* Updates the castle defenders enemies at a predefined rate. */
static void attacker_task (__unused__ void *data)
{
    if(state == PLAYING_CAT_DEF) {
        attacker_move_enemy ();
        attacker_spawn_enemy ();
    }
}

/* Calls the castle defender arrow function at a predefined rate. */
static void arrow_update_task (__unused__ void *data)
{

    if(state == PLAYING_CAT_DEF) {
        arrow_update ();
    }

}

/* Calls the castle defender ir send function at a predefined rate. */
static void ir_send_task (__unused__ void *data)
{
    if (state == PLAYING_CAT_DEF) {
        cat_def_ir_send ();
    }
}

/* Calls the castle defender ir receive function at a predefined rate. */
static void ir_recv_task (__unused__ void *data)
{
    if (state == PLAYING_CAT_DEF) {
        cat_def_ir_recv ();
    }
}

/* Updates the display with respect to the current state. */
static void display_task (__unused__ void *data)
{
    if (state == PLAYING_CAT_DEF) {
        cat_def_render ();
    } else if (state == GAME_SELECT) {
        tinygl_update();
    } else if (state == PLAYER_SELECT_CAT_DEF) {
        tinygl_update();
    } else if (state == END) {
        tinygl_update();
    }
}

/* Iterates through the game options based on the direction given and
 * overflows to the beginng/end when the last/first game is reached.
 * The displayed text is than updated according to the current game.
 * */
void update_game_select (int8_t direction)
{

    if (selected_game == LAST_GAME && direction > 0) {
        selected_game = FIRST_GAME;
    } else if (selected_game == FIRST_GAME && direction < 0) {
        selected_game = LAST_GAME;
    } else {
        selected_game += direction;
    }

    tinygl_clear ();
    switch (selected_game) {
    case CAT_DEF:
        tinygl_text ("CASTLE DEFENDER ");
        break;
    case RPSLV:
        tinygl_text ("ROCK PAPER SCISSOR LIZARD VULKAN ");
        break;
    }
}

/* Depending on the current state, takes a nav switch event and perfroms an
 * action. Some actions including changing the current state.
 * */
static void navswitch_task (__unused__ void *data)
{
    static bool init = 0;

    if (!init) {
        navswitch_init ();
        init = 1;
    }

    navswitch_update ();

    if (state == PLAYING_CAT_DEF && game->player_mode != PLAYER_TWO) {
        if (navswitch_push_event_p (NAVSWITCH_EAST)) {
            defender_move_up ();
        }
        if (navswitch_push_event_p (NAVSWITCH_WEST)) {
            defender_move_down ();
        }
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            defender_shoot ();
        }
    } else if (state == PLAYING_CAT_DEF && game->player_mode == PLAYER_TWO) {
        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            attacker_move_right ();
        }
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            attacker_move_left ();
        }
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            attacker_select_spawn ();
        }
    } else if (state == PLAYER_SELECT_CAT_DEF) {
        if (navswitch_push_event_p (NAVSWITCH_EAST)) {
            cat_def_update_player_select (UP);
        }
        if (navswitch_push_event_p (NAVSWITCH_WEST)) {
            cat_def_update_player_select (DOWN);
        }
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            state = PLAYING_CAT_DEF;
        }
    } else if (state == GAME_SELECT) {
        if (navswitch_push_event_p (NAVSWITCH_EAST)) {
            update_game_select (UP);
        }
        if (navswitch_push_event_p (NAVSWITCH_WEST)) {
            update_game_select (DOWN);
        }
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            switch (selected_game) {
            case CAT_DEF:
                state = INIT_CAT_DEF;
                break;
            case RPSLV:
                state = PLAYING_RPSLV;
                break;
            }
        }
    } else if (state == END) {
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            state = INIT_GAME_SELECT;
        }
    }
}


/* Used to intialise certian things and used to change state when certain
 * conditions are meet.
 * */
static void game_task (__unused__ void *data)
{
    if (state == PLAYING_CAT_DEF) {
        if(cat_def_get_lives () < 0) {
            //Player died
            tinygl_clear ();
            tinygl_text ("GAME OVER ");
            state = END;
        }
    } else if (state == INIT_CAT_DEF) {
        cat_def_init (SINGLE);
        cat_def_screen_init ();
        //initialises player select
        tinygl_clear ();
        tinygl_text ("SINGLE PLAYER ");
        state = PLAYER_SELECT_CAT_DEF;
    } else if (state == INIT_GAME_SELECT) {
        tinygl_clear ();
        tinygl_text ("CASTLE DEFENDER ");
        selected_game = CAT_DEF;
        state = GAME_SELECT;
    } else if (state == PLAYING_RPSLV) {
        //RPSLV is still in beta
        state = INIT_GAME_SELECT;
    }
}

int main (void)
{

    task_t tasks[] = {
        {
            .func = game_task, .period = TASK_RATE / GAME_TASK_RATE,
            .data = 0
        },
        {
            .func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE,
            .data = 0
        },
        {
            .func = navswitch_task, .period = TASK_RATE / BUTTON_TASK_RATE,
            .data = 0
        },
        {
            .func = arrow_update_task, .period = TASK_RATE / ARROW_UPDATE_TASK_RATE,
            .data = 0
        },
        {
            .func = attacker_task, .period = TASK_RATE / ATTACKER_TASK_RATE,
            .data = 0
        },
        {
            .func = collision_task, .period = TASK_RATE / COLLISION_TASK_RATE,
            .data = 0
        },
        {
            .func = ir_recv_task, .period = TASK_RATE / IR_TASK_RATE,
            .data = 0
        },
        {
            .func = ir_send_task, .period = TASK_RATE / IR_TASK_RATE,
            .data = 0
        },
    };

    system_init ();
    ir_uart_init ();
    cat_def_screen_init ();
    cat_def_init (SINGLE);
    state = INIT_GAME_SELECT;

    task_schedule (tasks, ARRAY_SIZE (tasks));
    return 0;
}
