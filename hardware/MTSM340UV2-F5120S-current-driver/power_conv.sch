EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 7
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L board-rescue:MAX17632-custom MAX176321
U 1 1 5D889306
P 2800 1950
AR Path="/5D889306" Ref="MAX176321"  Part="1" 
AR Path="/5D65053E/5D889306" Ref="MAX176321"  Part="1" 
F 0 "MAX176321" H 3300 2600 50  0000 L CNN
F 1 "MAX17632" H 3300 1050 50  0000 L CNN
F 2 "custom_lib:QFN-16-1EP_3x3mm_Pitch0.5mm" H 2700 1850 50  0001 C CNN
F 3 "" H 2700 1850 50  0001 C CNN
	1    2800 1950
	1    0    0    -1  
$EndComp
$Comp
L power:+36V #PWR0124
U 1 1 5D897D9D
P 1550 1000
F 0 "#PWR0124" H 1550 850 50  0001 C CNN
F 1 "+36V" V 1565 1128 50  0000 L CNN
F 2 "" H 1550 1000 50  0001 C CNN
F 3 "" H 1550 1000 50  0001 C CNN
	1    1550 1000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2450 1250 2450 1000
Wire Wire Line
	2450 1000 2000 1000
Wire Wire Line
	2450 1000 2700 1000
Wire Wire Line
	2700 1000 2700 1250
Connection ~ 2450 1000
Wire Wire Line
	2000 1700 2000 1000
Connection ~ 2000 1000
Wire Wire Line
	2000 1000 1650 1000
$Comp
L Device:C C11
U 1 1 5D8988E8
P 4150 1950
F 0 "C11" H 4265 1996 50  0000 L CNN
F 1 "0.1uF" H 4265 1905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 4188 1800 50  0001 C CNN
F 3 "~" H 4150 1950 50  0001 C CNN
	1    4150 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 1700 3800 1700
Wire Wire Line
	4150 1700 4150 1800
Wire Wire Line
	3550 1900 3800 1900
Wire Wire Line
	3800 1900 3800 1700
Connection ~ 3800 1700
Wire Wire Line
	3800 1700 4150 1700
Wire Wire Line
	4150 2100 3550 2100
$Comp
L Device:L L1
U 1 1 5D899E5F
P 4650 1700
F 0 "L1" V 4469 1700 50  0000 C CNN
F 1 "10uH" V 4560 1700 50  0000 C CNN
F 2 "custom_lib:XAL5050-103MEC" H 4650 1700 50  0001 C CNN
F 3 "~" H 4650 1700 50  0001 C CNN
	1    4650 1700
	0    1    1    0   
$EndComp
Wire Wire Line
	4150 1700 4500 1700
Connection ~ 4150 1700
$Comp
L power:GND #PWR0162
U 1 1 5D89A8CF
P 4950 1400
F 0 "#PWR0162" H 4950 1150 50  0001 C CNN
F 1 "GND" H 4955 1227 50  0000 C CNN
F 2 "" H 4950 1400 50  0001 C CNN
F 3 "" H 4950 1400 50  0001 C CNN
	1    4950 1400
	-1   0    0    1   
$EndComp
Wire Wire Line
	4950 1700 4800 1700
$Comp
L Device:C C12
U 1 1 5D89B4FE
P 4950 1550
F 0 "C12" H 5065 1596 50  0000 L CNN
F 1 "22uF" H 5065 1505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4988 1400 50  0001 C CNN
F 3 "~" H 4950 1550 50  0001 C CNN
	1    4950 1550
	1    0    0    -1  
$EndComp
Connection ~ 4950 1700
Wire Wire Line
	3550 2300 4200 2300
Wire Wire Line
	4200 2300 4200 2800
Wire Wire Line
	4200 2800 5450 2800
Wire Wire Line
	5450 2800 5450 1700
Wire Wire Line
	4950 1700 5450 1700
Connection ~ 5450 1700
Wire Wire Line
	5450 1700 5550 1700
$Comp
L power:+5V #PWR0169
U 1 1 5D89FC59
P 5550 1700
F 0 "#PWR0169" H 5550 1550 50  0001 C CNN
F 1 "+5V" V 5565 1828 50  0000 L CNN
F 2 "" H 5550 1700 50  0001 C CNN
F 3 "" H 5550 1700 50  0001 C CNN
	1    5550 1700
	0    1    1    0   
$EndComp
$Comp
L Device:C C9
U 1 1 5D8A0F75
P 1650 1150
F 0 "C9" H 1765 1196 50  0000 L CNN
F 1 "2.2uF" H 1765 1105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1688 1000 50  0001 C CNN
F 3 "~" H 1650 1150 50  0001 C CNN
	1    1650 1150
	1    0    0    -1  
$EndComp
Connection ~ 1650 1000
Wire Wire Line
	1650 1000 1550 1000
$Comp
L power:GND #PWR0170
U 1 1 5D8A20C7
P 1650 1300
F 0 "#PWR0170" H 1650 1050 50  0001 C CNN
F 1 "GND" H 1655 1127 50  0000 C CNN
F 2 "" H 1650 1300 50  0001 C CNN
F 3 "" H 1650 1300 50  0001 C CNN
	1    1650 1300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0171
U 1 1 5D8A372C
P 2000 2300
F 0 "#PWR0171" H 2000 2050 50  0001 C CNN
F 1 "GND" V 2005 2172 50  0000 R CNN
F 2 "" H 2000 2300 50  0001 C CNN
F 3 "" H 2000 2300 50  0001 C CNN
	1    2000 2300
	0    1    1    0   
$EndComp
$Comp
L Device:C C8
U 1 1 5D8A3E13
P 1450 1950
F 0 "C8" H 1565 1996 50  0000 L CNN
F 1 "2.2uF" H 1565 1905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1488 1800 50  0001 C CNN
F 3 "~" H 1450 1950 50  0001 C CNN
	1    1450 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 1800 2000 1800
Wire Wire Line
	2000 1800 2000 1900
Wire Wire Line
	2000 2100 1450 2100
$Comp
L power:GND #PWR0172
U 1 1 5D8A51C4
P 1450 2100
F 0 "#PWR0172" H 1450 1850 50  0001 C CNN
F 1 "GND" H 1455 1927 50  0000 C CNN
F 2 "" H 1450 2100 50  0001 C CNN
F 3 "" H 1450 2100 50  0001 C CNN
	1    1450 2100
	1    0    0    -1  
$EndComp
Connection ~ 1450 2100
$Comp
L Device:C C10
U 1 1 5D8A54E8
P 2450 3250
F 0 "C10" H 2565 3296 50  0000 L CNN
F 1 "5600pF" H 2565 3205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2488 3100 50  0001 C CNN
F 3 "~" H 2450 3250 50  0001 C CNN
	1    2450 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 2850 2450 3100
$Comp
L power:GND #PWR0173
U 1 1 5D8A60DD
P 2450 3400
F 0 "#PWR0173" H 2450 3150 50  0001 C CNN
F 1 "GND" H 2455 3227 50  0000 C CNN
F 2 "" H 2450 3400 50  0001 C CNN
F 3 "" H 2450 3400 50  0001 C CNN
	1    2450 3400
	1    0    0    -1  
$EndComp
NoConn ~ 2950 2850
NoConn ~ 3200 2850
$Comp
L power:GND #PWR0174
U 1 1 5D8A9D36
P 2950 1050
F 0 "#PWR0174" H 2950 800 50  0001 C CNN
F 1 "GND" H 2955 877 50  0000 C CNN
F 2 "" H 2950 1050 50  0001 C CNN
F 3 "" H 2950 1050 50  0001 C CNN
	1    2950 1050
	-1   0    0    1   
$EndComp
Wire Wire Line
	2950 1050 2950 1150
Wire Wire Line
	3200 1250 3200 1150
Wire Wire Line
	3200 1150 2950 1150
Connection ~ 2950 1150
Wire Wire Line
	2950 1150 2950 1250
Wire Wire Line
	5450 2800 5450 3050
Wire Wire Line
	5450 3050 2700 3050
Wire Wire Line
	2700 3050 2700 2850
Connection ~ 5450 2800
$Comp
L board-rescue:MIC5365-1.2YC5-TR-TinyFPGA-BX-rescue U?
U 1 1 5E1B4969
P 8800 2025
AR Path="/5DEFED32/5E1B4969" Ref="U?"  Part="1" 
AR Path="/5D65053E/5E1B4969" Ref="U7"  Part="1" 
F 0 "U7" H 9000 2275 60  0000 L CNN
F 1 "MIC5365-1.2YC5-TR" H 9000 2175 60  0000 L CNN
F 2 "custom_lib:SC-70-C5" H 8800 2025 60  0001 C CNN
F 3 "" H 8800 2025 60  0001 C CNN
	1    8800 2025
	1    0    0    -1  
$EndComp
$Comp
L power:+1V2 #PWR?
U 1 1 5E1B4970
P 10100 1825
AR Path="/5DEFED32/5E1B4970" Ref="#PWR?"  Part="1" 
AR Path="/5D65053E/5E1B4970" Ref="#PWR0152"  Part="1" 
F 0 "#PWR0152" H 10100 1675 50  0001 C CNN
F 1 "+1V2" H 10100 1965 50  0000 C CNN
F 2 "" H 10100 1825 50  0001 C CNN
F 3 "" H 10100 1825 50  0001 C CNN
	1    10100 1825
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E1B4976
P 9400 2875
AR Path="/5DEFED32/5E1B4976" Ref="#PWR?"  Part="1" 
AR Path="/5D65053E/5E1B4976" Ref="#PWR0153"  Part="1" 
F 0 "#PWR0153" H 9400 2625 50  0001 C CNN
F 1 "GND" H 9400 2725 50  0000 C CNN
F 2 "" H 9400 2875 50  0001 C CNN
F 3 "" H 9400 2875 50  0001 C CNN
	1    9400 2875
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 2725 9400 2775
Wire Wire Line
	8700 2125 8800 2125
Connection ~ 9400 2775
$Comp
L board-rescue:C-TinyFPGA-BX-rescue C?
U 1 1 5E1B497F
P 8700 2425
AR Path="/5DEFED32/5E1B497F" Ref="C?"  Part="1" 
AR Path="/5D65053E/5E1B497F" Ref="C28"  Part="1" 
F 0 "C28" H 8725 2525 50  0000 L CNN
F 1 "1uF" H 8725 2325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8738 2275 50  0001 C CNN
F 3 "" H 8700 2425 50  0001 C CNN
	1    8700 2425
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:C-TinyFPGA-BX-rescue C?
U 1 1 5E1B4986
P 10100 2375
AR Path="/5DEFED32/5E1B4986" Ref="C?"  Part="1" 
AR Path="/5D65053E/5E1B4986" Ref="C36"  Part="1" 
F 0 "C36" H 10125 2475 50  0000 L CNN
F 1 "1uF" H 10125 2275 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 10138 2225 50  0001 C CNN
F 3 "" H 10100 2375 50  0001 C CNN
	1    10100 2375
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 2775 7500 2775
Wire Wire Line
	8700 2775 8700 2575
Connection ~ 8700 2125
Connection ~ 8700 2025
Wire Wire Line
	10100 1825 10100 2125
Wire Wire Line
	10000 2125 10100 2125
Connection ~ 10100 2125
Wire Wire Line
	10100 2775 10100 2525
$Comp
L power:+5V #PWR?
U 1 1 5E1B4995
P 6800 1825
AR Path="/5DEFED32/5E1B4995" Ref="#PWR?"  Part="1" 
AR Path="/5D65053E/5E1B4995" Ref="#PWR0175"  Part="1" 
F 0 "#PWR0175" H 6800 1675 50  0001 C CNN
F 1 "+5V" H 6800 1965 50  0000 C CNN
F 2 "" H 6800 1825 50  0001 C CNN
F 3 "" H 6800 1825 50  0001 C CNN
	1    6800 1825
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 1825 6800 2025
Connection ~ 6800 2025
$Comp
L power:GND #PWR?
U 1 1 5E1B499D
P 7500 2875
AR Path="/5DEFED32/5E1B499D" Ref="#PWR?"  Part="1" 
AR Path="/5D65053E/5E1B499D" Ref="#PWR0176"  Part="1" 
F 0 "#PWR0176" H 7500 2625 50  0001 C CNN
F 1 "GND" H 7500 2725 50  0000 C CNN
F 2 "" H 7500 2875 50  0001 C CNN
F 3 "" H 7500 2875 50  0001 C CNN
	1    7500 2875
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 2875 7500 2775
Connection ~ 7500 2775
$Comp
L board-rescue:C-TinyFPGA-BX-rescue C?
U 1 1 5E1B49A5
P 6800 2425
AR Path="/5DEFED32/5E1B49A5" Ref="C?"  Part="1" 
AR Path="/5D65053E/5E1B49A5" Ref="C17"  Part="1" 
F 0 "C17" H 6825 2525 50  0000 L CNN
F 1 "1uF" H 6825 2325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6838 2275 50  0001 C CNN
F 3 "" H 6800 2425 50  0001 C CNN
	1    6800 2425
	1    0    0    -1  
$EndComp
$Comp
L board-rescue:C-TinyFPGA-BX-rescue C?
U 1 1 5E1B49AC
P 8200 2425
AR Path="/5DEFED32/5E1B49AC" Ref="C?"  Part="1" 
AR Path="/5D65053E/5E1B49AC" Ref="C26"  Part="1" 
F 0 "C26" H 8225 2525 50  0000 L CNN
F 1 "1uF" H 8225 2325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8238 2275 50  0001 C CNN
F 3 "" H 8200 2425 50  0001 C CNN
	1    8200 2425
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 2575 6800 2775
Connection ~ 6800 2125
Wire Wire Line
	8200 2775 8200 2575
Connection ~ 8200 2775
Connection ~ 8700 2775
Wire Wire Line
	8700 2025 8700 2125
$Comp
L power:+3V3 #PWR?
U 1 1 5E1B49B9
P 8450 1825
AR Path="/5DEFED32/5E1B49B9" Ref="#PWR?"  Part="1" 
AR Path="/5D65053E/5E1B49B9" Ref="#PWR0188"  Part="1" 
F 0 "#PWR0188" H 8450 1675 50  0001 C CNN
F 1 "+3V3" H 8450 1965 50  0000 C CNN
F 2 "" H 8450 1825 50  0001 C CNN
F 3 "" H 8450 1825 50  0001 C CNN
	1    8450 1825
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 2025 8450 1825
Connection ~ 8450 2025
Text Notes 8050 1425 0    60   ~ 0
Voltage Regulation
Wire Wire Line
	9400 2775 9400 2875
Wire Wire Line
	9400 2775 10100 2775
Wire Wire Line
	8700 2125 8700 2275
Wire Wire Line
	8700 2025 8800 2025
Wire Wire Line
	10100 2125 10100 2150
Wire Wire Line
	6800 2025 6800 2125
Wire Wire Line
	7500 2775 8200 2775
Wire Wire Line
	7500 2775 7500 2725
Wire Wire Line
	6800 2125 6800 2275
Wire Wire Line
	8200 2025 8450 2025
Wire Wire Line
	8200 2775 8700 2775
Wire Wire Line
	8700 2775 9400 2775
Wire Wire Line
	8450 2025 8700 2025
Wire Wire Line
	6800 2025 6900 2025
Wire Wire Line
	6800 2125 6900 2125
$Comp
L board-rescue:MIC5504-3.3YM5-TR-TinyFPGA-BX-rescue U?
U 1 1 5E1B49D1
P 6900 2025
AR Path="/5DEFED32/5E1B49D1" Ref="U?"  Part="1" 
AR Path="/5D65053E/5E1B49D1" Ref="U6"  Part="1" 
F 0 "U6" H 7500 2315 60  0000 C CNN
F 1 "MIC5504-3.3YM5-TR" H 7500 2209 60  0000 C CNN
F 2 "custom_lib:SC70-5" H 6900 2025 60  0001 C CNN
F 3 "" H 6900 2025 60  0001 C CNN
	1    6900 2025
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 2025 8200 2125
Wire Wire Line
	8200 2125 8100 2125
Connection ~ 8200 2125
Wire Wire Line
	8200 2125 8200 2275
NoConn ~ 8100 2025
NoConn ~ 10000 2025
$Comp
L Connector:TestPoint TP8
U 1 1 5DF22875
P 5450 1700
F 0 "TP8" H 5508 1820 50  0000 L CNN
F 1 "TestPoint" H 5500 1950 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 5650 1700 50  0001 C CNN
F 3 "~" H 5650 1700 50  0001 C CNN
	1    5450 1700
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint TP9
U 1 1 5DF22949
P 8200 2025
F 0 "TP9" H 8258 2145 50  0000 L CNN
F 1 "TestPoint" H 8275 1950 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 8400 2025 50  0001 C CNN
F 3 "~" H 8400 2025 50  0001 C CNN
	1    8200 2025
	1    0    0    -1  
$EndComp
Connection ~ 8200 2025
$Comp
L Connector:TestPoint TP10
U 1 1 5DF22BCD
P 10500 2125
F 0 "TP10" H 10558 2245 50  0000 L CNN
F 1 "TestPoint" H 10575 2050 50  0000 L CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 10700 2125 50  0001 C CNN
F 3 "~" H 10700 2125 50  0001 C CNN
	1    10500 2125
	0    1    1    0   
$EndComp
Wire Wire Line
	10500 2125 10100 2125
Wire Wire Line
	10100 2150 10100 2225
$EndSCHEMATC