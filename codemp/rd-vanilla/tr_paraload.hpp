#pragma once
#include "tr_local.h"

void R_ParallelInitialize();
void R_ParallelShutdown();

bool R_ParallelDownloadReady();
image_t * R_ParallelDownloadNetImage(char const * name);
void R_ReloadNetImages();
