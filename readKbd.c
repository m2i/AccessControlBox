#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <wiringPi.h>

char remapKeyCode(int scanCode);

static const char * const evval[3] = {
  " RELEASED",
  "   PRESSED",
  " REPEATED",
};

void main() {
  const char* device = "/dev/input/by-path/platform-3f980000.usb-usb-0:1.5:1.0-event-kbd";
  int nready;
  fd_set readfds;
  struct timeval read_timeout;
  struct input_event ev;
  ssize_t n;
  int fd;
  int index = 0;
  char card[15];
  int dead_counter = 0;
  if (wiringPiSetup() == -1) {
    return;
  }
  pinMode(25, OUTPUT);
			
  fd = open(device, O_RDONLY);
  if (fd == -1) {
    printf("Could not open device: %s\n", strerror(errno));
    return;
  }
  
  while (1) {
    
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    read_timeout.tv_usec = 500000;
    read_timeout.tv_sec = 0;
    nready = select(fd+1, &readfds, NULL, NULL, &read_timeout);
    if (nready == -1) {
      //      die?
      continue;
    } else if (nready == 0 && dead_counter > 10) {
      // Do nothing
      dead_counter = 0;
      printf("Output Off\n");
      digitalWrite(25, 0);
      continue;
    } else if (nready == 0) {
      dead_counter++;
      printf("No Input\n");      
    } else {
      //data ready
      n = read(fd, &ev, sizeof(ev));
      dead_counter = 0;
      if (n == (ssize_t)-1) {
	if (errno = EINTR) {
	  continue;
	} else {
	  break;
	}
      } else if (n == 0) {
	continue;
      }else if (n != sizeof(ev)) {
	errno = EIO;
	break;
      }
      if (ev.type == EV_KEY) {
	if (ev.value >= 0 && ev.value <= 2) {
	  printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
	  if (ev.value == 1) {
	    char key = remapKeyCode(ev.code);
	    if (key != '\n') {
	      card[index++] = key;
	    } else {
	      card[index] = 0;
	      printf("%s\n", card);
	      index = 0;
	      // Talk to database
	      digitalWrite(25, 1);
	    } 
	  }
	}
      }
    }
  }
}

char remapKeyCode(int scanCode) {
  if (scanCode >= KEY_1 && scanCode <= KEY_9) {
    return scanCode - KEY_1 + 1+'0';
  } else if (scanCode == KEY_0) {
    return '0';
  } else if (scanCode == KEY_MINUS) {
    return '-';
  } else if (scanCode == KEY_ENTER) {
    return '\n';
  } else {
    return -1;
  }
}
