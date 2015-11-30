#include <iostream>
#include <wiringPiI2C.h>
#include <cmath>
#include <unistd.h>

using namespace std;

#define REF_P_XL       0x08
#define REF_P_L        0x09
#define REF_P_H        0x0A

#define WHO_AM_I       0x0F

#define RES_CONF       0x10

#define CTRL_REG1      0x20
#define CTRL_REG2      0x21
#define CTRL_REG3      0x22
#define INTERRUPT_CFG  0x23
#define INT_SOURCE     0x24
#define THS_P_L        0x25
#define THS_P_H        0x26
#define STATUS_REG     0x27

#define PRESS_OUT_XL   0x28
#define PRESS_OUT_L    0x29
#define PRESS_OUT_H    0x2A

#define TEMP_OUT_L     0x2B
#define TEMP_OUT_H     0x2C

#define AMP_CTRL       0x30

#define DELTA_PRESS_XL 0x3C
#define DELTA_PRESS_L  0x3D
#define DELTA_PRESS_H  0x3E

#define LPS331         0b1011101

int main()
{
   int lps331 = wiringPiI2CSetup(LPS331);
   if(lps331 == -1)
   {
      cout << "Error opening LPS331." << endl;
      return lps331;
   }

   wiringPiI2CWriteReg8(lps331, CTRL_REG1, 0b11100000);

   long pressure_XL, pressure_L, pressure_H;
   double pressure_b;
   long double pressure, altitude;

   while(1)
   {
      pressure_XL = wiringPiI2CReadReg8(lps331, PRESS_OUT_XL);
      pressure_L = wiringPiI2CReadReg8(lps331, PRESS_OUT_L);
      pressure_H = wiringPiI2CReadReg8(lps331, PRESS_OUT_H);

      pressure_b = pressure_H << 16 | pressure_L << 8 | pressure_XL;
      //cout << "Bytes: " << pressure_b << endl;

      pressure = pressure_b / 4096;
      //cout << "mbar: " << pressure << endl;
      altitude = 44308.7 * (1 - pow(pressure/1013.25,0.190284) )
      cout << altitude << endl;
   }

   return lps331;
}
