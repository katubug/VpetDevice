
#include "game/selection.h"

void selection_step(Selection& selection, int direction){
    if (selection.count <= 0) return;   //count instead of index because you're checking if there's anything there to select, not checking what is selected.
    selection.index = (selection.index + direction + selection.count) % selection.count;
}   //the second "selection.count" adds a full lap of the menu for safety to keep the number positive despite c++ quirks. the third one after the % is dividing the previous number (in parentheses) by the selection.count in order to wrap the selection around by the amount the % (aka remainder) hands back to us.