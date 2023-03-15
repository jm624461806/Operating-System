Instructions for executing codes:
Part A:
Compilation: g++ vaccines.cpp -std=c++11 -lpthread -o vaccines
Execution: ./vaccines

Part B:
Compilation: g++ social_distancing.cpp -std=c++11 -lpthread -o social_distancing
Execution: ./social_distancing 3 6
Notes: I put a sleep() function inside the vaccinated kid thread so that the thread will not immedietely go back to the
while loop and possible to "block" the non-vaccinated kid threeads from being executed.