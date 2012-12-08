#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <X11/Xlib.h>

void eventLoop(Display *display) {
  // Variable Declarations
  XEvent event;
  int i;
  int fds[100];
  int fdsLast; // Where to add the next FD in fds[]
  int bufferSize;
  char buffer;
  fd_set descriptors;


  // Setup X Fd
  fds[0]    = ConnectionNumber(display);
  fdsLast   = 0; 


  for(;;) {
    // Add file descriptors to set
    FD_ZERO(&descriptors);
    for (i = 0; i <= fdsLast; i++) {
      FD_SET(fds[i], &descriptors);
      fprintf(stderr, "Adding to fd set %d\n", i);
    }

    // Select from the last FD in the set +1 
    if (select(fds[i], &descriptors, NULL, NULL, NULL)) {

      /* Check xFD */
      if (FD_ISSET(fds[0], &descriptors)) {
        while (XPending(display) > 0) {
          XNextEvent(display, &event);
          if (event.type == MapRequest) {
            mapWindowToFilesystem(event.xmaprequest.window);
          }
        }
      } 

      /* Handle Fifo Fds */
      for (i = 1; i <= fdsLast; i++) {
        if (
            FD_ISSET(fds[i], &descriptors) && 
            (bufferSize = read(fds[i], buffer, sizeof(buffer)) > 0)) {
          //buffer[bufferSize] = '\0';
        }
      }
    }

  }

  // Clean up Fdsdd 
  for (i = 0; i <= (sizeof(fds) / sizeof(fds[0])); i++)
      close(fds[i]);

}

Bool createDirectory(char * path) {
  struct stat buffer = {0};
  if (stat(path, &buffer) == -1) {
    mkdir(path, 0700);
    return True;
  } 
  return False;
}

Bool makeFifo(char * path) {
  if (mkfifo(path, S_IRWXU) == 0) {
    return True;
  } else {
    return False;
  }
}

Bool mapWindowToFilesystem(Window * window) {
  char * name;

  name = malloc(sizeof(char) * 18);
  sprintf(name, "/tmp/meis/%d", (int) window);

  if ( createDirectory(name)) {
    free(name);
    return True;
  } else {
    free(name);
    return False;
  }
}

Bool mapFromFilesystem(char * path, int display) {

  /*
     XMapWindow(display, window);
     XSetWindowBorderWidth(display, event.xmaprequest.window, 10);
     XSetWindowBorder(display, 
     event.xmaprequest.window, 
     BlackPixel(display, activeScreen));
     */
}


int main(int argc, char * argv[]) {
  Display *display;
  int screen;
  Window root;

  // Create the meis directory if doesn't already exist
  createDirectory("/tmp/meis");

  // Open Display, Set Root, and Active Screen
  assert(display = XOpenDisplay(NULL));
  screen = DefaultScreen(display);
  root   = DefaultRootWindow(display);

  // Select SubstructureNotify & SubstructureRedirect
  XSelectInput(display, root, FocusChangeMask | 
      PropertyChangeMask | SubstructureNotifyMask | 
      SubstructureRedirectMask | KeyPressMask | ButtonPressMask);
  XFlush(display);

  // Call event loop to handle FIFO Fds, Inotify Events, and X Events
  eventLoop(display);

  return 0;
}
