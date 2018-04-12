/*
 * Implementation of si4713 driver for the board.
 * 
 *       Author: David Kroell
 *      Version: 1.2.0
 */

#include <ctype.h>
#include <errno.h>
#include <debug.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <arch/byteorder.h>
#include <nuttx/si4713.h>
#include <nuttx/i2c.h>

//
// Used to send commands to the module
//
static int si4713_send_command(struct si4713_device *sdev, const u8 command,
			       const u8 args[], const int argn, u8 response[],
			       const int respn, const int usecs);



void si4713_powerup() {
  
}
