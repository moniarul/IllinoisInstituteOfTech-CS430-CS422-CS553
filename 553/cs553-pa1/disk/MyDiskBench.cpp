#include <pthread.h>
#include <string>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <algorithm>

#define TenGBtoByte 1e10
#define reportPath "/exports/home/schatterjee/reports/report-disk.txt"
#define fileToRW "/tmp/schatterjee.txt"

using namespace std;

pthread_mutex_t mutexsum;

struct workDetails {
    int threadcount;
    int chunkSize;
    int accessType;
    int threadid;
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

void *DiskWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadcount = thisWork->threadcount;
    int chunkSize = thisWork->chunkSize;
    int accessType = thisWork->accessType;
    int threadid = thisWork->threadid;
    //data to be written
    char *data = new char[chunkSize + 1];
    fill_n(data, chunkSize, char(threadid + 65));

    long noOfRW = (TenGBtoByte / (threadcount * chunkSize));
    FILE *theFile = fopen(fileToRW, "a+");

    if (accessType == 0) { //Write sequential Block
        for (long i = 0; i < noOfRW; ++i) {
            pthread_mutex_lock(&mutexsum);
            fprintf(theFile, "%s", data);
            data[chunkSize] = '\0';
            fflush(theFile);
            pthread_mutex_unlock(&mutexsum);
        }
    } else if (accessType == 1) { //Read sequential Block
        ifstream fileStrm(fileToRW, ifstream::binary);
        fileStrm.seekg(threadid * chunkSize * noOfRW);
        for (long i = 0; i < noOfRW; ++i) {
            fileStrm.read(data, chunkSize);
        }
        fileStrm.close();
    } else if (accessType == 2) { //Write Random Block
        int *randomArray = new int[noOfRW];
        for (int i = 0; i < noOfRW; randomArray[i] = i, i++);
        shuffle(&randomArray[0], &randomArray[noOfRW], default_random_engine(1122));
        for (long i = 0; i < noOfRW; ++i) {
            pthread_mutex_lock(&mutexsum);
            fseek(theFile, randomArray[i] * chunkSize, SEEK_CUR);
            fputs(data, theFile);
            pthread_mutex_unlock(&mutexsum);
        }

    } else { //Read Random Block
        int *randomArray = new int[noOfRW];
        for (int i = 0; i < noOfRW; randomArray[i] = i, i++);
        shuffle(&randomArray[0], &randomArray[noOfRW], default_random_engine(1122));
        for (long i = 0; i < noOfRW; ++i) {
            pthread_mutex_lock(&mutexsum);
            fseek(theFile, randomArray[i] * chunkSize, SEEK_CUR);
            fgets(data, chunkSize, theFile);
            pthread_mutex_unlock(&mutexsum);
            data[chunkSize] = '\0';
        }
    }
    delete[] data;
    fclose(theFile);
    pthread_exit((void *) NULL);
}

int main(int argc, char *argv[]) {
    int accessType[] = {0, 1, 2, 3}; // 0 WS, 1 RS, 2WR, 3 RR
    int NUM_THREADS = atoi(argv[1]);
    int BLOCKSIZE = atoi(argv[2]);
    string report;
    struct timeval start, end;

    for (int type: accessType) {
        pthread_t thread[NUM_THREADS];
        workDetails detail[NUM_THREADS];
        pthread_attr_t attr;
        int rc;
        long threadNo;
        void *status;
        ifstream fileStrm;

        /* Initialize and set thread detached attribute */
        pthread_mutex_init(&mutexsum, NULL);
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        gettimeofday(&start, NULL);
        for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
            detail[threadNo].threadcount = NUM_THREADS;
            detail[threadNo].chunkSize = BLOCKSIZE;
            detail[threadNo].accessType = type;
            detail[threadNo].threadid = threadNo;
            rc = pthread_create(&thread[threadNo], &attr, DiskWorker, (void *) &detail[threadNo]);
            if (rc) {
                error("ERROR in creating p_thread");
                exit(-1);
            }
        }

        for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
            rc = pthread_join(thread[threadNo], &status);
            if (rc) {
                error("ERROR; in p_thread join");
                exit(-1);
            }
        }
        gettimeofday(&end, NULL);
        /* Free attribute and wait for the other threads */
        pthread_attr_destroy(&attr);
        pthread_mutex_destroy(&mutexsum);

        double exec_t =
                (float) (end.tv_usec - start.tv_usec) / 1000000.0 + (float) (end.tv_sec - start.tv_sec);
        report = "\nThreads " + to_string(NUM_THREADS) +
                 ", BLOCKSIZE - MB" + to_string(BLOCKSIZE/(1000*1000.0)) +
                 ", TotalTime - " + to_string(exec_t) +
                 ", AccessType -" + (!type ? "WS" : type == 1 ? "RS" : type == 2 ? "WR" : "RR") +
                 ", Throughput MBps: " + to_string(TenGBtoByte / (1000 * 1000 * exec_t)) +
                 "\n";
        cout << report;
        storeInfile(reportPath, report.c_str());
        //Every write operation is follwed by read so after read
        // Delete the creted file To avoid disk overflow
        if ((type == 1 || type == 3) && remove(fileToRW) != 0)
            error("Error deleting file");
    }
    pthread_exit(NULL);
}