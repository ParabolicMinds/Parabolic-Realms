#pragma once
#include "tr_local.h"

void R_ParallelInitialize();
void R_ParallelShutdown();

void R_ParallelDownloadNetTexture(char const * name, shader_t * sh);
bool R_ParallelDownloadReady();
