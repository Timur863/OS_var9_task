#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_NAME_MAX_LENGTH 255

#define MIN_VAL 500
#define SENSITIVITY_250 8.75f
#define SENSITIVITY_500 17.5f
#define SENSITIVITY_2000 70.0f

#define MAGIC_CONST_X (SENSITIVITY_2000 * 2)
#define MAGIC_CONST_Y (SENSITIVITY_2000 * 2)
#define MAGIC_CONST_Z (SENSITIVITY_2000 * 2)

#define TIME 10.0

uint8_t xSign, ySign, zSign;
float xPosition = 0;
float yPosition = 0;
float zPosition = 0;

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

int askTemp()
{
    int temp = rand() % 20 - 10;
    printf("Temp deviation : %d\n", temp);
    fflush(stdout);
    return temp;
}

float askGiro(char pos, double time)
{
    int16_t gyroValue = rand() % 65536 - 32768;
    if (gyroValue < MIN_VAL && gyroValue > -MIN_VAL)
        gyroValue = 0;

    if ((gyroValue & 0x8000) == 0)
        xSign = 0;
    else {
        xSign = 1;
        gyroValue &= 0x7FFF;
        gyroValue = 0x8000 - gyroValue;
    }

    if (pos == 'X') {
        if (xSign == 0)
            xPosition += MAGIC_CONST_X * gyroValue * (time / 1000) / 1000;
        else
            xPosition -= MAGIC_CONST_X * gyroValue * (time / 1000) / 1000;
        printf("X : %lf\n", xPosition);
        return xPosition;
    } else if (pos == 'Y') {
        if (ySign == 0)
            yPosition = MAGIC_CONST_Y * gyroValue * (time / 1000) / 1000;
        else
            yPosition -= MAGIC_CONST_Y * gyroValue * (time / 1000) / 1000;
        printf("Y : %lf\n", yPosition);
        return yPosition;
    } else if (pos == 'Z') {
        if (zSign == 0)
            zPosition = MAGIC_CONST_Z * gyroValue * (time / 1000) / 1000;
        else
            zPosition -= MAGIC_CONST_Z * gyroValue * (time / 1000) / 1000;
        printf("Z : %lf\n", zPosition);
        return zPosition;
    }
}

void help()
{
    printf("    Use this application for reading from gyroscope\n");
    printf("    execute format: ./gyro [-h][-t][-g] \n");
    printf("    [-h] return: help\n");
    printf("    [-g] return: gyroscope deflection angle in X,Y,Z\n");
    printf("    [-t] return: device temperature change in C deg\n");
}

int main(int argc, char *argv[])
{
    struct timespec ts;
    char nav_data[FIFO_NAME_MAX_LENGTH] = {0};
    int state = 0;
    if (argc > 1) {
        if ((strcmp(argv[1], "-h") == 0)) {
            help();
            return 0;
        } else if ((strcmp(argv[1], "-t") == 0)) {
            state = 1;
        } else if ((strcmp(argv[1], "-g") == 0)) {
state = 2;
} else {
help();
return 0;
}}

    strncpy(nav_data, argv[2], FIFO_NAME_MAX_LENGTH - 1);
    printf("%s is the nav_data named channel \n", nav_data);

    create_fifo(nav_data);

srand(time(NULL));

system("clear");
if (state == 1) {
    while (1) {
        //system("clear");
        clock_gettime(CLOCK_REALTIME, &ts);
        char temp_message[64];
        snprintf(temp_message, sizeof(temp_message), "[%ld.%09ld] Temp diviation: %d ", ts.tv_sec, ts.tv_nsec, askTemp());
        write_to_fifo(nav_data, temp_message);
        //askTemp();
        usleep(TIME * 10000);
    }
}
if (state == 2) {
    while (1) {
        //system("clear");
        clock_gettime(CLOCK_REALTIME, &ts);
        char temp_message[64];
        snprintf(temp_message, sizeof(temp_message), "[%ld.%09ld] X, Y, Z: %f, %f, %f ", ts.tv_sec, ts.tv_nsec, askGiro('X', TIME), askGiro('Y', TIME), askGiro('Z', TIME));
        write_to_fifo(nav_data, temp_message);
        //askGiro('X', TIME);
        //askGiro('Y', TIME);
        //askGiro('Z', TIME);
        fflush(stdout);
        usleep(TIME * 10000);
    }
}

return 0;
}
