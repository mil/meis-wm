#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <X11/Xlib.h>

Bool createDirectory(char * path) {
  struct stat buffer = {0};
  if (stat(path, &buffer) == -1) {
    mkdir(path, 0700);
    return True;
  } 
  return False;
}


int main(void) {
  char * name;
  Display * display;
  Window root;
  XEvent event;


  createDirectory("/tmp/meis");

  if (!(display = XOpenDisplay(0x0))) return 1;
  root = DefaultRootWindow(display);

  XSelectInput(display, root, FocusChangeMask | PropertyChangeMask | SubstructureNotifyMask | SubstructureRedirectMask | KeyPressMask | ButtonPressMask);

  name = malloc(sizeof(char) * 18);

  for(;;) {
    XNextEvent(display, &event);

    if (event.type == MapRequest) {
      sprintf(name, "/tmp/meis/%d", (int) event.xmaprequest.window);
      createDirectory(name);
      XMapWindow(display, event.xmaprequest.window);
    }
  }

  return 0;
}

