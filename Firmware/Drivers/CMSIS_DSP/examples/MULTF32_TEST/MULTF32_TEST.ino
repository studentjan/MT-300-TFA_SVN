#include <CMSIS_DSP.h>

float snums[8] = {0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0};
float mult[8] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0};

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  float dnums[8];
  int i;
  
  arm_mult_f32(&snums[0], &mult[0], &dnums[0], 8);  /* Multiply all of snums[] by mult[] respectively,
                                                     * storing in dnums[].
                                                     */

  /* End result should be 8 whole integers in real number representation,
   * from 1.00 to 8.00.
   */
  for (i=0; i < 8; i++) {
    Serial.print("#"); Serial.print(i); Serial.print(": "); Serial.println(dnums[i]);
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(1000);
}
