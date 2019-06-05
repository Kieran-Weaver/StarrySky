#ifndef IMGUI_HELPER_H
#define IMGUI_HELPER_H
#include "../imgui/imgui.h"
void SliderTLeftLabel(const char* label, int* tpointer, int lower_bound, int upper_bound);
void SliderTLeftLabel(const char* label, float* tpointer, float lower_bound, float upper_bound);
#endif
