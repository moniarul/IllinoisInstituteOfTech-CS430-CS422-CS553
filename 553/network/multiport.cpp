#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mutex> //nedded or failing in linux
#include <fcntl.h>

#define reportPath "/exports/home/schatterjee/report.txt"
#define sharedPath "/exports/home/schatterjee/serverip.txt"
#define created "/tmp/results.txt"
#define oneGBtoByte  1000000000

using namespace std;
mutex cout_mutex;
pthread_mutex_t centalstore;
int portnum[] = {1024,1025,1026,1027};

struct workDetails {
    int threadcount;
    int chunkSize;
    char *dataStore;
    char *dataToSend;
    char *serverIp;
    int threadid;
    long long *totalRecv;
};

void error(char *msg) {
    perror(msg);
    exit(1);
}

int storeInfile(char *filename, const char *data, char* mode="a+") {
    FILE *f = fopen(filename, mode);
    if (f == NULL) {
        cout << "Error opening file to Write Report";
        exit(1);
    }
    fprintf(f, "%s", data);
    fclose(f);
    return 1;
}

void readIpFromFile(char *serverIp) {
    FILE *file;
    file = fopen(sharedPath, "r");
    fscanf(file, "%s", serverIp);
    cout << "Server is running at - " << serverIp << endl;
}

void *clientWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadcount = thisWork->threadcount;
    int threadid = thisWork->threadid;
    int chunkSize = thisWork->chunkSize;
    char *serverIp = thisWork->serverIp;
    char *dataToSend = thisWork->dataToSend;
    int port = portnum[threadid];
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //int status = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
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
    long totalPackets = oneGBtoByte / (chunkSize * threadcount);
    long i;

    for (i = 0; i < totalPackets; ++i) {
        if (send(sockfd, dataToSend, chunkSize, 0) < 0) error("ERROR writing to socket");
//        else {
//            lock_guard<mutex> guard(cout_mutex);
//            cout << " client " << thisWork->threadid << "Wrote " << dataToSend << endl;
//        }
    }

    cout << " client " << thisWork->threadid << "Exit";
    close(connection);
    pthread_exit((void *) NULL);
}

void *serverWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadid = thisWork->threadid;
    int threadcount = thisWork->threadcount;
    int chunkSize = thisWork->chunkSize;
    char *dataStore = thisWork->dataStore;
    long long *totatlRecv = thisWork->totalRecv;
    int port = portnum[threadid];
    struct sockaddr_in cli_addr;
    char *buffer = new char[chunkSize + 1];
    long numOfBytesRead;

    struct sockaddr_in serv_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    tie(serv_addr.sin_family, serv_addr.sin_addr.s_addr, serv_addr.sin_port) = make_tuple(AF_INET, INADDR_ANY,
                                                                                          htons(port));

    if (::bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    socklen_t clilen = sizeof(cli_addr);
    sockfd = ::accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    //fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
    if (sockfd < 0)
        error("ERROR on accept");

    while (1) {
        numOfBytesRead = recv(sockfd, buffer, chunkSize, 0);
        if (numOfBytesRead > 0) {
            lock_guard<mutex> guard(cout_mutex);
            *totatlRecv += 1;
            cout << *totatlRecv << "Server " << thisWork->threadid << " Received - " << numOfBytesRead << endl;
            pthread_mutex_lock(&centalstore);
            buffer[numOfBytesRead] = NULL;
            strncat(dataStore, buffer, strlen(buffer));
            pthread_mutex_unlock(&centalstore);
            buffer[0] = NULL;
            //if (*totatlRecv >= oneGBtoByte/chunkSize) {
            if (strlen(dataStore) >= oneGBtoByte) {
                dataStore[oneGBtoByte] = NULL;
                cout << "Done - " << endl;
                storeInfile(created, dataStore);
            }
        }
    }
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
    dataToSend[BLOCKSIZE] = NULL;

    string report;
    struct timeval start, end;

    pthread_t thread[NUM_THREADS];
    workDetails detail[NUM_THREADS];
    pthread_attr_t attr;
    int rc;
    long threadNo;
    void *status;

    /* Initialize and set thread detached attribute */
    pthread_mutex_init(&centalstore, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (strcmp(isServer, "server") == 0) {
        char host_name[50];
        gethostname(host_name, 50);
        if (!storeInfile(sharedPath, host_name, "w")) cout << "Couldn't write IP to file" << endl;
    } else {
        readIpFromFile(serverIp);
    }
    gettimeofday(&start, NULL);
    long long totalRecv = 0;
    for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
        detail[threadNo].threadcount = NUM_THREADS;
        detail[threadNo].chunkSize = BLOCKSIZE;
        detail[threadNo].threadid = threadNo;
        if (strcmp(isServer, "server") == 0) {
            detail[threadNo].totalRecv = &totalRecv;
            detail[threadNo].dataStore = dataStore;
            rc = pthread_create(&thread[threadNo], &attr, serverWorker, (void *) &detail[threadNo]);
        } else {
            detail[threadNo].serverIp = serverIp;
            detail[threadNo].dataToSend = dataToSend;
            rc = pthread_create(&thread[threadNo], &attr, clientWorker, (void *) &detail[threadNo]);
        }
        if (rc) {
            error("ERROR in creating p_thread");
            exit(-1);
        }
    }
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&centalstore);
    for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
        rc = pthread_join(thread[threadNo], &status);
        if (rc) {
            error("ERROR; in p_thread join");
            exit(-1);
        }
    }
    gettimeofday(&end, NULL);
    delete[] dataStore;
    delete[] dataToSend;
    double exec_t =
            (float) (end.tv_usec - start.tv_usec) / 1000000.0 + (float) (end.tv_sec - start.tv_sec);
    report = "\nThreads " + to_string(NUM_THREADS) +
             ", BLOCKSIZE -" + to_string(BLOCKSIZE) +
             ", TotalTime - " + to_string(exec_t) +
             "\n";
    cout << report;
    storeInfile(reportPath, report.c_str());

    pthread_exit(NULL);
}