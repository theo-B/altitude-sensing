#include <iostream>
#include <wiringPiI2C.h>
#include <cmath>
#include <unistd.h>

using namespace std;

#define SAMPLES_TO_AVG 200 //THIS HAS BEEN CHANGED FROM 180, CHECK IT WORKS
#define SAMPLE_RATE_US 70000

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
   double pressure_b, altitude_sum = 0;
   long double pressure, altitude[SAMPLES_TO_AVG], altitude_smooth[SAMPLES_TO_AVG];

   // Populate initial array
   for(int i=0; i<SAMPLES_TO_AVG; i++)
   {
      // READ BAROMETER
      pressure_XL = wiringPiI2CReadReg8(lps331, PRESS_OUT_XL);
      pressure_L = wiringPiI2CReadReg8(lps331, PRESS_OUT_L);
      pressure_H = wiringPiI2CReadReg8(lps331, PRESS_OUT_H);

      pressure_b = pressure_H << 16 | pressure_L << 8 | pressure_XL;
      //cout << "Bytes: " << pressure_b << endl;

      pressure = pressure_b / 4096;
      //cout << "mbar: " << pressure << endl;
      altitude[i] = 44308.7 * (1 - pow(pressure/1013.25, 0.190284));
   }

   //cout << "Beginning smoothing" << endl;
   usleep(500000);

   while(1)
   {
      for(int i=0; i<SAMPLES_TO_AVG; i++)
      {
         // READ BAROMETER
         pressure_XL = wiringPiI2CReadReg8(lps331, PRESS_OUT_XL);
         pressure_L = wiringPiI2CReadReg8(lps331, PRESS_OUT_L);
         pressure_H = wiringPiI2CReadReg8(lps331, PRESS_OUT_H);

         pressure_b = pressure_H << 16 | pressure_L << 8 | pressure_XL;
         //cout << "Bytes: " << pressure_b << endl;

         pressure = pressure_b / 4096;
         //cout << "mbar: " << pressure << endl;
         altitude[i] = 44308.7 * (1 - pow(pressure/1013.25, 0.190284));
         cout << altitude[i] << ", ";

         // Iterate through the full array, summing all the terms
         for(int c=0; c<SAMPLES_TO_AVG; c++)
         {
            altitude_sum += altitude[c];
         }
         altitude_smooth[i] = altitude_sum / SAMPLES_TO_AVG;
         cout << altitude_smooth[i] << endl;
         altitude_sum = 0;
         usleep(SAMPLE_RATE_US);
      }
   }

   return lps331;
}
