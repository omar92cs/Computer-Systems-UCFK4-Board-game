/** @file   defender.h
    @author itb14, aaa113, ran86
    @date   12 October 2017
    @brief  Interface for castle defender game.
**/

// Standard imports
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "castle_defender.h"

/* Intialises the game to it's start state. */
void cat_def_init (player_t player)
{
    // Set game screen size and player mode
    game->player_mode = player;
    game->screen_size.x = SCREEN_X_MAX;
    game->screen_size.y = SCREEN_Y_MAX;

    // Set score to zero and refill lives
    game->lives = INTIAL_LIVES;
    game->score = 0;

    // Set defender start position and intialise arrow
    game->defender.x = 0;
    game->defender.y = DEFENDER_Y_START;
    game->arrow.pos.x = 0;
    game->arrow.pos.y = 0;
    game->arrow.active = false;

    // Intialise enemys
    game->enemy1.pos.x = 0;
    game->enemy1.pos.y = 0;
    game->enemy1.active = false;

    game->enemy2.pos.x = 0;
    game->enemy2.pos.y = 0;
    game->enemy2.active = false;

    game->enemy3.pos.x = 0;
    game->enemy3.pos.y = 0;
    game->enemy3.active = false;

    // Multiplayer stuff
    game->attacker_input = 0;
    game->ready_to_send = false;
    game->attacker.x = ATTACKER_X_START;
    game->attacker.y = 0;
}

/* Compares the position of the arrow to each enemy, if they are
 * the same they are both reset and the score is incremented
 * otherwise there nothing happens.
 * */
void cat_def_detect_collision (void)
{
    if (game->arrow.active && game->player_mode != PLAYER_TWO) {
        if (game->arrow.pos.x == game->enemy1.pos.x
                && game->arrow.pos.y == game->enemy1.pos.y
                && game->enemy1.active) {
            arrow_reset ();
            reset_enemy1 ();
            game->score++;
        }

        if (game->arrow.pos.x == game->enemy2.pos.x
                && game->arrow.pos.y == game->enemy2.pos.y
                && game->enemy2.active) {
            arrow_reset ();
            reset_enemy2 ();
            game->score++;
        }

        if (game->arrow.pos.x == game->enemy3.pos.x
                && game->arrow.pos.y == game->enemy3.pos.y
                && game->enemy3.active) {
            arrow_reset ();
            reset_enemy3 ();
            game->score++;
        }
    }
}

/* Takes the a game specific co-ordinate and changes it to a tinygl
 * point and uses the api to add it to the current screen.
 * */
static void cat_def_draw_pixel (common_pos_t pos)
{
    tinygl_point_t point = tinygl_point (pos.y, pos.x);
    tinygl_draw_point (point, 1);
}

/* Renders the defender specific game sprites. */
static void cat_def_render_defender (void)
{
    static uint8_t count = 0;

    tinygl_clear ();

    cat_def_draw_pixel(game->defender);

    if(game->arrow.active) {
        cat_def_draw_pixel(game->arrow.pos);
    }

    if(count % FLASH_DELAY == 0) {
        //flashes attackers
        if(game->enemy1.active) {
            cat_def_draw_pixel(game->enemy1.pos);
        }
        if(game->enemy2.active) {
            cat_def_draw_pixel(game->enemy2.pos);
        }
        if(game->enemy3.active) {
            cat_def_draw_pixel(game->enemy3.pos);
        }

        //flashes attackers that have passed
        static uint8_t i;
        for (i = SCREEN_Y_MAX; i > game->lives; i--) {
            tinygl_point_t point = {i, 0};
            tinygl_draw_point (point, 1);
        }
    }

    tinygl_update ();
    count++;
}

/* Renders the attacker specific game sprites. */
static void cat_def_render_attacker (void)
{
    static uint8_t count = 0;

    tinygl_clear ();

    if(count % 8 == 0) {
        cat_def_draw_pixel(game->attacker);
    }

    tinygl_update ();
    count++;
}

/* Decides whether to render the attacker or defender. */
void cat_def_render (void)
{
    if (game->player_mode != PLAYER_TWO) {
        cat_def_render_defender ();
    } else {
        cat_def_render_attacker ();
    }
}

/* Increments the defenders y position with the upper limit being the
 * top of the screen and how many enemies occupys the bottom of the
 * screen.
 * */
static void defender_move (int8_t direction)
{

    if (game->defender.y >= game->lives && direction > 0) {
        game->defender.y = game->lives;
    } else if (game->defender.y <= 0 && direction < 0) {
        game->defender.y = 0;
    } else {
        game->defender.y += direction;
    }

}

/* Moves the defender sprtite up the screen. */
void defender_move_up (void)
{
    defender_move (1);
}

/* Moves the defender sprtite down the screen. */
void defender_move_down (void)
{
    defender_move (-1);
}

/* Creates an arrow. */
void defender_shoot (void)
{
    arrow_spawn ();
}

/* Increments the defenders x position with the left limit being the
 * left of the screen and the second to last column.
 * */
static void attacker_move (int8_t direction)
{

    if (game->attacker.x >= SCREEN_X_MAX && direction > 0) {
        game->attacker.x = SCREEN_X_MAX;
    } else if (game->attacker.x <= 1 && direction < 0) {
        game->attacker.x = 1;
    } else {
        game->attacker.x += direction;
    }

}

/* Moves the attacker sprite left. */
void attacker_move_left (void)
{
    attacker_move (1);
}

/* Moves the attacker sprite right. */
void attacker_move_right (void)
{
    attacker_move (-1);
}

/* Sets the ir output to the current attacker x position. */
void attacker_select_spawn (void)
{
    game->attacker_input = game->attacker.x;
    game->ready_to_send = true;
}

/* If the arrow isn't active spawns an arrow at the current defender
 * y position. This prevents ore than one arrow being on screen.
 */
static void arrow_spawn (void)
{
    if(!game->arrow.active) {
        game->arrow.pos.x = 0;
        game->arrow.pos.y = game->defender.y;
        game->arrow.active = true;
    }
}

/* Resets the arrow so it's inactive. */
static void arrow_reset (void)
{
    game->arrow.pos.x = 0;
    game->arrow.pos.y = 0;
    game->arrow.active = false;
}

/* Moves the arrow forward one and down by one */
static void arrow_advance_w_drop (void)
{
    game->arrow.pos.x++;
    game->arrow.pos.y++;
}

/* Moves the arrow forward one */
static void arrow_advance (void)
{
    game->arrow.pos.x++;
}

/* Moves the arrow down by one */
static void arrow_drop (void)
{
    game->arrow.pos.y++;
}

/* Updates the arrows x and y position depending on it's current x position
 * causing the arrow to fall over time.
 */
void arrow_update (void)
{
    if (game->arrow.active && game->player_mode != PLAYER_TWO) {
        if(game->arrow.pos.x <= 1) {
            arrow_advance ();
        } else if(game->arrow.pos.x == 2) {
            arrow_advance ();
        } else if(game->arrow.pos.x == 3) {
            arrow_advance_w_drop ();
        } else if(game->arrow.pos.x == 4) {
            arrow_advance ();
        } else if(game->arrow.pos.x == 5) {
            arrow_advance_w_drop ();
        } else if(game->arrow.pos.x == 6) {
            arrow_reset ();
        }

        if(game->arrow.pos.y > SCREEN_Y_MAX) {
            arrow_reset ();
        }
    }
}

/* Resets enemy1 so it's inactive. */
static void reset_enemy1 (void)
{
    game->enemy1.pos.x = 0;
    game->enemy1.pos.y = 0;
    game->enemy1.active = false;
}

/* Resets enemy2 so it's inactive. */
static void reset_enemy2 (void)
{
    game->enemy2.pos.x = 0;
    game->enemy2.pos.y = 0;
    game->enemy2.active = false;
}

/* Resets enemy3 so it's inactive. */
static void reset_enemy3 (void)
{
    game->enemy3.pos.x = 0;
    game->enemy3.pos.y = 0;
    game->enemy3.active = false;
}

/* Decrements all on screen enemies y position by 1.
 * If an enemy reaches the botom of the screen the
 * palyer loses a life and the enemy is reset.
 */
void attacker_move_enemy (void)
{
    if (game->player_mode != PLAYER_TWO) {

        if(game->enemy1.active) {
            game->enemy1.pos.y++;
            if(game->enemy1.pos.y > 4) {
                reset_enemy1 ();
                cat_def_lose_life ();
            }
        }

        if(game->enemy2.active) {
            game->enemy2.pos.y++;
            if(game->enemy2.pos.y > 4) {
                reset_enemy2 ();
                cat_def_lose_life ();
            }
        }

        if(game->enemy3.active) {
            game->enemy3.pos.y++;
            if(game->enemy3.pos.y > 4) {
                reset_enemy3 ();
                cat_def_lose_life ();
            }
        }

    }
}

/* Spawns enemies off screen with a one row gap between them.
 * The y co-ordinate of the spawn is a random number between
 * 1 and 6 if playing in single player or a position defined
 * by the attacker in multiplayer.
 */
void attacker_spawn_enemy (void)
{
    if (game->player_mode != PLAYER_TWO) {

        static int8_t last_spawn = 1;

        int8_t spawn_point;
        if (game->player_mode == SINGLE) {
            spawn_point = (rand () % game->screen_size.x) + 1;
        } else if (game->player_mode == PLAYER_ONE) {
            if (game->attacker_input != 0) {
                spawn_point = game->attacker_input;
                game->attacker_input = 0;
            } else {
                return;
            }
        } else {
            return;
        }

        if(last_spawn == 1 && !game->enemy1.active) {
            game->enemy1.pos.x = spawn_point;
            game->enemy1.pos.y = -1;
            game->enemy1.active = true;
        } else if(last_spawn == 3 && !game->enemy2.active) {
            game->enemy2.pos.x = spawn_point;
            game->enemy2.pos.y = -1;
            game->enemy2.active = true;
        } else if(last_spawn == 5 && !game->enemy3.active) {
            game->enemy3.pos.x = spawn_point;
            game->enemy3.pos.y = -1;
            game->enemy3.active = true;
        }

        if(last_spawn == NUM_SPAWN_ROW) {
            last_spawn = 1;
        } else {
            last_spawn++;
        }

    }
}

/* Allows other modules to access current games number of lives */
int8_t cat_def_get_lives (void)
{
    return game->lives;
}

/* Decrements the current players lives. If playing multiplayer
 * the ready to send flag is set to indicate that the defender
 * should transfer the number of lives to the attacker.
 */
static void cat_def_lose_life (void)
{
    game->lives--;
    if (game->defender.y >= game->lives) {
        defender_move_up ();
    }
    game->ready_to_send = true;
}

/* Iterates through the player modes based on the direction given and
 * overflows to the beginng/end when the last/first mode is reached.
 * The displayed text is than updated according to the current mode.
 * */
void cat_def_update_player_select (int8_t direction)
{

    if (game->player_mode == PLAYER_TWO && direction > 0) {
        game->player_mode = SINGLE;
    } else if (game->player_mode == SINGLE && direction < 0) {
        game->player_mode = PLAYER_TWO;
    } else {
        game->player_mode += direction;
    }

    tinygl_clear ();
    switch (game->player_mode) {
    case SINGLE:
        tinygl_text ("SINGLE PLAYER ");
        break;
    case PLAYER_ONE:
        tinygl_text ("DEFENDER ");
        break;
    case PLAYER_TWO:
        tinygl_text ("ATTACKER ");
        break;
    }
}

/* Sends the spawn point of an enemy if playing as the attacker or sends
 * the current number of lives if the defender.
 */
void cat_def_ir_send (void)
{
    if (game->player_mode == PLAYER_TWO && game->ready_to_send == true) {
        ir_uart_putc (game->attacker_input);
        game->ready_to_send = false;
    }
    if (game->player_mode == PLAYER_ONE && game->ready_to_send == true) {
        ir_uart_putc (game->lives);
        game->ready_to_send = false;
    }
}

/* Receives the current number of lives if the attacker or receives
 * the spawn point of an enemy if playing as the defender.
 */
void cat_def_ir_recv (void)
{
    if (game->player_mode == PLAYER_TWO) {
        if (!ir_uart_read_ready_p ()) {
            return;
        }
        game->lives = ir_uart_getc ();
    } else if (game->player_mode == PLAYER_ONE) {
        if (!ir_uart_read_ready_p ()) {
            return;
        }
        game->attacker_input = ir_uart_getc ();
    }
}

/* Intialises tinygl to the necessary values for castle defender */
void cat_def_screen_init (void)
{
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (CAT_DEF_SCROLL_CHAR_PER_10_SEC);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_init (CAT_DEF_DISPLAY_REFRESH_FREQ);
}
