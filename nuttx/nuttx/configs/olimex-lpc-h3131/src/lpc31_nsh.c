/****************************************************************************
 * configs/olimex-lpc-h3131/src/lpc31_nsh.c
 *
 *   Copyright (C) 2013 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
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
 * 3. Neither the name NuttX nor the names of its contributors may be
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <stdio.h>
#include <debug.h>
#include <errno.h>

#ifdef CONFIG_SYSTEM_USBMONITOR
#  include <apps/usbmonitor.h>
#endif

#include "lpc31_internal.h"

#include "lpc_h3131.h"

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/
/* Configuration ************************************************************/

/* PORT and SLOT number probably depend on the board configuration */

#ifdef HAVE_MMCSD
#  if defined(CONFIG_NSH_MMCSDSLOTNO) && CONFIG_NSH_MMCSDSLOTNO != 0
#    error "Only one MMC/SD slot"
#    undef CONFIG_NSH_MMCSDSLOTNO
#  endif
#  ifndef CONFIG_NSH_MMCSDSLOTNO
#    define CONFIG_NSH_MMCSDSLOTNO 0
#  endif
#endif

/* Debug ********************************************************************/

#ifdef CONFIG_CPP_HAVE_VARARGS
#  ifdef CONFIG_DEBUG
#    define message(...) lowsyslog(__VA_ARGS__)
#  else
#    define message(...) printf(__VA_ARGS__)
#  endif
#else
#  ifdef CONFIG_DEBUG
#    define message lowsyslog
#  else
#    define message printf
#  endif
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nsh_archinitialize
 *
 * Description:
 *   Perform architecture specific initialization
 *
 ****************************************************************************/

int nsh_archinitialize(void)
{
#if defined(HAVE_MMCSD) || defined(HAVE_USBHOST)
  int ret;
#endif

#ifdef HAVE_MMCSD
  /* Create the SDIO-based MMC/SD device */

  message("nsh_archinitialize: Create the MMC/SD device\n");
  ret = lpc31_mmcsd_initialize(CONFIG_NSH_MMCSDSLOTNO);
  if (!sdio)
    {
      message("nsh_archinitialize: Failed to initialize SDIO slot %d\n",
              CONFIG_NSH_MMCSDSLOTNO, CONFIG_NSH_MMCSDMINOR);
      return -ENODEV;
    }
#endif

#ifdef HAVE_USBHOST
  /* Initialize USB host operation.  lpc31_usbhost_initialize() starts a thread
   * will monitor for USB connection and disconnection events.
   */

  message("nsh_archinitialize: Start USB host services\n");
  ret = lpc31_usbhost_initialize();
  if (ret != OK)
    {
      message("ERROR: Failed to start USB host services: %d\n", ret);
      return ret;
    }
#endif

#ifdef HAVE_USBMONITOR
  /* Start the USB Monitor */

  message("nsh_archinitialize: Start the USB monitor\n");
  ret = usbmonitor_start(0, NULL);
  if (ret != OK)
    {
      message("nsh_archinitialize: Failed to start USB monitor: %d\n", ret);
    }
#endif

  return OK;
}
