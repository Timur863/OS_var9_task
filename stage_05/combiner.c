#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>

#define NAV_MODULE_PIPE "/tmp/nav_data"
#define RANGEFINDER_PIPE "/tmp/range_finder"
ssize_t read_line(int fd, char *buffer, size_t buf_size);

void printCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t seconds = tv.tv_sec;
    struct tm* local_time = localtime(&seconds);
    printf("Время %02d:%02d:%02d\n", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
}

int main() {
    mkfifo(NAV_MODULE_PIPE, 0666);
    mkfifo(RANGEFINDER_PIPE, 0666);

    int pipe_fd1 = open(NAV_MODULE_PIPE, O_RDONLY | O_NONBLOCK);
    int pipe_fd2 = open(RANGEFINDER_PIPE, O_RDONLY | O_NONBLOCK);

    if (pipe_fd1 < 0 || pipe_fd2 < 0) {
        perror("Error opening named pipes");
        exit(1);
    }

    struct pollfd fds[2];
    fds[0].fd = pipe_fd1;
    fds[0].events = POLLIN;
    fds[1].fd = pipe_fd2;
    fds[1].events = POLLIN;

    char buffer[256];
    ssize_t bytes_read;

    while (1) {
        int ret = poll(fds, 2, -1);

        if (ret < 0) {
            perror("Error in poll");
            exit(1);
        }

        

        if (fds[0].revents & POLLIN) {
            bytes_read = read_line(pipe_fd1, buffer, sizeof(buffer));
            // printf("Read from pipe1 bytes: %d",bytes_read)
            if (bytes_read > 0) {
                printCurrentTime();
                printf("%s\n", buffer);
            }
        }

        if (fds[1].revents & POLLIN) {
            bytes_read = read_line(pipe_fd2, buffer, sizeof(buffer));
            if (bytes_read > 1) {
                //printf("bytes: %d\n", bytes_read);
                printf("Высота: %s\n", buffer);
            }
        }
    }

    close(pipe_fd1);
    close(pipe_fd2);

    unlink(NAV_MODULE_PIPE);
    unlink(RANGEFINDER_PIPE);

    return 0;
}

ssize_t read_line(int fd, char *buffer, size_t buf_size) {
    ssize_t total_bytes_read = 0;
    ssize_t bytes_read;
    char ch;

    while (total_bytes_read < buf_size - 1) {
        bytes_read = read(fd, &ch, 1);

        if (bytes_read < 0) {
            // Ошибка чтения
            return -1;
        } else if (bytes_read == 0) {
            // Конец файла или потока данных
            break;
        }

        buffer[total_bytes_read++] = ch;

        if (ch == '\n') {
            // Конец строки
            break;
        }
    }

    // Завершаем строку нулевым символом
    buffer[total_bytes_read] = '\0';

    return total_bytes_read;
}
