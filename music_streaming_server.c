// path=/home/krishna/Desktop/SEM6/NETWORKS/assignments/songs
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

/* SOUND CAPTURE AND STREAMING FROM DEVICE MICROPHONE */
/*
#include <alsa/asoundlib.h>

int capture_audio(snd_pcm_t *handle, char *buffer, int size) {
    int err;

    // Check if size exceeds the buffer size
    if (size > 4096) {
        fprintf(stderr, "Error: Buffer size exceeds maximum\n");
        return -1;
    }

    // Read audio data into buffer
    if ((err = snd_pcm_readi(handle, buffer, size)) != size) {
        fprintf(stderr, "audio read failed (%s)\n", snd_strerror(err));
        return -1;
    }

    return 0;
}
*/

int streaming;             // number of clients streaming
int *threadRunning = NULL; // array to check if thread is running or not
struct Values
{
    int client_fd;
    char *songdir;
    char *client_ip;
    int index;
}; // structure to pass multiple arguments to thread
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *fun(void *obj)
{
    int client_fd = ((struct Values *)obj)->client_fd;
    char *songdir = ((struct Values *)obj)->songdir;
    char *client_ip = ((struct Values *)obj)->client_ip;
    int index = ((struct Values *)obj)->index;
    char songNumberc[1024];
    read(client_fd, songNumberc, 1024);
    // printf("song number asked %s",songNumberc);
    int songNumber;
    sscanf(songNumberc, "%d", &songNumber); // converts string to integer

    printf("client %s (fd= %d)  asked for song number %d\n", client_ip, client_fd, songNumber);
    /* if(songNumber==-1)
     {
             snd_pcm_t *capture_handle;
             // Open PCM device for recording (capture)
             if (snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0) {
                     fprintf(stderr, "Error opening PCM device\n");
                     return NULL;
             }

             // Set parameters: Sample rate, channels, etc.
             snd_pcm_hw_params_t *hw_params;
             snd_pcm_hw_params_alloca(&hw_params);
             snd_pcm_hw_params_any(capture_handle, hw_params);
             snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
             snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE);
             snd_pcm_hw_params_set_channels(capture_handle, hw_params, 2);
             unsigned int sample_rate = 44100; //44KHz audio
             snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &sample_rate, 0);
             snd_pcm_hw_params(capture_handle, hw_params);


             // Send microphone data
             printf("HI\n");
             char buffer[4096];
             while (1) {
             // Capture audio from microphone
             if (capture_audio(capture_handle, buffer, 4096) < 0)
                 break;

             // Send microphone data to client
                 send(client_fd, buffer, 4096, 0);
             }
             return NULL;
     }*/
    FILE *fp;
    DIR *dir = opendir(songdir);
    if (dir == NULL)
    {
        printf("error in dir name");
        return NULL;
    }
    struct dirent *entry;
    int position = 0;
    while ((entry = readdir(dir)) != NULL)
    { // the first two entries are . and .. so we skip them
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        if (position == songNumber)
        {
            char ss[1000] = "";
            strcat(ss, songdir);
            strcat(ss, "/");
            strcat(ss, entry->d_name);
            fp = fopen(ss, "r");
            if (fp == NULL)
            {
                printf("error in opening file\n");
                return NULL;
            }
            break;
        }
        position++;
    }
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }
    int bytes_read;
    int BUFFER_SIZE = 4096;
    char buffer[4096] = {0};
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        int bytes_send = send(client_fd, buffer, bytes_read, 0);
        if (bytes_send != bytes_read)
        {
            perror("problem in sending or receiving\n");
            fclose(fp);
            closedir(dir);
            close(client_fd);
            pthread_mutex_lock(&lock);
            streaming--;
            threadRunning[index] = 0;
            pthread_mutex_unlock(&lock);
            return NULL;
        }
    }
    /*
    //to do this edit client code
    memset(buffer,0,4096);
    strcpy(buffer,"song completed\n");
    send(client_fd,buffer,bytes_read,0);
    */

    fclose(fp);
    closedir(dir);
    close(client_fd);
    pthread_mutex_lock(&lock);
    streaming--;
    threadRunning[index] = 0;
    pthread_mutex_unlock(&lock);
    // printf("fd= %d,  ip= %s ending\n",client_fd,client_ip);
}

int main(int argc, char *argv[])
{
    streaming = 0;
    int PORT = atoi(argv[1]);           // port number
    char *songdir = argv[2];            // path of songs
    int maxConnections = atoi(argv[3]); // maximum number of connections

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init has failed run again\n");
        return 1;
    }
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // SOCKET - Create TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address details
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // BIND - Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }
    int max_queue = 1000; // maximum number of clients that can wait in queue
    // LISTEN - Start listening for incoming connections
    if (listen(server_fd, max_queue) < 0)
    {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);
    pthread_t threads[maxConnections]; // array of threads
    threadRunning = malloc(maxConnections * (sizeof(int)));
    for (int i = 0; i < maxConnections; i++)
        threadRunning[i] = 0;

    while (1)
    {
        while (streaming == maxConnections)
        {

        } // wait till any thread is free
        int client_fd;
        // ACCEPT - accept incoming connection
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Acceptance failed");
            exit(EXIT_FAILURE);
        }

        char client_ip[INET_ADDRSTRLEN]; // client ip address
        inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Client IP address: %s connected\n", client_ip);
        struct Values *obj = (struct Values *)malloc(sizeof(struct Values));
        obj->client_fd = client_fd;
        obj->songdir = songdir;
        obj->client_ip = client_ip;

        // create thread
        for (int i = 0; i < maxConnections; i++)
        {
            if (threadRunning[i] == 0)
            {
                pthread_mutex_lock(&lock);
                threadRunning[i] = 1;
                streaming++;
                pthread_mutex_unlock(&lock);
                obj->index = i;
                pthread_create(&threads[i], NULL, &fun, obj);
                break;
            }
        }
    }

    // wait till all threads over
    for (int i = 0; i < maxConnections; i++)
    {
        int x;
        pthread_join(threads[i], (void **)&x);
    }
    printf("Main ending\n");
    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}
