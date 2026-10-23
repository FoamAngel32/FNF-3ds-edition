#include "general.h"
#include "states/play_state.h"
#include "states/title_state.h"
StateFunction func_state_init[TOTAL_STATE] = {
    init_TitleState,
    NULL,
    init_PlayState,
};

StateFunction func_state_update[TOTAL_STATE] = {
    update_TitleState,
    NULL,
    update_PlayState,
};
