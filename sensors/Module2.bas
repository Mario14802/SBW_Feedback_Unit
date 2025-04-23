Attribute VB_Name = "Module2"

Function Calculate_ADC_Value(V_motor As Double, R200 As Double, R203 As Double, V_ref As Double, ADC_bits As Integer) As Double
    ' Define variables
    Dim V_adc_node As Double, max_ADC As Double, ADC_value As Double
    
    ' Calculate the voltage at the ADC input
    V_adc_node = V_motor * (R203 / (R200 + R203))
    
    ' Calculate max ADC count
    max_ADC = 2 ^ ADC_bits - 1
    
    ' Check if voltage exceeds reference voltage
    If V_adc_node > V_ref Then
        ADC_value = max_ADC ' Clamp to max ADC value
    Else
        ' Convert voltage to ADC digital value
        ADC_value = (V_adc_node / V_ref) * max_ADC
    End If
    
    ' Return the ADC value rounded to nearest integer
    Calculate_ADC_Value = Round(ADC_value, 0)
End Function

