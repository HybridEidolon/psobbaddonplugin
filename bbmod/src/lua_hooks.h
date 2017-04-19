#pragma once

#include "sol.hpp"

// Called before Direct3D8 Present function is called.
void psoluah_Present(void);
// Calls all the init callbacks and stores their return values into the addons list.
void psoluah_Init(void);

void psoluah_KeyPressed(int key_code);

void psoluah_KeyReleased(int key_code);
