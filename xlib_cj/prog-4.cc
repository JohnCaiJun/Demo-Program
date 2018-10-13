/**********************************************
* Copyright (c)2018,caijun
* Filename: fullscreen_window
* Description: 应用程序窗口全屏
* Author: Cai Jun <johncai.caijun.cn@gmail.com>
* Time: 14:42 星期六 2018/10/13
***********************************************/

#ifndef FULLSCREEN_WINDOW
#define FULLSCREEN_WINDOW

// get active window
int xdo_get_active_window(Display *display, Window *window_ret) {
  Atom type;
  int size;
  long nitems;
  unsigned char *data;
  Atom request;
  Window root;

  request = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
  root = XDefaultRootWindow(display);
  data = xdo_get_window_property_by_atom(display, root, request, &nitems, &type, &size);

  if (nitems > 0) {
    *window_ret = *((Window*)data);
  } else {
    *window_ret = 0;
  }
  free(data);

  return 0;
}

// send event fullscreen
int send_event_fullscreen(Display *display, Window window)
{
    XEvent event;
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;

    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(display, "_NET_WM_STATE", False);
    event.xclient.window = window;
    event.xclient.format = 32;
    event.xclient.data.l[0] = _NET_WM_STATE_ADD;
    event.xclient.data.l[1] = XInternAtom(display,"_NET_WM_STATE_FULLSCREEN",False);
    event.xclient.data.l[2] = 0;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;
    
    if (XSendEvent(display, DefaultRootWindow(display), False, mask, &event)) {
        return EXIT_SUCCESS;
    }
    else {
        fprintf(stderr, "Cannot send %s event.\n", "_NET_WM_STATE");
        return EXIT_FAILURE;
    }
}

// get active window
void get_active_window_main(int argc, char** argv)
{
	Window window = 0;
	auto display = XOpenDisplay(nullptr);
	auto root_window = DefaultRootWindow(display);
	
	// get screen count
	cout << "\nscreen_coutn : "<<XScreenCount(display)<<endl;

	// get active window
	xdo_get_active_window(display,&window);

	cout << "get_active_window :" << window << endl;
	// send event fullscreen
	send_event_fullscreen(display,window);
	XMoveWindow(display,window,0,0);

	XCloseDisplay(display);
}


// main
int main(int argc, char** argv)
{
	get_active_window_main(argc,argv);
	return 0;
}

#endif
