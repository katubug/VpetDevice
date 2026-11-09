#pragma once

struct Selection {
    int index = 0;   // which item is highlighted
    int count = 0;   // how many items in the list
};

void selection_step(Selection& selection, int direction);