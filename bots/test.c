#include <stdio.h>
#include <string.h>

#include "bot_interface/bot.h"

#define NINJAHELPER_IMPLEMENT
#include "bot_interface/helpers.h"

static _Bool finished = 0;
static int action_choice = 0;

void NINJACLOWN_DLLEXPORT NINJACLOWN_CALLCONV bot_think()
{
	if (finished) {
		action_choice = action_choice + 1;
		if (action_choice >= 3) {
			action_choice = 0;
			ninja_turn_right();
		} else {
			ninja_move_forward();
		}

		return;
	}

	if (ninja_get_angle() < 2.8f) {
		ninja_turn_left();
	} else if (ninja_get_x() > 7.5) {
		ninja_move_forward();
	} else {
        ninja_activate_button();
		finished = 1;
	}
}
