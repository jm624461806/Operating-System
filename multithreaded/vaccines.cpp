#include <iostream>
#include <pthread.h>
#include <string>
#include <fstream>
#include <semaphore.h>

/*
 * Created by Jingyi Mao
 */
using namespace std;

int NUMBER_OF_THREADS = 21;

struct vaccineCount {
    int v1 = 0;
    int v2 = 0;
    int v3 = 0;
};

void* threadCount(void* args_p) {
    vaccineCount* vaccineCount = new struct vaccineCount;
    int cityNumber = *((int*) args_p);
    delete (int*) args_p;
    string city = to_string(cityNumber);
    try {
        ifstream processFile("./hw4/city"+city+".txt");
        string vaccine;
        if ( processFile.good()) {
            while ( getline (processFile, vaccine)) {
                if (vaccine[vaccine.length()-1] == '\n') {
                    vaccine.erase(vaccine.length()-1);
                }

                if(vaccine.compare("v1") == 0) {
                    vaccineCount->v1 += 1;
                } else if(vaccine.compare("v2") == 0) {
                    vaccineCount->v2 += 1;
                } else if(vaccine.compare("v3") == 0) {
                    vaccineCount->v3 += 1;
                }
            }
        } else {
            cout << "File not exist.\n";
        }
        processFile.close();
        return (void*) vaccineCount;
    } catch(const std::exception& e) {
        cout << "Error occurred in thread " << city << " with exception: " << e.what() << "\n" << endl;
        exit(1);
    }
}

int main() {

    pthread_t threads[NUMBER_OF_THREADS];

    for(int i = 0; i < NUMBER_OF_THREADS; i++) {
        int* cityNumber = new int;
        *cityNumber = i+1;
        if (pthread_create(&threads[i], NULL, threadCount, (void*) cityNumber) != 0) {
            printf("ERROR: pthread creation failed.\n");
            return 1;
        };
    }

    void* resultsPointers[NUMBER_OF_THREADS];
    int v1=0, v2=0, v3=0;
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        if (pthread_join(threads[i], &(resultsPointers[i])) != 0) {
            printf("ERROR: pthread join failed.\n");
            return 1;
        } else {
//            // check if there's a null in void*, if there is, that's because the catch block in the thread-related function
//            // and we should terminte the program.

              // now use exit inside thread
//            if(resultsPointers[i] == NULL) {
//                terminate();
//            }
            vaccineCount* result = (vaccineCount*) resultsPointers[i];
            v1 += result->v1;
            v2 += result->v2;
            v3 += result->v3;
            delete result;
        }
    }
    printf("Total vaccine counts across 21 cities:\n");
    printf("v1: %i\n", v1);
    printf("v2: %i\n", v2);
    printf("v3: %i\n", v3);

    return 0;
}
