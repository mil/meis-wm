#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <X11/Xlib.h>
Display *display;
Window root;
int screen;


void event_loop() {
  // Variable Declarations
  XEvent event;
  for (;;) {
    XNextEvent(display, &event);
    if (event.type == MapRequest) {
      //XMapWindow( display, event.xmaprequest.window);
      map_window(&event.xmaprequest.window);
    }
  }
}

Bool create_directory(char * path) {
  struct stat buffer = {0};
  if (stat(path, &buffer) == -1) {
    mkdir(path, 0700);
    return True;
  } 
  return False;
}


Bool map_window(Window * window) {
  char * name;
  fprintf(stderr, "Mapping window");

  XMapWindow(display, *window);
  XSetWindowBorderWidth(display, *window, 10);
  XSetWindowBorder(display, *window, BlackPixel(display, screen)); 


  name = malloc(sizeof(char) * 18);
  sprintf(name, "/tmp/meis/%d", (int) *window);

  if ( create_directory(name)) {
    free(name); return True;
  } else {
    free(name); return False;
  }

 }


int main(int argc, char * argv[]) {
  fprintf(stderr, "Intilaized\n");
    // Create the meis directory if doesn't already exist
  create_directory("/tmp/meis");

  // Open Display, Set Root, and Active Screen
  assert(display = XOpenDisplay(NULL));
  screen = DefaultScreen(display);
  root   = DefaultRootWindow(display);

  // Select SubstructureNotify & SubstructureRedirect
  XSelectInput(display, root, FocusChangeMask | PropertyChangeMask | SubstructureNotifyMask | SubstructureRedirectMask | KeyPressMask | ButtonPressMask);
  XFlush(display);
  fprintf(stderr, "right before the event loop\n");

  // Call event loop to handle FIFO Fds, Inotify Events, and X Events
  event_loop();

  return 0;
}
