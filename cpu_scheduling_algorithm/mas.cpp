#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <map>

/*
 * Created by Jingyi Mao
 */
using namespace std;

struct ProcessInfo {
    int id;
    int cpuBurst; // works as remaining time at the same time
    int arrivalTime;
};

/*
 * Input: string s: string needs to be split
 *        string splitter
 * Output: ProcessInfo
 * This function is to split the s with splitter and wrap it into ProcessInfo.
 */
ProcessInfo splitAndReturnStruct(string s, string splitter)
{

    int pos;
    vector<string> afterSplit;
    struct ProcessInfo singleProcessInfo;

    s += splitter;
    int size = s.size();

    for(int i=0; i<size; i++)
    {
        pos = s.find(splitter,i);
        if(pos<size)
        {
            string sub = s.substr(i,pos-i);
            afterSplit.push_back(sub);
            i= pos + splitter.size()-1;
        }
    }

    singleProcessInfo.id = stoi(afterSplit[0]);
    singleProcessInfo.cpuBurst = stoi(afterSplit[1]);

    string arrivalTime = afterSplit[2];
    if (!arrivalTime.empty() && arrivalTime[arrivalTime.size() - 1] == '\r')
        arrivalTime.erase(arrivalTime.size() - 1);

    singleProcessInfo.arrivalTime = stoi(arrivalTime);

    return singleProcessInfo;
}

/*
 * Used in priority queue. Key is arrivalTime, if equal, compare id.
 */
bool operator<(const ProcessInfo &a, const ProcessInfo &b) {
    if (a.arrivalTime == b.arrivalTime)
        return a.id > b.id;
    return a.arrivalTime > b.arrivalTime;
}

/*
 * Input: priority_queue<ProcessInfo> allProcesses: All the processes need to be executed.
 *        vector<int> *processRunOnCpu: A vector takes the id of the process run on CPU
 *        map<int, int> *tMap: id->termination time maps, mapping the id and termination time
 * Output: None
 * Implementation of MASAlgorithm
 * In this function, three normal queues are used to realize the FCFS function. In each while loop cycle, the processes
 * contain arrival time less than current time will be added to the q0 and then q0->q1->q2. If else if were used to make
 * sure only one queue will be executed. In q1 and q2, process can be interrupted by a new coming process.
 */
void MASAlgorithm (priority_queue<ProcessInfo> allProcesses, vector<int> *processRunOnCpu, map<int, int> *tMap) {
    int q0TimeQuantum = 6;
    int q1TimeQuantum = 12;
    int currentTime = 0;

    queue<ProcessInfo> q0, q1, q2;
    while(!q0.empty() || !q1.empty() || !q2.empty() || !allProcesses.empty()) {
        // check currentTime, if there is any process arrives, add it to q0
        while(!allProcesses.empty() && currentTime >= allProcesses.top().arrivalTime) {
            q0.push(allProcesses.top());
            allProcesses.pop();
        }
        // check if q0 empty or not, execute q0 first
        if(!q0.empty()) {
            while(!q0.empty()) {
                ProcessInfo currentProcess = q0.front();
                q0.pop();

                if(currentProcess.cpuBurst <= q0TimeQuantum) {
                    // complete the execution
                    // and map termination time
                    currentTime += currentProcess.cpuBurst;
                    (*tMap)[currentProcess.id] = currentTime;
                } else {
                    currentProcess.cpuBurst -= q0TimeQuantum;
                    currentTime += q0TimeQuantum;
                    q1.push(currentProcess);
                }
                // add the process into the vector
                (*processRunOnCpu).push_back(currentProcess.id);
            }
        } else if (!q1.empty()) {
            // check if q1 empty or not, execute q1. If  the allprocess head has process which arrival time == currentTime,
            // we have to break the process and add the current process back to the tail
            while(!q1.empty()) {
                ProcessInfo currentProcess = q1.front();
                q1.pop();

                // The running process might be interrupted by a new coming process, if that happens, add them into q0 and
                // current process to the tail of the queue and break the queue.
                if((!allProcesses.empty() && (currentTime + q1TimeQuantum) > allProcesses.top().arrivalTime) &&
                        (currentProcess.cpuBurst + currentTime) > allProcesses.top().arrivalTime) {
                    int runningTime = allProcesses.top().arrivalTime - currentTime;
                    currentProcess.cpuBurst -= runningTime;
                    q1.push(currentProcess);
                    currentTime += runningTime;
                    break;
                } else {
                    if(currentProcess.cpuBurst <= q1TimeQuantum) {
                        // complete the execution
                        currentTime += currentProcess.cpuBurst;
                        (*tMap)[currentProcess.id] = currentTime;
                    } else {
                        currentProcess.cpuBurst -= q1TimeQuantum;
                        currentTime += q1TimeQuantum;
                        q2.push(currentProcess);
                    }
                }
                (*processRunOnCpu).push_back(currentProcess.id);
            }
        }else if (!q2.empty()) {
            // check if q2 empty or not, execute q2. If the allprocess head has process which arrival time <= currentTime,
            // we have to break the process and add the current process back to the tail
            while(!q2.empty()) {
                ProcessInfo currentProcess = q2.front();
                q2.pop();

                // same idea as q1.
                if(!allProcesses.empty() && (currentTime + currentProcess.cpuBurst) > allProcesses.top().arrivalTime){
                    int runningTime = allProcesses.top().arrivalTime - currentTime;
                    currentProcess.cpuBurst -= runningTime;
                    q2.push(currentProcess);
                    currentTime += runningTime;
                    break;
                } else {
                    currentTime += currentProcess.cpuBurst;
                    (*tMap)[currentProcess.id] = currentTime;
                }
                (*processRunOnCpu).push_back(currentProcess.id);
            }
        }
    }
}

/*
 * Main function that takes command line argument, extract the processes info, execute scheding algorithm.
 */
int main(int argc, char** argv)
{
    // check if the argument is 2
    if(argc != 2) {
        cout << "Only accept one argument.\n";
        return 1;
    }

    // ge the name of the file
    string fileName = argv[1];

    //use ifstream to open the file, store in a priority queue and check if file exists or not.
    ifstream processFile(fileName);
    string line;

    priority_queue<ProcessInfo> jobAPQ;
    vector<ProcessInfo> jobArr;
    string process;
    if ( processFile.good()) {
        while ( getline (processFile, process)) {
            // split the input and construct into the process struct
            ProcessInfo processStruct = splitAndReturnStruct(process, ",");
            jobAPQ.push(processStruct);
            jobArr.push_back(processStruct);
        }
    } else {
        cout << "File not exist.\n";
        processFile.close();
        return 1;
    }
    processFile.close();

    vector<int> processRunOnCpu;
    map<int, int> terminationMap;
    //execute the scheduling algorithm
    MASAlgorithm(jobAPQ,  &processRunOnCpu, &terminationMap);

    // MAS Algorithm
    cout << "===================================================="<< "\n";
    cout << "Report of MAS algorithm: "<< "\n";
    // print the process + termination time
    for(int i = 0; i < processRunOnCpu.size(); i++) {
        int id = processRunOnCpu[i];
        string message = to_string(id) + "(" + to_string(terminationMap[id]) + ") ";
        cout << message << " ";
    }
    cout << "\n";
    //print the turnaroundtime and waiting time
    float totalTurnAroundTime = 0.0;
    float totalWaitingTime = 0.0;
    for(int i = 0; i < jobArr.size(); i++) {
        totalTurnAroundTime += terminationMap[i+1] - jobArr[i].arrivalTime;
        totalWaitingTime += terminationMap[i+1] - jobArr[i].arrivalTime - jobArr[i].cpuBurst;
    }
    float averageTurnAroundTime = totalTurnAroundTime /  jobArr.size();
    float averageWaitingTime = totalWaitingTime /  jobArr.size();
    printf("Average TurnAroundTime of MAS algorithm is: %.3f\n", averageTurnAroundTime);
    printf("Average WaitingTime of MAS algorithm is: %.3f\n", averageWaitingTime);
    cout << "===================================================="<< "\n";

    return 0;
}
