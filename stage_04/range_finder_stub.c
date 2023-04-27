#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define FIFO_NAME_MAX_LENGTH 255
void help();

void Exiting(int parameter) {
    exit(parameter);
}

void Exiting_sig() {
    exit(0);
}

void create_fifo(char *path)
{
    if (access(path, F_OK) == -1)
    {
        if (mkfifo(path, 0666) != 0)
        {
            perror("Error creating the named pipe");
            exit(EXIT_FAILURE);
        }
    }
}

void write_to_fifo(char *path, char *message)
{
    int fifo_fd = open(path, O_WRONLY);
    if (fifo_fd == -1)
    {
        perror("Error opening the named pipe");
        exit(EXIT_FAILURE);
    }
    write(fifo_fd, message, strlen(message) + 1);
    close(fifo_fd);
}


double get_fake_distance() {
    srand(time(NULL));
    return (rand() % 400) / 10.0;
}

int main(int argc, char *argv[]) {
    struct timespec ts;
    
    char range_finder[FIFO_NAME_MAX_LENGTH] = {0};
    int quiet = 0;
    //printf("%d is arg count \n",argc);
    if (argc > 2) {
        if ((strcmp(argv[1], "-h") == 0)) {
            help();
            return 0;
        } else {
            if ((strcmp(argv[1], "-q") == 0)) {
                quiet = 1;
            }
        }
        // Получите имя именованного канала из аргумента командной строки

         strncpy(range_finder, argv[2], FIFO_NAME_MAX_LENGTH - 1);
         printf("%s is the range_finder named channel \n", range_finder);
    } // else {
    //     help();
    //     return 0;
    // }

    // if ((quiet && argc != 3) || (!quiet && argc != 2)) {
    //     help();
    //     return 0;
    // }

    if (!quiet)
        printf("\nThe fake rangefinder application was started\n\n");

    double sl;
    int argument = 1;
    if (quiet)
        argument++;
    
    create_fifo(range_finder);
    signal(SIGINT, Exiting_sig);

    while (1) {
        double fake_distance = get_fake_distance();
        clock_gettime(CLOCK_REALTIME, &ts);
        if (!quiet) {
            char temp_message[64];
            snprintf(temp_message, sizeof(temp_message), "[%ld.%09ld] Distance: %lf cm\n", ts.tv_sec, ts.tv_nsec, fake_distance);
            write_to_fifo(range_finder, temp_message);
          
            //write_to_fifo(range_finder,)
            printf("[%ld.%09ld] Distance: %lf cm\n", ts.tv_sec, ts.tv_nsec, fake_distance);
        }
            //printf("Fake distance: %lf cm\n", fake_distance);
        else
            printf("%lf\n", fake_distance);
        fflush(stdout);

        sl = atoi(argv[argument]);
        if ((sl > 0) && (sl < 60000))
            usleep(sl * 1000);
        else
            sleep(1);
    }

    return 0;
}

void help() {
    printf("    Use this fake rangefinder application for generating random distances\n");
    printf("    execute format: ./fake_range TIME NAME_CHANNEL_PATH\n");
    printf("    return: length in cm\n");
    printf("    TIME - pause between writing in ms\n");
    printf("    NAME_CHANNEL_PATH - path to name channel\n");
    printf("    -h - help\n");
    printf("    -q - quiet\n");
}
