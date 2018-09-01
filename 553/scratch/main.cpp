#include <pthread.h>
#include <string>
#include <iostream>
#define TenGBtoByte 1048576
#define fileToWrite "/tmp/schatterjee.txt"

using namespace std;
pthread_mutex_t mutexsum;
struct workDetails {
    int threadcount;
    int chunkSize;
    char *data;
};

void *SPWork(void *threadarg) {

    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadcount = thisWork->threadcount;
    int chunkSize = thisWork->chunkSize;
    char *data = thisWork->data;
    long noOfWrites = (TenGBtoByte / (threadcount * chunkSize));
    FILE *f = fopen(fileToWrite, "a+");

    for (long i = 0; i < noOfWrites; ++i) {
        pthread_mutex_lock(&mutexsum);
        fprintf(f, "%s", data);
        fflush (f);
        pthread_mutex_unlock(&mutexsum);
    }
    fclose(f);
    pthread_exit((void *) NULL);
}

int main(int argc, char *argv[]) {
    int blocksize[] = {1024};
    int NUM_THREADS = 128;
    for (int BLOCKSIZE: blocksize) {
        char *data = new char[BLOCKSIZE+1];
        fill_n(data, BLOCKSIZE, 'x');
        data[BLOCKSIZE] = NULL;

        pthread_t thread[NUM_THREADS];
        workDetails detail[NUM_THREADS];
        pthread_attr_t attr;
        int rc;
        long threadNo;
        void *status;
        pthread_mutex_init(&mutexsum, NULL);
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
            detail[threadNo].threadcount = NUM_THREADS;
            detail[threadNo].chunkSize = BLOCKSIZE;
            detail[threadNo].data = data;
            rc = pthread_create(&thread[threadNo], &attr, SPWork, (void *) &detail[threadNo]);
            if (rc) exit(-1);
        }
        pthread_attr_destroy(&attr);
        for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
            rc = pthread_join(thread[threadNo], &status);
            if (rc) exit(-1);
        }
        pthread_mutex_destroy(&mutexsum);
        delete[] data;
    }
    pthread_exit(NULL);
}