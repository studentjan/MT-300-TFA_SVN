#include <CMSIS_DSP.h>

float snums[] = {0.5, 0.6, 0.7, 1.8, 0.8, 0.45, 0.43, -50.58, 0.42, 0.515, 0.505, 0.443, 0.5, 0.8, 0.395, 0.495};

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  float stddev;
  int i;
  
  arm_std_f32(snums, 16, &stddev);  /* Analyze all values in snums[] and compute the Standard Deviation
                                                * of the set, storing it in the (single float) referenced by &stddev.
                                                */
  for (i=0; i < 16; i++) {
    Serial.print("#"); Serial.print(i); Serial.print(": "); Serial.println(snums[i]);
  }
  Serial.print("Standard Deviation: "); Serial.println(stddev);
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(1000);
}
