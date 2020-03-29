#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

using namespace std;

void delayloop(double req_delay) {
  struct timeval start, check, end;
  double elapsed_seconds;
  gettimeofday(&start, NULL);
  do {
    gettimeofday(&check, NULL);
    elapsed_seconds = (check.tv_sec + (check.tv_usec / 1000000.0)) -
                      (start.tv_sec + (start.tv_usec / 1000000.0));
  } while (elapsed_seconds < req_delay);
}

//create a thread per request
void * processRequest(void * varg) {
  /*
  //receive request
    char request[20];
    memset(request, 0, sizeof(request));
    recv(client_fd, request, sizeof(request), 0);
    cout << "request: " << request;
    //sparse the request
    string l1 = request;
    double delay = stoi(l1);
    cout << "delay: " << delay << endl;
    int num = stoi(l1.substr(l1.find(",") + 1));
    cout << "number of bucket: " << num << endl;
    //delay loop
    delayloop(delay);
    //add delay count to certain bucket
    bucket[num] += delay;

    //send response back
    string l2 = to_string(bucket[num]) + "\n";
    const char *response = l2.c_str();
    cout << "response: " << response; 
    send(client_fd, response, strlen(response), 0);
  */
}

// ./server num_of_cores threading_strategy num_of_buckets
int main(int argc, char * argv[]) {
  if (argc != 4) {
    cerr << "Error: incorrect number of arguments" << endl;
    exit(EXIT_FAILURE);
  }

  int cores = atoi(argv[1]);
  int threads = atoi(argv[2]);
  double bucket[atoi(argv[3])] = {0};

  //setup server
  //////////////////////////////////////////////////////////////
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;
  const char * hostname = NULL;
  const char * port = "12345";

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  }  //if

  socket_fd = socket(host_info_list->ai_family,
                     host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  }  //if

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  }  //if

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  }  //if

  cout << "Waiting for connection on port " << port << endl;
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_fd;
  client_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (client_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  }  //if
  //////////////////////////////////////////////////////////////////

  for (int i = 0; i < 100; i++) {
    //receive request
    char request[20];
    memset(request, 0, sizeof(request));
    recv(client_fd, request, sizeof(request), 0);
    cout << "request: " << request;
    //sparse the request
    string l1 = request;
    double delay = stoi(l1);
    cout << "delay: " << delay << endl;
    int num = stoi(l1.substr(l1.find(",") + 1));
    cout << "number of bucket: " << num << endl;
    //delay loop
    delayloop(delay);
    //add delay count to certain bucket
    bucket[num] += delay;

    //send response back
    string l2 = to_string(bucket[num]) + "\n";
    const char * response = l2.c_str();
    cout << "response: " << response;
    send(client_fd, response, strlen(response), 0);
  }

  freeaddrinfo(host_info_list);
  close(socket_fd);

  return 0;
}
