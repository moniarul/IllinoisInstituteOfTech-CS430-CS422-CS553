#include <pthread.h>
#include <string>
#include <sys/time.h>
#include <iostream>
#include <immintrin.h>
#include <typeinfo>
#define  oneTrillion 1e12
#define reportPath "/exports/home/schatterjee/reports/report-cpu.txt"

using namespace std;

void error(char *msg) {
    perror(msg);
    exit(1);
}

int storeInfile(char* filename, const char* data){
    FILE *f = fopen(filename, "a+");
    if (f == NULL){
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
    T datatype;
    int threadCount = *((int*)t);
    long long i, adjustedIterationCount = 1;

    if(typeid(int) == typeid(datatype)){
        //_mm256_add_epi32 performs 8 ops in a cycle in following vector +1 for i++ = 9
        adjustedIterationCount = oneTrillion/(threadCount*17);
        __m256i store1 = _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 8);
        __m256i store2 = _mm256_setr_epi32(11, 21, 31, 41, 51, 61, 71, 81);
        for (i=0; i< adjustedIterationCount; i++){
            store1 = _mm256_add_epi32(store1, store2);
        }
        int* res = (int*)&store1;
        cout << " After Operation Int " << res[0] << "\n" << endl;
    }else if(typeid(char) == typeid(datatype)){
        //_mm256_add_epi8 performs 32 opertions in a single cycle in following vector +1 for i++ = 33
        adjustedIterationCount = oneTrillion/(threadCount*33);
        __m256i store1 = _mm256_setr_epi8('A' ,'B' ,'C' ,'D','E','F','G' ,'H' ,'I' ,'J' ,'K' ,'L' ,'M' ,'N' ,'O' ,'P' ,'Q' ,'R' ,'S' ,'T' ,'U' ,'V' ,'W' ,'X' ,'Y' ,'Z','V' ,'W' ,'X' ,'Y' ,'Z','Z');
        __m256i store2 = _mm256_setr_epi8(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32);
        for (i=0; i< adjustedIterationCount; i++){
            store1 = _mm256_add_epi8(store1, store2);
        }
        char* res = (char*)&store1;
        cout << " After Operation Char " << res[0] << "\n" << endl;
    } else if(typeid(short) == typeid(datatype)){
        //_mm256_add_epi8 performs 32 opertions in a single cycle in following vector +1 for i++ = 33
        adjustedIterationCount = oneTrillion/(threadCount*33);
        __m256i store1 = _mm256_setr_epi16(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
        __m256i store2 = _mm256_set_epi16(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
        for (i=0; i< adjustedIterationCount; i++){
            store1 = _mm256_sub_epi16(store1, store2);
        }
        short* res = (short*)&store1;
        cout << " After Operation Short " << res[0] << "\n" << endl;
    } else if(typeid(double) == typeid(datatype)) {
        //_mm256_fmsub_ps makes 16 ops in a cycle(vectorOfSize8 * 2 for each mull & sub ) +1 for i++ = total ops 17
        adjustedIterationCount = oneTrillion/(threadCount*17);
        __m256 store1 = _mm256_setr_ps (1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
        __m256 store2 = _mm256_setr_ps(1.0, 2.0, 3.0, 4.0,5.0, 6.0, 7.0, 8.0);
        for (i=0; i< adjustedIterationCount; i++){
            store1 = _mm256_fmsub_ps(store1, store2, store2);
        }
        double* res = (double*)&store1;
        cout << " After Operation Double " << ((double*)&store1)[0] << "\n" << endl;
    }
    pthread_exit(NULL);
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
            for(threadNo=0; threadNo< NUM_THREADS; threadNo++) {
                if(!precision.compare("QP")){
                    rc = pthread_create(&thread[threadNo], &attr, SPWork<char>, (void *)&NUM_THREADS);
                }else if(!precision.compare("HP")){
                    rc = pthread_create(&thread[threadNo], &attr, SPWork<short>, (void *)&NUM_THREADS);
                }else if(!precision.compare("SP")){
                    rc = pthread_create(&thread[threadNo], &attr, SPWork<int>, (void *)&NUM_THREADS);
                }else{
                    rc = pthread_create(&thread[threadNo], &attr, SPWork<double>, (void *)&NUM_THREADS);
                }

                if (rc) {
                    error("ERROR in creating p_thread");
                    exit(-1);
                }
            }

            /* Free attribute and wait for the other threads */
            pthread_attr_destroy(&attr);
            for(threadNo=0; threadNo< NUM_THREADS; threadNo++) {
                rc = pthread_join(thread[threadNo], &status);
                if (rc) {
                    error("ERROR; in p_thread join");
                    exit(-1);
                }
            }
            gettimeofday(&end, NULL);
            double exec_t = (float)(end.tv_usec - start.tv_usec)/1000000.0 + (float)(end.tv_sec - start.tv_sec) ;
            GigaOps = oneTrillion / (1e9 * exec_t);
            report = "Precisions- " + precision + ", Threads " + to_string(NUM_THREADS) + ", TotalTime - " + to_string(exec_t)
                     + ", GigaOps- " + to_string(GigaOps) + "\n";
            cout << report;
            storeInfile(reportPath, report.c_str());
        }
    }
    pthread_exit(NULL);
}