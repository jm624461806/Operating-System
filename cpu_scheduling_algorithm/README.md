# CPU Scheduling Algorithm
## Files
- **mas.cpp** cpp file which implements the Multiqueue Adaptive Scheduling (MAS) CPU scheduling algorithm.
- **srtf.cpp** cpp file which implements the Shortest-Remaining-Time-First (SRTF) CPU scheduling algorithm.
## Compilation and Execution 
Assuming the txt file in under the same directory of the executable files
### MAS
- Compile MAS: ```g++ mas.cpp -o mas```
- Execute MAS: ```./mas test.txt ```
### SRTF
- Compile SRTF: ```g++ srtf.cpp -o srtf```
- Execute SRTF: ```./srtf test.txt ```
## Example Output
### MAS
```
====================================================
Report of MAS algorithm: 
1(15)  2(10)  3(11)  4(14)  1(15)  
Average TurnAroundTime of SRTF algorithm is: 9.750
Average WaitingTime of SRTF algorithm is: 6.000
====================================================
```
### SRTF
```
====================================================
Report of SRTF algorithm: 
1(15)  2(7)  3(5)  2(7)  4(10)  1(15)  
Average TurnAroundTime of SRTF algorithm is: 6.500
Average WaitingTime of SRTF algorithm is: 2.750
====================================================
```



