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


#define reportPath "/exports/home/schatterjee/reports/report-pingpong.txt"
#define sharedPath "/exports/home/schatterjee/ips/serverip"
#define oneMillion  1e6

using namespace std;
pthread_mutex_t centalstore;
int portnum[] = {1024,1025,1026,1027,1028, 1029, 1030, 1031};

struct workDetails {
    int threadcount;
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

void *tcpClientWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    long long *totatlRecv = thisWork->totalRecv;
    int threadid = thisWork->threadid;
    char *serverIp = thisWork->serverIp;
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
    long numOfBytesRead;
    char buffer[1] = {'y'};
    while (*totatlRecv <  oneMillion) {
        if (send(sockfd, buffer, 1, 0) < 0) error("ERROR writing to socket");
        numOfBytesRead = recv(sockfd, buffer, 1, 0);
        if (numOfBytesRead > 0) {
            pthread_mutex_lock(&centalstore);
            *totatlRecv += numOfBytesRead;
            pthread_mutex_unlock(&centalstore);
        }
    }
    close(connection);
    pthread_exit((void *) NULL);
}

void *tcpServerWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    long long *totatlRecv = thisWork->totalRecv;
    int threadid = thisWork->threadid;
    int port = portnum[threadid];
    struct sockaddr_in cli_addr;
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
    char buffer[1] = {'y'};
    while (*totatlRecv <  oneMillion) {
        numOfBytesRead = recv(sockfd, buffer, 1, 0);
        if (numOfBytesRead > 0) {
            if (send(sockfd, buffer, 1, 0) < 0) error("ERROR writing to socket");
            pthread_mutex_lock(&centalstore);
            *totatlRecv += numOfBytesRead;
            pthread_mutex_unlock(&centalstore);
        }
    }
    close(sockfd);
    pthread_exit((void *) NULL);
}

////UDP START

void *udpClientWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadid = thisWork->threadid;
    long long *totatlRecv = thisWork->totalRecv;
    char *serverIp = thisWork->serverIp;
    int port = portnum[threadid];
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
    long long numOfBytesRead;
    int servlen = sizeof(serv_addr);

    char buffer[1] = {'y'};
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    while (*totatlRecv <  oneMillion) {
        if (sendto(sockfd, buffer, 1, 0, (struct sockaddr *) &serv_addr, servlen) < 0)
            error("ERROR writing to socket");
        numOfBytesRead = recvfrom(sockfd, buffer, 1, 0,(struct sockaddr *)&remaddr, &addrlen);
        if (numOfBytesRead > 0) {
            pthread_mutex_lock(&centalstore);
            *totatlRecv += numOfBytesRead;
            pthread_mutex_unlock(&centalstore);
        }
    }
    pthread_exit((void *) NULL);
}

void *udpServerWorker(void *threadarg) {
    struct workDetails *thisWork;
    thisWork = (struct workDetails *) threadarg;
    int threadid = thisWork->threadid;
    long long *totatlRecv = thisWork->totalRecv;
    int port = portnum[threadid];
    long numOfBytesRead;

    struct sockaddr_in serv_addr, remaddr;
    socklen_t addrlen = sizeof(remaddr);
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    ::tie(serv_addr.sin_family, serv_addr.sin_addr.s_addr, serv_addr.sin_port) = ::make_tuple(AF_INET, INADDR_ANY,
                                                                                              htons(port));

    if (::bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    char buffer[1] = {'y'};
    while (*totatlRecv <  oneMillion) {
        numOfBytesRead = recvfrom(sockfd, buffer, 1, 0,(struct sockaddr *)&remaddr, &addrlen);
        if (numOfBytesRead > 0) {
            pthread_mutex_lock(&centalstore);
            *totatlRecv += numOfBytesRead;
            pthread_mutex_unlock(&centalstore);
            if (sendto(sockfd, buffer, 1, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR writing to socket");
        }
    }
    close(sockfd);
    pthread_exit((void *) NULL);
}

////UDP END

int main(int argc, char *argv[]) {
    char serverIp[20];
    char *isServer = argv[1];
    int NUM_THREADS = atoi(argv[2]);
    int isTcp = atoi(argv[3]);

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
    //999 is to separate ping pong ip files from basic TCP / UDP programme
    string customfile = sharedPath + to_string(9999) + to_string(NUM_THREADS)+to_string(isTcp);
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
        detail[threadNo].threadid = threadNo;
        detail[threadNo].totalRecv = &totalRecv;
        if (strcmp(isServer, "server") == 0) {
            if(isTcp){
                threadStatus = pthread_create(&thread[threadNo], &attr, tcpServerWorker, (void *) &detail[threadNo]);
            }else{
                threadStatus = pthread_create(&thread[threadNo], &attr, udpServerWorker, (void *) &detail[threadNo]);
            }

        } else {
            detail[threadNo].serverIp = serverIp;
            if(isTcp){
                threadStatus = pthread_create(&thread[threadNo], &attr, tcpClientWorker, (void *) &detail[threadNo]);
            } else{
                threadStatus = pthread_create(&thread[threadNo], &attr, udpClientWorker, (void *) &detail[threadNo]);
            }

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
    double exec_t =
            (float) (end.tv_usec - start.tv_usec) / 1000000.0 + (float) (end.tv_sec - start.tv_sec);
    string report = "\n  Threads " + to_string(NUM_THREADS) +
                    ", TotalTime - " + to_string(exec_t) +
                    ", Mode " + isServer +
                    ", Total Data Received " + to_string(totalRecv) +
                    ", Latency in ms: " +  to_string(exec_t*1000/oneMillion) +
                    ", Server Node  " + serverIp +
                    ", Transmission Mode: " + (isTcp ? "TCP" : "UDP" ) +
                    "..\n";
    cout << report;
    storeInfile(reportPath, report.c_str());
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&centalstore);
    pthread_exit(NULL);
}