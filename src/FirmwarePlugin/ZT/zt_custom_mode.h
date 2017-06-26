/****************************************************************************
 *
 *   Copyright (c) 2013-2016 ZT Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name ZT nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file px4_custom_mode.h
 * ZT custom flight modes
 *
 */

#ifndef ZT_CUSTOM_MODE_H_
#define ZT_CUSTOM_MODE_H_

#include <stdint.h>

enum ZT_CUSTOM_MAIN_MODE {
    ZT_CUSTOM_MAIN_MODE_MANUAL = 1,
    ZT_CUSTOM_MAIN_MODE_STABILIZED,
    ZT_CUSTOM_MAIN_MODE_RATTITUDE,
    ZT_CUSTOM_MAIN_MODE_ALTCTL,
    ZT_CUSTOM_MAIN_MODE_POSCTL,
    ZT_CUSTOM_MAIN_MODE_SIMPLE,
    ZT_CUSTOM_SUB_MODE_AUTO_READY,
    ZT_CUSTOM_SUB_MODE_AUTO_TAKEOFF,
    ZT_CUSTOM_SUB_MODE_AUTO_LOITER,
    ZT_CUSTOM_SUB_MODE_AUTO_MISSION,
    ZT_CUSTOM_SUB_MODE_AUTO_RTL,
    ZT_CUSTOM_SUB_MODE_AUTO_LAND,
    ZT_CUSTOM_SUB_MODE_AUTO_RTGS,
    ZT_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET,
    ZT_CUSTOM_MAIN_MODE_ACRO
};

#endif /* ZT_CUSTOM_MODE_H_ */
