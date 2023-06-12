#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include </home/selfdrivingcar/selfdrivingcar/gem-e2_hardware-platform/hello_world/dac_spi/orin_spi.h>

#define MAX_LINE_LENGTH 1024
#define MAX_STEER_POS 1024
#define MAX_STEER_NEG -1024
#define MAX_ACCEL 1024
#define STEER_NEUTRAL 0
#define STEER_CHANGE 100
#define ACCEL_CHANGE 100
// states of the state machine
enum STATES
{
    IDLE,
    TURN_LEFT,  // if drive by wire is finished
    TURN_RIGHT, // if drive by wire is finished
    SLOW_DOWN,
    STOP,
    ACCELERATE,
    END
};

// these are the events that can cause changes to the state machine
// enum EVENTS
// {
//     FRONT_EVENT,
//     BACK_EVENT,
//     HUMAN_EVENT
// };

// this struct will carry the state variable and any data we need to transfer
struct car_struct
{
    enum STATES state;
    int data;
};

int main(void)
{
    // init stuff for state machine
    setup();
    struct car_struct car;
    car.state = IDLE;
    // open pipeline to TOF data, make sure python code is running
    char line[MAX_LINE_LENGTH];
    char line2[MAX_LINE_LENGTH];
    char last_line[MAX_LINE_LENGTH];
    char last_cam[MAX_LINE_LENGTH];
    int TOF_data[3];        // array of TOF data, TOF 0x29 first 0x2C last
    int steering_angle = 0; // pos is right, neg is left, to a max of +/-1024
    int accelerate_val = 0;

    // printf("TRANSITIONING TO IDLE");

    while (1)
    {
        // int ii = 0;
        // while (ii < 10000)
        // {
        //     ii++;
        // }

        // get TOF data, this could easily be a function
        FILE *TOFdata = fopen("TOF_data", "r");     // open pipeline to TOF data
        FILE *Camdata = fopen("cam_data.txt", "r"); // open pipeline camera data
        if (TOFdata == NULL)
        {
            perror("fopen");
            return -1;
        }
        if (Camdata == NULL)
        {
            perror("fopen");
            return -1;
        }

        while (fgets(line, sizeof(line), TOFdata)) // get last line (most recent) TOF data
        {
            strcpy(last_line, line);
        }
        // while (fgets(line2, sizeof(line2), Camdata)) // get last line (most recent) TOF data
        // {
        //     strcpy(last_cam, line2);
        // }

        fgets(line2, sizeof(line2), Camdata);
        strcpy(last_cam, line2);
        int human = atoi(last_cam);

        if (ferror(TOFdata))
        {
            perror("fgets");
            fclose(TOFdata);
            return -1;
        }
        if (ferror(Camdata))
        {
            perror("fgets");
            fclose(Camdata);
            return -1;
        }
        // printf("%s\n",last_cam);

        char *token = strtok(last_line, " "); // tokenize the string, the first one is 0x29, the last 0x2C
        int i = 0;
        while (token)
        {
            TOF_data[i] = atoi(token); // need to turn the strings to ints
            // printf("TOF %d: %d\n", i, TOF_data[i]);
            i++;
            token = strtok(NULL, " ");
        }

        fclose(TOFdata); // close the pipe, we don't need it after this
        fclose(Camdata);

        // actual car state machine
        switch (car.state)
        {
        case (IDLE):

            // state transitions
            if (TOF_data[0] < 1000)
            {
                printf("TRANSITIONING TO TURN_LEFT\n");
                car.state = TURN_LEFT;
            }
            else if (TOF_data[1] < 1000)
            {
                printf("TRANSITIONING TO TURN_LEFT\n");
                car.state = TURN_LEFT;
            }
            else if ((TOF_data[2] < 1000) || (TOF_data[3] < 1000))
            {
                printf("TRANSITIONING TO ACCELERATE\n");
                // printf("TRANSITIONING TO TURN_LEFT\n");
                // printf("TRANSITIONING TO TURN_RIGHT\n");
                car.state = ACCELERATE;
            }

            // if human, then change to stop. This has highest priority
            if (human)
            {
                steering_angle = STEER_NEUTRAL;
                printf("Steering angle: %d\n", steering_angle);
                printf("TRANSITIONING TO STOP\n");
                car.state = STOP;
            }
            if ((TOF_data[0] < 1000) && (TOF_data[1] < 1000) && (TOF_data[2] < 1000) && (TOF_data[3] < 1000))
            {
                printf("TRANSITION TO END\n");
                car.state = END;
            }
            break;

        case (ACCELERATE): // since the drive by wire doesn't respond to the car, the function should send back the speed it's trying to set the car too
            // accelerate the car by some amount as defined by the spi function
            accelerate_val += ACCEL_CHANGE;
            if (accelerate_val > MAX_ACCEL)
            {
                accelerate_val = MAX_ACCEL;
            }
            send_spi_msg(accelerate_val);
            printf("Acceleration val: %d\r\n", accelerate_val);

            // state transitions
            if (TOF_data[0] < 1000)
            {
                printf("TRANSITIONING TO TURN_LEFT\n");
                car.state = TURN_LEFT;
            }
            else if (TOF_data[1] < 1000)
            {
                printf("TRANSITIONING TO TURN_LEFT\n");
                car.state = TURN_LEFT;
            }
            else if ((TOF_data[0] > 1000) && (TOF_data[1] > 1000) && (TOF_data[2] > 1000) && (TOF_data[3] > 1000))
            {
                printf("TRANSITIONING TO IDLE\n");
                car.state = IDLE;
            }
            // if human, then change to stop. This has highest priority.
            if (human)
            {
                printf("TRANSITIONING TO STOP\n");
                steering_angle = STEER_NEUTRAL;
                accelerate_val = 0;
                send_spi_msg(accelerate_val);
                printf("Steering angle: %d\n", steering_angle);
                printf("Acceleration val: %d\r\n", accelerate_val);
                car.state = STOP;
            }
            if ((TOF_data[0] < 1000) && (TOF_data[1] < 1000) && (TOF_data[2] < 1000) && (TOF_data[3] < 1000)))
            {
                printf("TRANSITIONING TO END\n");
                car.state = END;
            }
            break;

        case (TURN_LEFT):
            // change steering angle of the car by some amount as defined by the spi function
            steering_angle = steering_angle - STEER_CHANGE;

            if (steering_angle < MAX_STEER_NEG)
            {
                steering_angle = MAX_STEER_NEG;
            }
            printf("Steering angle: %d\n", steering_angle);

            // state transitions
            if (TOF_data[0] < 1000)
            {
               printf("TRANSITIONING TO TURN_RIGHT\n");
               car.state = TURN_RIGHT;
            }
            if (TOF_data[1] < 1000)
            {
                printf("TRANSITIONING TO ACCELERATE\n");
                car.state = ACCELERATE;
            }
            else if ((TOF_data[0] > 1000) && (TOF_data[1] > 1000) && (TOF_data[2] > 1000) && (TOF_data[3] > 1000))
            {
                printf("TRANSITIONING TO IDLE\n");
                car.state = IDLE;
            }
            if (human)
            {
                printf("TRANSITIONING TO STOP\n");
                steering_angle = STEER_NEUTRAL;
                send_spi_msg(accelerate_val);
                printf("Steering angle: %d\n", steering_angle);
                printf("Acceleration val: %d\r\n", accelerate_val);
                car.state = STOP;
            }
            if ((TOF_data[0] < 1000) && (TOF_data[1] < 1000) && (TOF_data[2] < 1000) && (TOF_data[3] < 1000))
            {
                printf("TRANSITIONING TO END\n");
                car.state = END;
            }
            break;

        case (TURN_RIGHT):
            // change steering angle of the car by some amount as defined by the spi function

            steering_angle = steering_angle + STEER_CHANGE;

            if (steering_angle > MAX_STEER_POS)
            {
                steering_angle = MAX_STEER_POS;
            }
            printf("Steering angle: %d\n", steering_angle);

            // state transitions
            if (TOF_data[1] < 1000)
            {
                printf("TRANSITIONING TO TURN_LEFT\n");
                car.state = TURN_LEFT;
            }
            else if ((TOF_data[2] < 1000) || (TOF_data[3] < 1000))
            {
                printf("TRANSITIONING TO ACCELERATE\n");
                car.state = ACCELERATE;
            }
            else if ((TOF_data[0] > 1000) && (TOF_data[1] > 1000) && (TOF_data[2] > 1000) && (TOF_data[3] > 1000))
            {
                printf("TRANSITIONING TO IDLE\n");
                car.state = IDLE;
            }
            if (human)
            {
                printf("TRANSITIONING TO STOP\n");
                steering_angle = STEER_NEUTRAL;
                send_spi_msg(accelerate_val);
                printf("Steering angle: %d\n", steering_angle);
                printf("Acceleration val: %d\r\n", accelerate_val);
            }
            break;

        case (STOP):
            // set acceleration straight to 0, set steering angle to 0 in transition and maintain it here
            steering_angle = STEER_NEUTRAL;
            accelerate_val = 0;
            send_spi_msg(accelerate_val);
            // if no human event, transition to idle
            if (!human)
            {
                printf("TRANSITIONING TO IDLE\n");
                car.state = IDLE;
            }
            break;

        case (END):
            close_spi();
            return 0;
            break;
            // printf("\n");
        }
    }
}
