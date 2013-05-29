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
      map_window(&event.xmaprequest.window);
    }
  }
}
void write_file(char * path, char * file, int content) {
  char fp[1000];
  FILE *f;

  sprintf(fp, "%s/%s\0", path, file);
  f = fopen(fp, "w+");
  fprintf(f, "%d", content);
  fclose(f);
}

void setup_directory (char * path) {
  if (!create_directory(path)) {
    char * syscall;
    sprintf(syscall, "rm -rf %s\0", path);
    system(syscall);
    create_directory(path);
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
  char * path;

  XMapWindow(display, *window);
  XSetWindowBorderWidth(display, *window, 10);
  XSetWindowBorder(display, *window, BlackPixel(display, screen)); 


  sprintf(path, "/tmp/meis/%d\0", (int) *window);
  create_directory(path);

  XWindowAttributes *window_attributes;
  XGetWindowAttributes(display, *window, window_attributes);

  write_file(path, "x", window_attributes -> x);
  write_file(path, "y", window_attributes -> y);
  write_file(path, "width", window_attributes -> width);
  write_file(path, "height", window_attributes -> height);
}


int main(int argc, char * argv[]) {
  fprintf(stderr, "Intilaized\n");
    // Create the meis directory if doesn't already exist
  setup_directory("/tmp/meis");

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
