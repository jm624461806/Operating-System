//
// Created by Jingyi Mao on 2/24/23.
//
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;


// declare named semaphore variables because unnamed semaphore is deprecated in MacOS.
int vaccinatedKidsCount = 0;
sem_t* playMutex;
sem_t* countMutex;

struct kid {
    string vaccinated;
    int id;
};

void play(string threadType, int threadId) {
    printf("%s Thread %d playing!\n", threadType.c_str(), threadId);
    sleep(1); //for the sleep() use #include <unistd.h>
    printf("%s Thread %d playing done!\n", threadType.c_str(), threadId);
}

void* vKidsThread(void* args_p) {
    kid* vKid = (kid*) args_p;
    int id = vKid->id;
    delete vKid;

    do {
        printf("V Thread %d about to INCREMENT the count.\n", id);
        sem_wait(countMutex); // INSTRUCTION 3
        vaccinatedKidsCount++;
        if (vaccinatedKidsCount == 1) {
            printf("V Thread %d DESIRING to PLAY!\n",id);
            sem_wait(playMutex); // INSTRUCTION 4
        }
        printf("V Thread %d DONE INCREMENTing the count.\n", id);
        sem_post(countMutex); // INSTRUCTION 5

        play("V", id);

        sem_wait(countMutex); // INSTRUCTION 6
        printf("V Thread %d about to DECREMENT the count.\n", id);
        vaccinatedKidsCount--;
        if (vaccinatedKidsCount == 0) {
            printf("V Thread %d; LAST one DONE PLAYING. Will SIGNAL!\n", id);
            sem_post(playMutex); // INSTRUCTION 7
        }
        printf("V Thread %d DONE DECREMENTing the count.\n", id);
        sem_post(countMutex); // INSTRUCTION 8

//        // introduce timeout for vaccinated kids so that non-vaccinated kids could
//        // have a chance to play
        sleep(1);
    }while(true);
}

void* nonVKidsThread(void* args_p) {
    kid* vKid = (kid*) args_p;
    int id = vKid->id;
    delete vKid;
    do {
        printf("NV Thread %d DESIRING to PLAY!\n", id);
        sem_wait(playMutex); //INSTRUCTION 1
        play("NV", id);
        sem_post(playMutex); // INSTRUCTION 2
    }while(true);
}

int main(int argc, char** argv) {
    // We use named semaphores and the thread is running infinite while loop. The semaphore will never be closed.
    // Thus, we have to close it first in each run and recreate it
    playMutex = sem_open("playMutex", O_CREAT | O_EXCL, 0644, 1);
    if (playMutex == SEM_FAILED) {
        sem_unlink("playMutex");
        playMutex = sem_open("playMutex", O_CREAT, 0644, 1);
    }

    countMutex = sem_open("countMutex", O_CREAT | O_EXCL, 0644, 1);
    if (countMutex == SEM_FAILED) {
        // If the semaphore already exists, delete it and then recreate
        sem_unlink("countMutex");
        countMutex = sem_open("countMutex", O_CREAT, 0644, 1);
    }

    int numberOfV;
    int numberOfNV;

    // check if the argument is 2
    if(argc != 3) {
        cout << "We need three arguments including the exec file";
        return 1;
    }

    // get the number of vaccinated kids
    char* numberOfVK = argv[1];
    for(int i = 0; i < strlen(numberOfVK); i++) {
        if(isdigit(*(numberOfVK+i)) == 0) {
            printf("%s is not a integer\n", numberOfVK);
            return 1;
        }
    }

    // get the number of non-vaccinated kids
    char* numberOfNVK = argv[2];
    for(int i = 0; i < strlen(numberOfNVK); i++) {
        if(isdigit(*(numberOfNVK+i)) == 0) {
            printf("%s is not a integer\n", numberOfNVK);
            return 1;
        }
    }

    numberOfV = stoi(numberOfVK);
    numberOfNV = stoi(numberOfNVK);

    if(numberOfNV == 0 && numberOfV == 0) {
        printf("Both input are 0, exit\n");
        return 0;
    }

    pthread_t vThreads[numberOfV];
    pthread_t nonVThreads[numberOfNV];

    // start the non-vaccinated kids threads
    for(int i = 0; i < numberOfNV; i++) {
        kid* vKid = new struct kid;
        vKid->id = i+1;
        vKid->vaccinated = "NV";
        if (pthread_create(&nonVThreads[i], NULL, nonVKidsThread, (void*) vKid) != 0) {
            printf("ERROR: pthread creation failed.\n");
            return 1;
        };
    }

    // start the vaccinated kids threads
    for(int i = 0; i < numberOfV; i++) {
        kid* vKid = new struct kid;
        vKid->id = i+1;
        vKid->vaccinated = "V";
        if (pthread_create(&vThreads[i], NULL, vKidsThread, (void*) vKid) != 0) {
            printf("ERROR: pthread creation failed.\n");
            return 1;
        };
    }

    // join the vaccinated kids threads
    for (int i = 0; i < numberOfV; i++) {
        if (pthread_join(vThreads[i], NULL) != 0) {
            printf("ERROR: pthread join failed.\n");
            return 1;
        }
    }

    // join the non-vaccinated kids threads
    for (int i = 0; i < numberOfNV; i++) {
        if (pthread_join(nonVThreads[i], NULL) != 0) {
            printf("ERROR: pthread join failed.\n");
            return 1;
        }
    }

    // destroy the named semaphores
    sem_close(playMutex);
    sem_close(countMutex);
    sem_unlink("playMutex");
    sem_unlink("countMutex");

    return 0;
}