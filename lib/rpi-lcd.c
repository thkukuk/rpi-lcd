/* Copyright (C) 2019  Thorsten Kukuk

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License version 2.1 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "rpi-lcd.h"

#define En 0b00000100 /* Enable bit */
#define Rw 0b00000010 /* Read/Write bit */
#define Rs 0b00000001 /* Register select bit */

#define E_PULSE 500 /* microseconds */
#define E_DELAY 500 /* microseconds */

static void
i2c_write_byte (int device, uint8_t data)
{
  unsigned char byte[1];

  byte[0] = data;

  write (device, byte, sizeof(byte));
  /* Wait 1msec, needed by display to catch commands  */
  /* usleep (1000); */
}

static void
i2c_strobe (int device, uint8_t data, int backlight)
{
  i2c_write_byte (device, data);
  usleep (E_DELAY);
  i2c_write_byte (device, data | En | backlight);
  usleep (E_PULSE);
  i2c_write_byte (device, (data & ~En) | backlight);
  usleep (E_DELAY);
}

static void
i2c_write_mode (int device, uint8_t data, int mode)
{
  i2c_strobe (device, mode | (data & 0xF0), LCD_BACKLIGHT);
  i2c_strobe (device, mode | ((data << 4) & 0xF0), LCD_BACKLIGHT);
}

static void
i2c_write (int device, uint8_t data)
{
  i2c_write_mode (device, data, 0);
}

/* Public functions */

void
lcdDisplayOn(int device)
{
  i2c_write (device, LCD_DISPLAYON);
}

void
lcdDisplayOff(int device)
{
  i2c_write (device, LCD_DISPLAYOFF);
}

void
lcdClear (int device)
{
  i2c_write (device, LCD_CLEARDISPLAY);
  /* i2c_write (device, LCD_RETURNHOME); */
}

int
lcdSetup (uint8_t address, uint8_t busnum)
{
  char *devname;
  int device;
  int32_t res;
  unsigned long funcs;

  asprintf (&devname, "/dev/i2c-%i", busnum);
  if (devname == NULL)
    return -1;

  if ((device = open (devname, O_RDWR)) < 0)
    {
      perror ("open() failed");
      free (devname);
      return -1;
    }
  free (devname);

  /* Query if i2c funcs are available */
  if (ioctl (device, I2C_FUNCS, &funcs) < 0)
    {
      perror ("ioctl() I2C_FUNCS failed");
      close (device);
      return -1;
    }

  if (ioctl (device, I2C_SLAVE, address) < 0)
    {
      perror ("i2c set address failed");
      close (device);
      return -1;
    }

  i2c_write (device, 0x33); /* 110011 Initialise */
  i2c_write (device, 0x32); /* 110010 Initialise */
  i2c_write (device, LCD_ENTRYMODESET | LCD_ENTRYLEFT);
  lcdDisplayOn (device); /* Display On, Cursor Off, Blink Off */
  i2c_write (device,
	     LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
  lcdClear (device);
  usleep (E_DELAY);

  return device;
}

void
lcdClose(int device)
{
  close (device);
}

void
lcdWriteString (int device, int line, char *str)
{
  switch (line)
    {
    case 0:
      /* Continue from last position */
      break;
    case 1:
      i2c_write (device, 0x80);
      break;
    case 2:
      i2c_write (device, 0xC0);
      break;
    case 3:
      i2c_write (device, 0x94);
      break;
    case 4:
      i2c_write (device, 0xD4);
      break;
  default:
    fprintf (stderr, "ERROR: line out of range: %i\n", line);
    exit (1);
  }

  int i = 0;
  while (str[i])
    {
      if (str[i] == '\n')
	i2c_write (device, 0xC0);
      else
	i2c_write_mode (device, str[i], Rs);
      i++;
    }
}
