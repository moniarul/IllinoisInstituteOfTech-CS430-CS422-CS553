#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <netinet/in.h>
#include <netdb.h>
#include <mutex>
#include <tuple>
#include <functional>


#define reportPath "/exports/home/schatterjee/reports/report-net.txt"
#define sharedPath "/exports/home/schatterjee/ips/serverip"

using namespace std;
pthread_mutex_t centalstore;
int portnum[] = {1024,1025,1026,1027,1028, 1029, 1030, 1031};
long long oneGBtoByte  = 1000000000;
struct workDetails {
    int threadcount;
    int chunkSize;
    char *dataToSend;
    char *serverIp;
    int threadid;
    long long *totalRecv;
};

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int storeInfile(const char *filename, const char *data, char* mode="a+") {
    FILE *f = fopen(filename, mode);
    if (f == NULL) {
        cout << "Error opening file to Write Report";
        exit(1);
    }
    fprintf(f, "%s", data);
    fclose(f);
    return 1;
}

void readFromFile(const char *filename, char *buff) {
    FILE *file;
    file = fopen(filename, "r");
    fscanf(file, "%s", buff);
}

void *clientWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadid = thisWork->threadid;
    int chunkSize = thisWork->chunkSize;
    char *serverIp = thisWork->serverIp;
    long long *totatlRecv = thisWork->totalRecv;
    char *dataToSend = thisWork->dataToSend;
    int port = portnum[threadid];
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening client socket");
    server = gethostbyname(serverIp);
    if (server == NULL) error("Could not detect server by node name");

    //change
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr,
          (char *) &serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    int connection = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if ( connection < 0)
        error("Server is not up.");
    long numOfBytesSent = 0;

    while (*totatlRecv <  100 * oneGBtoByte) {
        numOfBytesSent = send(sockfd, dataToSend, chunkSize, 0);
        if (numOfBytesSent > 0) {
            pthread_mutex_lock(&centalstore);
            *totatlRecv += numOfBytesSent;
            pthread_mutex_unlock(&centalstore);
            numOfBytesSent = 0;
        }
    }
    string x = "\n Client Send " + to_string(*totatlRecv) + " Count - " + to_string(100.0 * oneGBtoByte) ;
    storeInfile(reportPath, x.c_str());
    close(connection);
    pthread_exit((void *) NULL);
}

void *serverWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadid = thisWork->threadid;
    int chunkSize = thisWork->chunkSize;
    long long *totatlRecv = thisWork->totalRecv;
    int port = portnum[threadid];
    struct sockaddr_in cli_addr;
    char *buffer = new char[chunkSize + 1];
    long numOfBytesRead;

    struct sockaddr_in serv_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    ::tie(serv_addr.sin_family, serv_addr.sin_addr.s_addr, serv_addr.sin_port) = ::make_tuple(AF_INET, INADDR_ANY,
                                                                                              htons(port));

    if (::bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    socklen_t clilen = sizeof(cli_addr);
    sockfd = ::accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (sockfd < 0)
        error("ERROR on accept");

    while (*totatlRecv <  100 * oneGBtoByte) {
        numOfBytesRead = recv(sockfd, buffer, chunkSize, 0);
        if (numOfBytesRead > 0) {
            pthread_mutex_lock(&centalstore);
            *totatlRecv += numOfBytesRead;
            pthread_mutex_unlock(&centalstore);
            numOfBytesRead = 0;
        }
    }
    string x = "\n Recv " + to_string(*totatlRecv) + " Count - " + to_string(100.0 * oneGBtoByte) ;
    storeInfile(reportPath, x.c_str());
    close(sockfd);
    delete[] buffer;
    pthread_exit((void *) NULL);
}

int main(int argc, char *argv[]) {
    char serverIp[20];
    int NUM_THREADS = atoi(argv[2]);
    int BLOCKSIZE = atoi(argv[3]);
    char *isServer = argv[1];
    //The oneGb memory where data will be stored by server
    char *dataStore = new char[oneGBtoByte + 1];
    //Prepare data that will be sent
    char *dataToSend = new char[BLOCKSIZE + 1];
    fill_n(dataToSend, BLOCKSIZE, 'x');
    dataToSend[BLOCKSIZE] = '\0';
    struct timeval start, end;

    pthread_t thread[NUM_THREADS];
    workDetails detail[NUM_THREADS];
    pthread_attr_t attr;
    int threadStatus;
    long threadNo;
    void *status;

    /* Initialize and set thread detached attribute */
    pthread_mutex_init(&centalstore, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    string customfile = sharedPath + to_string(NUM_THREADS)+to_string(BLOCKSIZE);
    if (strcmp(isServer, "server") == 0) {
        char host_name[50];
        gethostname(host_name, 50);
        if (!storeInfile(customfile.c_str(), host_name, "w")) cout << "Couldn't write IP to file" << endl;
    } else {
        readFromFile(customfile.c_str(),serverIp);
    }
    gettimeofday(&start, NULL);
    long long totalRecv = 0;
    for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
        detail[threadNo].threadcount = NUM_THREADS;
        detail[threadNo].chunkSize = BLOCKSIZE;
        detail[threadNo].threadid = threadNo;
        detail[threadNo].totalRecv = &totalRecv;
        if (strcmp(isServer, "server") == 0) {
            threadStatus = pthread_create(&thread[threadNo], &attr, serverWorker, (void *) &detail[threadNo]);
        } else {
            detail[threadNo].serverIp = serverIp;
            detail[threadNo].dataToSend = dataToSend;
            threadStatus = pthread_create(&thread[threadNo], &attr, clientWorker, (void *) &detail[threadNo]);
        }
        if (threadStatus) {
            error("ERROR in creating p_thread");
            exit(-1);
        }
    }

    for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
        threadStatus = pthread_join(thread[threadNo], &status);
        if (threadStatus) {
            error("ERROR; in p_thread join");
            exit(-1);
        }
    }
    gettimeofday(&end, NULL);
    delete[] dataStore;
    delete[] dataToSend;
    double exec_t =
            (float) (end.tv_usec - start.tv_usec) / 1000000.0 + (float) (end.tv_sec - start.tv_sec);
    string report = "\n  Threads " + to_string(NUM_THREADS) +
                    ", BLOCKSIZE -" + to_string(BLOCKSIZE) +
                    ", TotalTime - " + to_string(exec_t) +
                    ", Mode " + isServer +
                    ", Total Data Recv/Send GB: " + to_string(totalRecv/(1000*1000*1000.0)) +
                    ", Throughput in Mbps: " +  to_string(totalRecv*8.0/(exec_t*1000*1000)) +
                    ", Server Node  " + serverIp +
                    "..\n";
    cout << report;
    storeInfile(reportPath, report.c_str());
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&centalstore);
    pthread_exit(NULL);
}