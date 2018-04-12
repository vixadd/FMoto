
/*
 * Si4713 driver support for the fm_xmitter modification.
 * allows for driver support from the code, and allows for
 * fm transmission using the required commands through the i2c
 * bus.
 *
 *             Author: David Kroell
 *            Version: 1.2.0
 */


#ifndef __INCLUDE_SI4713_H
#define __INCLUDE_SI4713_H

#include <nuttx/util.h>

#define SI4713_MAX_I2C_SIZE   252

/****************** Registers *******************/
#define SI4713_POWER_UP           0x01      // Power up device and mode selection.
#define SI4713_GET_REV            0x10      // Returns revision information in the device
#define SI4713_POWER_DOWN         0x11      // Power down device
#define SI4713_SET_PROPERTY       0x12      // Sets the value of a property
#define SI4713_GET_PROPERTY       0x13      // Retrieves a property value
#define SI4713_GET_INT_STATUS     0x14      // Read interrupt status
#define SI4713_PATCH_ARGS         0x15      // Reserved command used for patch file downloads
#define SI4713_PATCH_DATA         0x16      // Reserved command used for patch file downloads
#define SI4713_TX_TUNE_FREQ       0x30      // Tunes to given transmit frequency
#define SI4713_TX_TUNE_POWER      0x31      // Sets the output power level and tunes the antenna capacitor
#define SI4713_TX_TUNE_MEASURE    0x32      // Measure the recieved noise level at the specified frequency.
#define SI4713_TX_TUNE_STATUS     0x33      // Queries the status of the previously sent TX Tune Freq,
                                            //                 TX Tune Power, and TX Tune Measure Cmd.
#define SI4713_TX_ASQ_STATUS      0x34      // Queries the TX status and input audio signal metrics.
#define SI4713_TX_RDS_BUFF        0x35      // Queries the status of the RDS Group Buffer and loads new data into the buffer.
#define SI4713_TX_RDS_PS          0x36      // Set up default PS strings
#define SI4713_GPIO_CTL           0x80      // Configures the GPO1, 2, and 3 as output for Hi-Z
#define SI4713_GPIO_SET           0x81      // Sets GPO1, 2, and 3 output level (low or high)

//
// This function will power up the board.
//
void si4713_powerup();

//
// This function will power down the board. Probably wont be used.
//
void si4713_powerdown();

//
// This function will automatically tune the transmitter to 101.1 MHz
// Note that this band is open where I am located in Newport News.
// 
void si4713_tune_freq();


/*
 * si4713_device - private data
 */
struct si4713_device {
  /* v4l2_subdev and i2c reference (v4l2_subdev priv data) */
  struct v4l2_subdev sd;

  /* private data structures */
  struct mutex mutex;

  struct completion work;
  struct rds_info rds_info;
  struct limiter_info limiter_info;
  struct pilot_info pilot_info;
  struct acomp_info acomp_info;
  struct regulator_bulk_data supplies[2];

  int gpio_reset;
  u32 frequency;
  u32 preemphasis;
  u32 mute;
  u32 power_level;
  u32 power_state;
  u32 antenna_capacitor;
  u32 stereo;
  u32 tune_rnl;

};


#endif /* Include si4713.h */
