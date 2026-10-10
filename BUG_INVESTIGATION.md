# Bug Investigation: Unreachable ADC Threshold

## 1. Objective
Investigate how an unreachable ADC threshold affects the warning status
and LED behavior in the STM32 sensor monitoring firmware.

## 2. Test Setup
- Board: STM32F401RE
- ADC resolution: 12-bit
- Valid ADC range: 0–4095
- Original threshold in the repository: 2500 ADC counts

## 3. Fault Injection
For a controlled experiment on the bug-reproduction-test branch,
the threshold was temporarily changed to 5000 ADC counts.

## 4. Observed Behavior
During testing, the UART output showed ADC readings reaching 4095.
The warning condition was never satisfied, and the LED remained OFF.

## 5. Root Cause
A 12-bit ADC cannot produce a value greater than 4095.
Therefore, the condition adc_value >= 5000U can never be true.

## 6. Correction and Verification
The threshold was restored to the original value of 2500 ADC counts.
The firmware should be rebuilt and tested to verify that readings at or
above 2500 trigger the warning and turn the LED ON, while lower readings
keep the LED OFF.

## 7. Evidence
Attach the UART screenshot showing the injected fault and the screenshot
showing the corrected behavior to the case-study report.

## 8. Conclusion
The experiment demonstrates how an incorrectly configured threshold
can make an alert condition unreachable. The fault was deliberately
introduced for testing and was not present in the original repository.
