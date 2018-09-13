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
  struct input_event ev;
  ssize_t n;
  int fd;
  int index = 0;
  char card[15];
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
    n = read(fd, &ev, sizeof(ev));
    if (n == (ssize_t)-1) {
      if (errno = EINTR) {
	continue;
      } else {
	break;
      }
    } else if (n == 0) {
      digitalWrite(25, 0);
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
