#include <pthread.h>
#include <string>
#include <sys/time.h>
#include <iostream>
#define  oneMillion 10000
#define reportPath "/exports/home/schatterjee/report.txt"

using namespace std;

void error(char *msg) {
    perror(msg);
    exit(1);
}

int storeInfile(char* filename, const char* data){
    FILE *f = fopen(filename, "a+");
    if (f == NULL)
    {
        cout <<"Error opening file to Write Report";
        exit(1);
    }
    fprintf(f, "%s \n", data);
    fclose(f);
    return 1;
}

template <typename T>
void *SPWork(void *t)
{
    T* temp = (T*) t;
    T r = *temp;
    int i,j, adjustedIterationCount;
    adjustedIterationCount = oneMillion/r;
    for (i=0; i< adjustedIterationCount; i++)
    {
        for (j = 0; j < oneMillion ; ++j) {
            r++ ;
        }
    }
    pthread_exit((void*) t);
}

int main (int argc, char *argv[])
{
    int threadcounts[] = {1,2,4};
    string precisions[] = {"QP", "HP", "SP", "DP"};
    string report;
    double GigaOps;
    struct timeval start, end;
    for (string precision: precisions) {
        for (int NUM_THREADS: threadcounts){
            pthread_t thread[NUM_THREADS];
            pthread_attr_t attr;
            int rc;
            long threadNo;
            void *status;

            /* Initialize and set thread detached attribute */
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
            gettimeofday(&start, NULL);
            for(threadNo=1; threadNo<= NUM_THREADS; threadNo++) {
                if(!precision.compare("QP")){
                    char dataType = NUM_THREADS;
                    rc = pthread_create(&thread[threadNo], &attr, SPWork<char>, (void *)&dataType);
                }else if(!precision.compare("HP")){
                    short dataType = NUM_THREADS;
                    rc = pthread_create(&thread[threadNo], &attr, SPWork<short>, (void *)&dataType);
                }else if(!precision.compare("HP")){
                    int dataType = NUM_THREADS;
                    rc = pthread_create(&thread[threadNo], &attr, SPWork<int>, (void *)&dataType);
                }else{
                    double dataType = NUM_THREADS;
                    rc = pthread_create(&thread[threadNo], &attr, SPWork<double>, (void *)&dataType);
                }

                if (rc) {
                    error("ERROR in creating p_thread");
                    exit(-1);
                }
            }

            /* Free attribute and wait for the other threads */
            pthread_attr_destroy(&attr);
            for(threadNo=1; threadNo<= NUM_THREADS; threadNo++) {
                rc = pthread_join(thread[threadNo], &status);
                if (rc) {
                    error("ERROR; in p_thread join");
                    exit(-1);
                }
            }
            gettimeofday(&end, NULL);
            double exec_t = (float)(end.tv_usec - start.tv_usec)/1000000.0 + (float)(end.tv_sec - start.tv_sec) ;
            GigaOps = (oneMillion * oneMillion) / (1e9 * exec_t);
            report = "Precisions- " + precision + ", Threads " + to_string(NUM_THREADS) + ", TotalTime - " + to_string(exec_t)
                    + ", GigaOps- " + to_string(GigaOps) + "\n";
            cout << report;
            storeInfile(reportPath, report.c_str());
        }
    }
    pthread_exit(NULL);
}