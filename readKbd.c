#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#define CARD_LENGTH 12

char remapKeyCode(int scanCode);

static const char * const evval[3] = {
  " RELEASED",
  "   PRESSED",
  " REPEATED",
};

void main() {
  const char* device = "/dev/input/by-id/usb-RFIDeas_USB_Keyboard-event-kbd";
  //const char* device = "/dev/input/by-id/usb-Dell_Dell_QuietKey_Keyboard-event-kbd";
  const char* fifo_name = "/tmp/cardIDs.fifo";
  struct timeval read_timeout;
  struct input_event ev;
  ssize_t n;
  int fd_kbd, fd_fifo;
  int index = 0;
  char card[CARD_LENGTH+1];
  
  if(mkfifo(fifo_name, 0666)) {
    printf("Could not make FIFO: %s\n", strerror(errno));
  }
  fd_fifo = open(fifo_name, O_WRONLY);
  if (fd_fifo < 0) {
    printf("Could not open FIFO: %s\n", strerror(errno));
  }
  fd_kbd = open(device, O_RDONLY);
  if (fd_kbd == -1) {
    printf("Could not open device: %s\n", strerror(errno));
    return;
  }
  card[CARD_LENGTH] = 0;
  while (1) {
    n = read(fd_kbd, &ev, sizeof(ev));
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
	if (ev.value == 1) {
	  char key = remapKeyCode(ev.code);
	  printf("%c,", key);
	  if (key != '\n') {
	    if (index >= CARD_LENGTH) {
	      index = 0;
	    } else {
	      card[index++] = key;
	    }
	  } else if (index == CARD_LENGTH) {
	    printf("%s\n", card);
	    write(fd_fifo, card, CARD_LENGTH);
	    write(fd_fifo, "\n", 1);
	    fsync(fd_fifo);
	    index = 0;
	  } else {
	    // too few characters
	    printf("only %d characters recieved\n", index);
	    index = 0;
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
