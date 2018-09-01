#include <pthread.h>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>

#define  iteration 100
#define GBtoByte 1e9
#define reportPath "/exports/home/schatterjee/reports/report-memory.txt"

using namespace std;

struct workDetails {
    int threadcount;
    int chunkSize;
    int accessType;
};

void error(char *msg) {
    perror(msg);
    exit(1);
}

int storeInfile(char *filename, const char *data) {
    FILE *f = fopen(filename, "a+");
    if (f == NULL) {
        cout << "Error opening file to Write Report";
        exit(1);
    }
    fprintf(f, "%s \n", data);
    fclose(f);
    return 1;
}

void *memoryWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadcount = thisWork->threadcount;
    int chunkSize = thisWork->chunkSize;
    int accessType = thisWork->accessType;
    int intSize = sizeof(int);
    int arraysize = GBtoByte / (threadcount * intSize);
    int noOfcopyOps = (arraysize * intSize / chunkSize);
    int *source = new int[arraysize];
    int *destination = new int[arraysize];
    fill_n(source, arraysize, 8.00);
    int noOfElemCopiedIneachItr = chunkSize/intSize;
    if (!accessType) { //Sequential access block
        for (int i = 0; i < noOfcopyOps; i++) {
            memcpy(destination + (noOfElemCopiedIneachItr * i ), source + (noOfElemCopiedIneachItr * i ), chunkSize);
        }
    }else { //Random access block
        int *randomArray = new int[noOfcopyOps];
        for(int i=0; i<noOfcopyOps; randomArray[i] = i,i++) ;
        shuffle (&randomArray[0], &randomArray[noOfcopyOps],default_random_engine(1122));
        for (int i = 0; i < noOfcopyOps; i++) {
            memcpy(destination + (noOfElemCopiedIneachItr * randomArray[i] ), source + (noOfElemCopiedIneachItr * randomArray[i] ), chunkSize);
        }
        delete[] randomArray;
    }
    //Print here to avoid loop elimination by -O3
    //cout << "\n After - Thread count " << threadcount << ", Array Size " << arraysize << " Value "<< destination[arraysize-1] << endl;
    delete[] source;
    delete[] destination;
    pthread_exit((void *) NULL);
}

int main(int argc, char *argv[]) {
    int threadcounts[] = {4, 2, 1};
    //Taking 16mb instead of 10mb as 1gb is not perfectly divisible by 10mb
    //And unnecessary if elese is required to make the code perfect.
    int blocksize[] = {10000000, 1000000, 1000};
    int accessType[] = {1,0}; // 0 RWS, 1 RWR

    string report;
    struct timeval start, end;

    for (int BLOCKSIZE: blocksize) {
        for (int NUM_THREADS: threadcounts) {
            for (int type: accessType) {
                gettimeofday(&start, NULL);
                for (int j = 0; j < iteration; j++) {
                    pthread_t thread[NUM_THREADS];
                    workDetails detail[NUM_THREADS];
                    pthread_attr_t attr;
                    int rc;
                    long threadNo;
                    void *status;

                    /* Initialize and set thread detached attribute */
                    pthread_attr_init(&attr);
                    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

                    for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
                        detail[threadNo].threadcount = NUM_THREADS;
                        detail[threadNo].chunkSize = BLOCKSIZE;
                        detail[threadNo].accessType = type;
                        rc = pthread_create(&thread[threadNo], &attr, memoryWorker, (void *) &detail[threadNo]);
                        if (rc) {
                            error("ERROR in creating p_thread");
                            exit(-1);
                        }
                    }

                    /* Free attribute and wait for the other threads */
                    pthread_attr_destroy(&attr);
                    for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
                        rc = pthread_join(thread[threadNo], &status);
                        if (rc) {
                            error("ERROR; in p_thread join");
                            exit(-1);
                        }
                    }
                }
                gettimeofday(&end, NULL);
                double exec_t =
                        (float) (end.tv_usec - start.tv_usec) / 1000000.0 + (float) (end.tv_sec - start.tv_sec);
                report = "\nThreads " + to_string(NUM_THREADS) +
                         ", BLOCKSIZE -" + to_string(BLOCKSIZE) +
                         ", TotalTime - " + to_string(exec_t) +
                         ", TotalData - GB:" + to_string(iteration) +
                         ", AccessType -" + (!type? "RWS" : "RWR")+
                         ", Throughput - GBPS:" + to_string(iteration/exec_t)+
                         "\n";
                cout << report;
                storeInfile(reportPath, report.c_str());
            }
        }
    }

    pthread_exit(NULL);
}