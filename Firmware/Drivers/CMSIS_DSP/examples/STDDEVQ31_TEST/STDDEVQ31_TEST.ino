#include <CMSIS_DSP.h>

float snums[] = {
  0.5,
  0.56,
  0.47,
  0.98,
  0.48,
  0.52533,
  0.513,
  0.508,
  0.515,
  0.498,
  0.495,
  0.425,
  0.422,
  0.479,
  0.480,
  0.478
};

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  q31_t intvals[16];
  q31_t stddev;
  float stddevF;
  int i;
  
  arm_float_to_q31(&snums[0], &intvals[0], 16);  // Copy floats to q31 representation
  for (i=0; i < 16; i++) {
    Serial.print("#"); Serial.print(i); Serial.print(": ");
    Serial.print(snums[i]); Serial.print(" ("); Serial.print(intvals[i]); Serial.println(")");
  }

  arm_std_q31(&intvals[0], 16, &stddev);  // Perform stddev computation in pure-integer math
  arm_q31_to_float(&stddev, &stddevF, 1); // Convert integer stddev to float approximation
  Serial.print("Standard Deviation (q31, q31-to-float): ");
    Serial.print(stddev); Serial.print(", "); Serial.println(stddevF);

  arm_std_f32(&snums[0], 16, &stddevF);   // Perform stddev computation in float math
  Serial.print("Standard Deviation (pure-float): "); Serial.println(stddevF);
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(1000);
}

/* Resulting output should be:
   #0: 0.50 (1073741824)
   #1: 0.56 (1202590848)
   #2: 0.47 (1009317312)
   #3: 0.98 (2104534016)
   #4: 0.48 (1030792128)
   #5: 0.53 (1128137600)
   #6: 0.51 (1101659136)
   #7: 0.51 (1090921728)
   #8: 0.51 (1105954048)
   #9: 0.50 (1069446848)
   #10: 0.50 (1063004416)
   #11: 0.43 (912680576)
   #12: 0.42 (906238080)
   #13: 0.48 (1028644672)
   #14: 0.48 (1030792128)
   #15: 0.48 (1026497152)
   Standard Deviation (q31, q31-to-float): 419008906, 0.20
   Standard Deviation (pure-float): 0.13

  This indicates some error introduced by the float-q31 or q31-float conversion
  process.  Otherwise the q31-to-float and pure-float stddev's would both read identically.
*/
