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
 * Custom comparator comparing arrival time
 */
class myComparatorArrival {
public:
    bool operator()(const ProcessInfo &p1, const ProcessInfo &p2) {
        if (p1.arrivalTime == p2.arrivalTime)
            return p1.id > p2.id;
        return p1.arrivalTime > p2.arrivalTime;
    }
};

/*
 * Custom comparator comparing remaining time
 */
class myComparatorRemaining {
public:
    bool operator()(const ProcessInfo &p1, const ProcessInfo &p2) {
        if (p1.cpuBurst == p2.cpuBurst)
            return p1.id > p2.id;
        return p1.cpuBurst > p2.cpuBurst;
    }
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
 * Input: priority_queue<ProcessInfo, vector<ProcessInfo>, myComparatorArrival> allProcesses: All the processes need to be executed.
 *        vector<int> *processRunOnCpu: A vector takes the id of the process run on CPU
 *        map<int, int> *tMap: id->termination time maps, mapping the id and termination time
 *        int totalTime: Total time to complete all the processes
 * Output: None
 * Implementation of SRTFAlgorithm
 * In this function, two PQ were used, one compares the arrivalTime, one compares the remaining time.
 * The total running time is known, we will go through the running time second by second. When the current
 * time is greater than the process arrival time, add them to the remainingTime PQ. Each time, pop out from remainingTime PQ,
 * deduct the remaining time (cpuburst) by one. If zero, means execution is completed.
 */
void SRTFAlgorithm(priority_queue<ProcessInfo, vector<ProcessInfo>, myComparatorArrival> allProcesses, vector<int> *processRunOnCpu, map<int, int> *tMap, int totalTime) {
    // initialize a PQ with a comparator comparing remainingTime, in this case cpuburst.
    priority_queue<ProcessInfo, vector<ProcessInfo>, myComparatorRemaining> jobRPQ;

    for(int currentTime=0; currentTime < totalTime; currentTime++) {
        while(!allProcesses.empty() && currentTime >= allProcesses.top().arrivalTime) {
            jobRPQ.push(allProcesses.top());
            allProcesses.pop();
        }

        ProcessInfo currentProcess = jobRPQ.top();
        jobRPQ.pop();

        currentProcess.cpuBurst -= 1;

        if((*processRunOnCpu).empty() || (!(*processRunOnCpu).empty() && (*processRunOnCpu).back() != currentProcess.id)) {
            (*processRunOnCpu).push_back(currentProcess.id);
        }

        // if this process is completed, dont put it into the PQ
        if(currentProcess.cpuBurst == 0) {
            (*tMap)[currentProcess.id] = (currentTime + 1);
            continue;
        }
        jobRPQ.push(currentProcess);
    }
};

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

    priority_queue<ProcessInfo, vector<ProcessInfo>, myComparatorArrival> jobAPQ;
    vector<ProcessInfo> jobArr;
    string process;
    int totalTimeNeeded;
    if ( processFile.good()) {
        while ( getline (processFile, process)) {
            // split the input and construct into the process struct
            ProcessInfo processStruct = splitAndReturnStruct(process, ",");
            jobAPQ.push(processStruct);
            jobArr.push_back(processStruct);
            totalTimeNeeded += processStruct.cpuBurst;
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
    SRTFAlgorithm(jobAPQ,  &processRunOnCpu, &terminationMap, totalTimeNeeded);

    // SRTF Algorithm
    cout << "===================================================="<< "\n";
    cout << "Report of SRTF algorithm: "<< "\n";
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
    printf("Average TurnAroundTime of SRTF algorithm is: %.3f\n", averageTurnAroundTime);
    printf("Average WaitingTime of SRTF algorithm is: %.3f\n", averageWaitingTime);
    cout << "===================================================="<< "\n";

    return 0;
}
