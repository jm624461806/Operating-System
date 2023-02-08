# CPU Scheduling
## Files
- **mas.cpp** cpp file which implements the Multiqueue Adaptive Scheduling (MAS) CPU scheduling algorithm.
- **srtf.cpp** cpp file which implements the Shortest-Remaining-Time-First (SRTF) CPU scheduling algorithm.
## Compilation and Execution
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
1(4) 
2(7) 
3(12) 
4(13) 
Average TurnAroundTime of MAS algorithm is: 3.25
Average WaitingTime of MAS algorithm is: 0
====================================================
```
### SRTF
```
====================================================
Report of SRTF algorithm: 
1(4) 
2(8) 
4(6) 
2(8) 
3(13) 
Average TurnAroundTime of SRTF algorithm is: 3.75
Average WaitingTime of SRTF algorithm is: 0.5
====================================================
```



