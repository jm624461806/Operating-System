#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>

using namespace std;

struct Block {
    int blockNumber;
    string blockName;
    string contents;
};

struct singleIndexBlock {
    string singleIndexBlockName = "twelve.txt";
    Block*  directPointers[100];
};

struct doubleIndexBlock {
    string doubleIndexBlockName = "thirteen.txt";
    singleIndexBlock* singleIndexBlocks[100];
};

struct myNode {
    string fileName;
    Block*  directPointers[12];
    singleIndexBlock* singlePointer;
    doubleIndexBlock* doublePointer;
    int blockIndex=0;
    int fileSize=0;
};

struct Execution {
    string readOrWrite;
    int blockNumber;
    string contents;
};

const map<int, std::string> numberToEnglishMap = {
        {0, "zero.txt"},
        {1, "one.txt"},
        {2, "two.txt"},
        {3, "three.txt"},
        {4, "four.txt"},
        {5, "five.txt"},
        {6, "six.txt"},
        {7, "seven.txt"},
        {8, "eight.txt"},
        {9, "nine.txt"},
        {10, "ten.txt"},
        {11, "eleven.txt"},
};

const int MAX_SIZE = 10112;

void* splitAndReturnStruct(string s, string splitter, string type)
{
    int pos;
    vector<string> afterSplit;

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
    if(strcmp(type.c_str(), "block") == 0) {
        Block* block = new Block;
        block->blockNumber = stoi(afterSplit[0]);
        block->contents = afterSplit[1];
        return block;
    }

    Execution* execution = new Execution;

    execution->readOrWrite = afterSplit[0];
    execution->blockNumber = stoi(afterSplit[1]);
    if(afterSplit.size() == 3) {
        execution->contents = afterSplit[2];
    }
    return execution;
}

void putInFileSystem(Block* block, myNode* inputNode) {
    int blockNumber = block->blockNumber;
    // check if exceed the current max size;
    if(blockNumber >= MAX_SIZE) {
        cout << "input file greater than max supported file size!" << endl;
        exit(1);
    }
    if(inputNode->blockIndex < 12) {
        block->blockName = numberToEnglishMap.at(blockNumber);
        inputNode->directPointers[blockNumber] = block;
        inputNode->blockIndex += 1;
    } else if(inputNode->blockIndex == 12){
        //printf("%d %d\n", blockNumber, inputNode->blockIndex );
        if(blockNumber >= 12 && blockNumber < 112) {
            inputNode->singlePointer->directPointers[blockNumber-12] = block;
            string blockName = "twelve_" + to_string(blockNumber-12) + ".txt";
            block->blockName = blockName;
            if(blockNumber==111) inputNode->blockIndex += 1;
        }
    } else if(inputNode->blockIndex == 13) {
        int doubleBlockNumber = blockNumber-112;
        int singleBlockIndex = doubleBlockNumber / 100;
        int directBlockIndex= doubleBlockNumber % 100;
        string blockName = "thirteen_" + to_string(singleBlockIndex) + "_" + to_string(directBlockIndex) + ".txt";
        block->blockName = blockName;

        // if not initialized, initialize it.
        if (!inputNode->doublePointer->singleIndexBlocks[singleBlockIndex]) {
            inputNode->doublePointer->singleIndexBlocks[singleBlockIndex] = new singleIndexBlock;
        }

        inputNode->doublePointer->singleIndexBlocks[singleBlockIndex]->directPointers[directBlockIndex] = block;
    }
    inputNode->fileSize += 1;
}


void performExecution(Execution* execution, myNode* inputNode) {

    int blockNumber = execution->blockNumber;

    if(strcmp(execution->readOrWrite.c_str(), "R") == 0) {
        printf("Read request for block number %d.\n",blockNumber);
    }else {
        printf("Write request for block number %d.\n",blockNumber);
    }

    if(blockNumber < 0) {
        printf("Invalid block number! Negative Number!");
        return;
    }

    if(blockNumber >= MAX_SIZE) {
        printf("Invalid block number! Greater than maximum data block index!");
        return;
    }


    if(blockNumber < 12) {
        printf("Accessed pointer %d of myNode: %s\n",blockNumber, numberToEnglishMap.at(blockNumber).c_str());
        if(blockNumber+1 > inputNode->fileSize) {
            printf("Invalid block number!\n");
            return;
        }

        Block* block = inputNode->directPointers[blockNumber];
        string blockName = block->blockName;

        if(strcmp(execution->readOrWrite.c_str(), "R") == 0) {
            printf("Reading from %s.\n", blockName.c_str());
            printf("Contents:\n %s\n",block->contents.c_str());
        } else {
            block->contents = execution->contents;
        }
        return;
    } else if(blockNumber < 112){

        printf("Accessed pointer 12 of myNode: twelve.txt\n");
        if(blockNumber+1 > inputNode->fileSize) {
            printf("Invalid block number!\n");
            return;
        }
        Block* block = inputNode->singlePointer->directPointers[blockNumber-12];
        string blockName = block->blockName;


        printf("Accessed pointer %d of twelve.txt: %s\n", blockNumber-12, blockName.c_str());
        if(strcmp(execution->readOrWrite.c_str(), "R") == 0) {
            printf("Reading from %s.\n", blockName.c_str());
            printf("Contents:\n %s\n",block->contents.c_str());
        } else {
            block->contents = execution->contents;
        }
        return;
    } else {
        int doubleBlockNumber = blockNumber-112;
        int singleBlockIndex = doubleBlockNumber / 100;
        int directBlockIndex= doubleBlockNumber % 100;

        printf("Accessed pointer 13 of myNode: thirteen.txt\n");
        if(blockNumber+1 > inputNode->fileSize) {
            printf("Invalid block number!\n");
            return;
        }

        printf("Accessed pointer %i of thirteen.txt: thirteen_%d.txt.\n", singleBlockIndex, singleBlockIndex);
        printf("Accessed pointer %i of thirteen_%d.txt: thirteen_%d_%d.txt.\n", directBlockIndex, singleBlockIndex, singleBlockIndex, directBlockIndex);

        Block* block = inputNode->doublePointer->singleIndexBlocks[singleBlockIndex]->directPointers[directBlockIndex];
        string blockName = block->blockName;

        if(strcmp(execution->readOrWrite.c_str(), "R") == 0) {
            printf("Reading from thirteen_%d_%d.txt.\n", singleBlockIndex, directBlockIndex);
            printf("Contents:\n %s\n", block->contents.c_str());
        } else {
            block->contents = execution->contents;
        }
        return;
    }
}

int main(int argc, char** argv) {
    // check if the argument is 2
    if(argc != 3) {
        printf("Only accept two arguments.\n");
        return 1;
    }

    // get the name of the input file
    string inputFileName = argv[1];
    // get the name of the access trance file
    string accessFileName = argv[2];

    myNode* inputNode;
    inputNode = new struct myNode;
    inputNode->singlePointer = new singleIndexBlock;
    inputNode->doublePointer = new doubleIndexBlock;

    //use ifstream to open the file, store in myNode.
    ifstream processInputFile(inputFileName);
    string blockEntire;

    bool firstLine = false;
    if ( processInputFile.good()) {
        while ( getline (processInputFile, blockEntire)) {
            if(firstLine) {
                // split the input and construct into the Block struct;
                Block* block = new Block;
                block = (Block*)splitAndReturnStruct(blockEntire, ",", "block");
                putInFileSystem(block, inputNode);
            }
            firstLine = true;
        }
    } else {
        printf("Input File not exist.\n");
        processInputFile.close();
        return 1;
    }
    processInputFile.close();

    // execute trace
    ifstream processTraceFile(accessFileName);
    string executionLine;

    if (processTraceFile.good()) {
        while ( getline ( processTraceFile, executionLine)) {
            // split the input and construct into the Execution struct
            Execution *execution = (Execution *) splitAndReturnStruct(executionLine, ", ", "execution");
            performExecution(execution, inputNode);
            printf("\n");
            delete execution;
        }
    } else {
        printf("Access File not exist.\n");
        processTraceFile.close();
        return 1;
    }
    processTraceFile.close();

    delete inputNode;

    return 0;
}

