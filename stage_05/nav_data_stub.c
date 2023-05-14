#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#define RAD_TO_DEG (180.0 / M_PI)  // Коэффициент для перевода радиан в градусы

#define FIFO_NAME_MAX_LENGTH 255

#define MIN_VAL 500
#define SENSITIVITY_250 8.75f
#define SENSITIVITY_500 17.5f
#define SENSITIVITY_2000 70.0f

#define MAGIC_CONST_X (SENSITIVITY_2000 * 2)
#define MAGIC_CONST_Y (SENSITIVITY_2000 * 2)
#define MAGIC_CONST_Z (SENSITIVITY_2000 * 2)

#define TIME 10.0

char nav_data[FIFO_NAME_MAX_LENGTH] = {0};

uint8_t xSign, ySign, zSign;
float xPosition = 0;
float yPosition = 0;
float zPosition = 0;

void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("Received signal SIGINT. Exiting.\n");
        write_to_fifo(nav_data, "nav_data завершила работу");
        exit(0);
    }
}

double get_direction(double pitch, double yaw, double roll) {
    double x = cos(yaw) * cos(pitch);
    double y = sin(yaw) * cos(pitch);
    double z = sin(pitch);

    double angle = atan2(y, x);  // Угол между вектором направления движения и осью X в плоскости XY
    double direction = angle * RAD_TO_DEG;  // Перевод угла из радианов в градусы

    return direction;
}

float askGiroOrientation(char pos, double time)
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

    float xAngle, yAngle, zAngle;
    if (pos == 'X') {
        if (xSign == 0)
            xAngle += MAGIC_CONST_X * gyroValue * (time / 1000) / 1000;
        else
            xAngle -= MAGIC_CONST_X * gyroValue * (time / 1000) / 1000;
        printf("X angle : %lf\n", xAngle);
        return xAngle;
    } else if (pos == 'Y') {
        if (ySign == 0)
            yAngle = MAGIC_CONST_Y * gyroValue * (time / 1000) / 1000;
        else
            yAngle -= MAGIC_CONST_Y * gyroValue * (time / 1000) / 1000;
        printf("Y angle : %lf\n", yAngle);
        return yAngle;
    } else if (pos == 'Z') {
        if (zSign == 0)
     zAngle = MAGIC_CONST_Z * gyroValue * (time / 1000) / 1000;
 else
     zAngle -= MAGIC_CONST_Z * gyroValue * (time / 1000) / 1000;
 printf("Z angle : %lf\n", zAngle);
 return zAngle;
}}

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

    signal(SIGINT, handle_signal);

    struct timespec ts;

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

    float x, y, z;
    double direction_angle;
if (state == 2) {
    while (1) {
        //system("clear");
        clock_gettime(CLOCK_REALTIME, &ts);
        char temp_message0[128];
        x = askGiroOrientation('X', TIME);
        y = askGiroOrientation('Y', TIME);
        z = askGiroOrientation('Z', TIME);
        direction_angle = get_direction(x, y, z);
        snprintf(temp_message0, sizeof(temp_message0), "Углы ориентации: %f, %f, %f \nНаправление движения: %f", x, y, z, direction_angle);
        write_to_fifo(nav_data, temp_message0);

        //char temp_message[64];
        //snprintf(temp_message, sizeof(temp_message), "[%ld.%09ld] X, Y, Z: %f, %f, %f ", ts.tv_sec, ts.tv_nsec, askGiro('X', TIME), askGiro('Y', TIME), askGiro('Z', TIME));
        //write_to_fifo(nav_data, temp_message);
        //askGiro('X', TIME);
        //askGiro('Y', TIME);
        //askGiro('Z', TIME);
        fflush(stdout);
        usleep(TIME * 10000);
    }
}

return 0;
}
