
/*

Copyright (c) 2017-2018, Feral Interactive
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of Feral Interactive nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

 */

#define _GNU_SOURCE

#include "gamemode.h"
#include "helpers.h"
#include "logging.h"

#include "daemon_config.h"
#include "gpu-query.h"

// TODO
// Gather GPU type and information
// Allow override of vendor and device
// Apply Nvidia GPU settings (CoolBits will be needed)
// Apply AMD GPU settings (Will need user changing pwm1_enable)
// Intel?
// Provide documentation on optimisations

/**
 * Attempts to identify the current in use GPU information
 */
int game_mode_initialise_gpu(GameModeConfig *config, GameModeGPUInfo **info)
{
	int status = 0;

	/* Verify input, this is programmer error */
	if (!info || *info)
		FATAL_ERROR("Invalid GameModeGPUInfo passed to %s", __func__);

	/* Early out if we have this feature turned off */
	long apply = 0;
	config_get_apply_gpu_optimisations(config, &apply);
	if (apply == 0)
		return 0;

	/* Create the context */
	GameModeGPUInfo *new_info = malloc(sizeof(GameModeGPUInfo));
	memset(new_info, 0, sizeof(GameModeGPUInfo));

	/* Get the config parameters */
	config_get_gpu_vendor(config, &new_info->vendor);
	config_get_gpu_device(config, &new_info->device);

	/* TODO: Detect the GPU vendor and device automatically when these aren't set */

	/* verify device ID */
	if (new_info->device == -1) {
		LOG_ERROR("Invalid gpu_device value set in configuration, will not appli optimisations!\n");
		free(new_info);
		return -1;
	}

	/* verify GPU vendor */
	if (new_info->vendor != Vendor_NVIDIA && new_info->vendor != Vendor_AMD &&
	    new_info->vendor != Vendor_Intel) {
		LOG_ERROR("Invalid gpu_vendor value set in configuration, will not apply optimisations!\n");
		LOG_ERROR("Possible values are: 0x%04x (NVIDIA) 0x%04x (AMD) 0x%04x (Intel)\n",
		          Vendor_NVIDIA,
		          Vendor_AMD,
		          Vendor_Intel);
		free(new_info);
		return -1;
	}

	/* Load the config based on GPU */
	switch (new_info->vendor) {
	case Vendor_NVIDIA:
		config_get_nv_core_clock_mhz_offset(config, &new_info->core);
		config_get_nv_mem_clock_mhz_offset(config, &new_info->mem);
		break;
	case Vendor_AMD:
		config_get_amd_core_clock_percentage(config, &new_info->core);
		config_get_amd_mem_clock_percentage(config, &new_info->mem);
		break;
	default:
		break;
	}
	/* TODO : Sanity check these values */

	/* Give back the new gpu info */
	*info = new_info;

	return status;
}

/* Simply used to free the GPU info object */
void game_mode_free_gpu(GameModeGPUInfo **info)
{
	/* Simply free the object */
	free(*info);
	*info = NULL;
}

/**
 * Applies GPU optimisations when gamemode is active and removes them after
 */
int game_mode_apply_gpu(const GameModeGPUInfo *info, bool apply)
{
	// Null info means don't apply anything
	if (!info)
		return 0;

	/* TODO Call gpuclockctl set */

	return 0;
}
