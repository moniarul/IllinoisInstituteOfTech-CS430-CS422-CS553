#include <pthread.h>
#include <cstring>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mutex>

#define reportPath "/exports/home/schatterjee/reports/report-net"
#define sharedPath "/exports/home/schatterjee/reports/serverips"
#define PORTNO 1026
long  oneGBtoByte = 1000000000;

using namespace std;
pthread_mutex_t centalstore;

struct workDetails {
    int threadcount;
    int chunkSize;
    char *dataStore;
    char *dataToSend;
    char *serverIp;
    int sockfd;
    int threadid;
    bool isPingPong;
    unsigned int *totalRecv;
};

void error(const string msg) {
    perror(msg.c_str());
    exit(1);
}

int storeInfile(const char *filename, const char *data, char *mode = "a+") {
    FILE *f = fopen(filename, mode);
    if (f == NULL) error("Error opening file to Write Report");
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
    int threadcount = thisWork->threadcount;
    int chunkSize = thisWork->chunkSize;
    char *serverIp = thisWork->serverIp;
    char *dataToSend = thisWork->dataToSend;
    bool isPingPong = thisWork->isPingPong;

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
    serv_addr.sin_port = htons(PORTNO);
    int connection = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (connection < 0)
        error("Server is not up.");
    long totalPackets = oneGBtoByte/(chunkSize * threadcount);
    if (isPingPong) {

    } else {
        long i;
        for (i = 0; i < totalPackets; ++i) {
            if (send(sockfd, dataToSend, chunkSize, 0) < 0) error("ERROR writing to socket");
//        else {
//            pthread_mutex_lock(&centalstore);
//            cout << " client " << thisWork->threadid << "Wrote " << dataToSend << endl;
//            pthread_mutex_unlock(&centalstore);
//        }
        }
    }

    cout << " client " << thisWork->threadid << "Exit";
    close(connection);
    pthread_exit((void *) NULL);
}

void *serverWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int chunkSize = thisWork->chunkSize;
    char *dataStore = thisWork->dataStore;
    int sockfd = thisWork->sockfd;
    unsigned int *totalRecv = thisWork->totalRecv;
    bool isPingPong = thisWork->isPingPong;
    struct sockaddr_in cli_addr; // = thisWork->cli_addr;
    auto *buffer = new char[chunkSize + 1];
    long numOfBytesRead;

    listen(sockfd, 5);
    socklen_t clilen = sizeof(cli_addr);
    sockfd = ::accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (sockfd < 0) error("ERROR on accept");
    if (isPingPong) {

    } else {
        while (*totalRecv) {
            numOfBytesRead = recv(sockfd, buffer, chunkSize, 0);
            if (numOfBytesRead > 0) {
                pthread_mutex_lock(&centalstore);
                //cout << *totalRecv << "Server " << thisWork->threadid << " Received - " << numOfBytesRead << endl;
                buffer[numOfBytesRead] = '\0';
                strncat(dataStore, buffer, strlen(buffer));
                buffer[0]= 0;
                pthread_mutex_unlock(&centalstore);
                if (strlen(dataStore) >= oneGBtoByte) {
                    dataStore[oneGBtoByte] = '\0';
                    *totalRecv = 0;
                }
            }
        }
    }
    close(sockfd);
    delete[] buffer;
    pthread_exit((void *) NULL);
}

int main(int argc, char *argv[]) {
    char *isServer = argv[1];
    int NUM_THREADS = atoi(argv[2]);
    int BLOCKSIZE = atoi(argv[3]);
    string experiment = argv[4];
    bool isPingPong = argc > 5;
    int sizefactor = 100;
    //To avoid extra clculation to handle resedue byte . It should be fully divisible.
    BLOCKSIZE = BLOCKSIZE * sizefactor;
    oneGBtoByte = (oneGBtoByte / BLOCKSIZE ) * BLOCKSIZE;
    //The oneGb memory where data will be stored by server
    char *dataStore = new char[oneGBtoByte + 1];
    //Prepare data that will be sent
    char *dataToSend = new char[BLOCKSIZE + 1];
    fill_n(dataToSend, BLOCKSIZE, 'x');
    dataToSend[BLOCKSIZE] = '\0';
    unsigned int totalRecv = 1;
    struct timeval start, end;
    char serverIp[20];

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
    int sockfd;
    string customfile = sharedPath + experiment;
    if (strcmp(isServer, "server") == 0) {
        char host_name[50];
        gethostname(host_name, 50);

        if (!storeInfile(customfile.c_str(), host_name, "w")) cout << "Couldn't write IP to file" << endl;

        struct sockaddr_in serv_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("ERROR opening socket");
        tie(serv_addr.sin_family, serv_addr.sin_addr.s_addr, serv_addr.sin_port) = make_tuple(AF_INET, INADDR_ANY,
                                                                                              htons(PORTNO));

        if (::bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error("ERROR on binding");

    } else {
        readFromFile(customfile.c_str(), serverIp);
    }

    gettimeofday(&start, nullptr);
    for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
        detail[threadNo].threadcount = NUM_THREADS;
        detail[threadNo].chunkSize = BLOCKSIZE;
        detail[threadNo].dataStore = dataStore;
        detail[threadNo].threadid = threadNo;
        detail[threadNo].totalRecv = &totalRecv;
        detail[threadNo].isPingPong = isPingPong;

        if (strcmp(isServer, "server") == 0) {
            detail[threadNo].sockfd = sockfd;
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

    for (threadNo = 0; threadNo < NUM_THREADS; threadNo++) {
        rc = pthread_join(thread[threadNo], &status);
        if (rc) {
            error("ERROR; in p_thread join");
            exit(-1);
        }
    }
    gettimeofday(&end, NULL);

    double exec_t =
            (float) (end.tv_usec - start.tv_usec) / 1000000.0 + (float) (end.tv_sec - start.tv_sec);
    string report = "\n  Threads " + to_string(NUM_THREADS) +
                    ", BLOCKSIZE -" + to_string(BLOCKSIZE/sizefactor) +
                    ", TotalTime - " + to_string(exec_t) +
                    ", Mode " + isServer +
                    ", Total Data Received " + to_string(strlen(dataStore)) +
                    ", Experiment " + argv[4] +
                    ", Server Node  " + serverIp +
                    "..\n";
    cout << report;
    storeInfile(reportPath, report.c_str());
    delete[] dataStore;
    delete[] dataToSend;
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&centalstore);
    pthread_exit(NULL);
}