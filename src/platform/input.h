#pragma once

enum class Verb {
    select,
    confirm,
    cancel
};

bool input_check_pressed(Verb verb);
bool input_check(Verb verb);
bool input_check_released(Verb verb);