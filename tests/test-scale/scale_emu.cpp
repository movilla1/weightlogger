#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <iostream>
#include <cmath>

using namespace std;

int main(int argc, char **argv)
{
  int USB;
  cout << "Using port" << argv[1] << endl;
  USB = open(argv[1], O_RDWR | O_NONBLOCK | O_NDELAY);
  char data[][7] = {
    {'0', '0', '0', '6', '9', '0', 0x1C},
    {'0', '0', '0', '6', '9', '0', 0x1C},
    {'0', '0', '0', '6', '8', '0', 0x1C},
    {'0', '0', '0', '6', '9', '5', 0x1C},
    {'0', '0', '0', '7', '0', '0', 0x1B},
    {'0', '0', '0', '6', '9', '0', 0x1B},
    {'0', '0', '0', '6', '9', '0', 0x1C},
    {'0', '0', '0', '6', '9', '0', 0x1C},
    {'0', '0', '0', '7', '0', '0', 0x1C},
    {'0', '0', '0', '6', '9', '0', 0x1B},
  };
  if (USB < 0)  {
    cout << "Please provide the port to use" << endl;
    cout << errno << endl;
    exit(1);
  }
  struct termios tty;
  memset(&tty, 0, sizeof tty);

  /* Error Handling */
  if (tcgetattr(USB, &tty) != 0) {
    cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
  }

  /* Set Baud Rate */
  cfsetospeed(&tty, B4800);
  cfsetispeed(&tty, B4800);
  /* Setting other Port Stuff */
  tty.c_cflag &= ~PARENB; // Make 8n1
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~CRTSCTS; // no flow control
  tty.c_lflag = 0;         // no signaling chars, no echo, no canonical processing
  tty.c_oflag = 0;         // no remapping, no delays
  tty.c_cc[VMIN] = 0;      // read doesn't block
  tty.c_cc[VTIME] = 5;     // 0.5 seconds read timeout

  tty.c_cflag |= CREAD | CLOCAL;                  // turn on READ & ignore ctrl lines
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // turn off s/w flow ctrl
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
  tty.c_oflag &= ~OPOST;                          // make raw

  /* Flush Port, then applies attributes */
  tcflush(USB, TCIFLUSH);

  if (tcsetattr(USB, TCSANOW, &tty) != 0) {
    cout << "Error " << errno << " from tcsetattr" << endl;
  }
  int pos;
  while (true)
  {
    sleep(1);
    cout << "Sending ";
    pos = (int)floor((rand()*1.0 / RAND_MAX) * 10);
    cout << " bytes at pos " << pos << " ";
    for (int x = 0; x < 7; x++)
    {
      cout << data[pos][x];
      write(USB, &data[pos][x], 1);
    }
    cout << endl;
  }
}