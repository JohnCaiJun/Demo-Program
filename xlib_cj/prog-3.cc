/**********************************************
* Copyright (c)2018,caijun
* Filename:
* Description: display window list
* Author: Cai Jun <johncai.caijun.cn@gmail.com>
* Time: 
***********************************************/

#ifndef PROG_3_CC

#define PROG_3_CC
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <iostream>

#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <regex.h>
#include <unistd.h>

using namespace std;

#define _NET_WM_STATE_ADD           1    /* add/set property */


static int compile_re(const char *pattern, regex_t *re) {
  int ret;
  if (pattern == NULL) {
    regcomp(re, "^$", REG_EXTENDED | REG_ICASE);
    return True;
  }

  ret = regcomp(re, pattern, REG_EXTENDED | REG_ICASE);
  if (ret != 0) {
    fprintf(stderr, "Failed to compile regex (return code %d): '%s'\n", ret, pattern);
    return False;
  }
  return True;
} /* int compile_re */

int match_window_name(Display *dpy, Window window, regex_t *re)
{
	int i;
	int count=0;
	char **list = NULL;
	XTextProperty tp;

	XGetWMName(dpy,window,&tp);

	if (tp.nitems > 0) {
	  //XmbTextPropertyToTextList(xdo->xdpy, &tp, &list, &count);
	  Xutf8TextPropertyToTextList(dpy, &tp, &list, &count);
	  for (i = 0; i < count; i++) {
	    if (regexec(re, list[i], 0, NULL, 0) == 0) {
	      XFreeStringList(list);
	      XFree(tp.value);
	      return True;
	    }
	  }
	} else {
	  /* Treat windows with no names as empty strings */
	  if (regexec(re, "", 0, NULL, 0) == 0) {
	    XFreeStringList(list);
	    XFree(tp.value);
	    return True;
	  }
	}
	XFreeStringList(list);
	XFree(tp.value);
	return False;
} /* 匹配窗口 */


/* Arbitrary window property retrieval
 * slightly modified version from xprop.c from Xorg */
unsigned char *xdo_get_window_property_by_atom(Display *display, Window window, Atom atom,
                                            long *nitems, Atom *type, int *size) {
  Atom actual_type;
  int actual_format;
  unsigned long _nitems;
  /*unsigned long nbytes;*/
  unsigned long bytes_after; /* unused */
  unsigned char *prop;
  int status;

  status = XGetWindowProperty(display, window, atom, 0, (~0L),
                              False, AnyPropertyType, &actual_type,
                              &actual_format, &_nitems, &bytes_after,
                              &prop);
  if (status == BadWindow) {
    fprintf(stderr, "window id # 0x%lx does not exists!", window);
    return NULL;
  } if (status != Success) {
    fprintf(stderr, "XGetWindowProperty failed!");
    return NULL;
  }

  /*
   *if (actual_format == 32)
   *  nbytes = sizeof(long);
   *else if (actual_format == 16)
   *  nbytes = sizeof(short);
   *else if (actual_format == 8)
   *  nbytes = 1;
   *else if (actual_format == 0)
   *  nbytes = 0;
   */

  if (nitems != NULL) {
    *nitems = _nitems;
  }

  if (type != NULL) {
    *type = actual_type;
  }

  if (size != NULL) {
    *size = actual_format;
  }
  return prop;
}


// get active window
int xdo_get_active_window(Display *display, Window *window_ret) {
  Atom type;
  int size;
  long nitems;
  unsigned char *data;
  Atom request;
  Window root;

//  if (_xdo_ewmh_is_supported(xdo, "_NET_ACTIVE_WINDOW") == False) {
//    fprintf(stderr,
//            "Your windowmanager claims not to support _NET_ACTIVE_WINDOW, "
//            "so the attempt to query the active window aborted.\n");
//    return XDO_ERROR;
//  }

  request = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
  root = XDefaultRootWindow(display);
  data = xdo_get_window_property_by_atom(display, root, request, &nitems, &type, &size);

  if (nitems > 0) {
    *window_ret = *((Window*)data);
  } else {
    *window_ret = 0;
  }
  free(data);

  //return _is_success("XGetWindowProperty[_NET_ACTIVE_WINDOW]",
   //                  *window_ret == 0, xdo);
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
	for ( int i = 0; i < argc; i++){
		cout << "arg[" << i << "] = " << argv[i] << endl;
	}

	
	Window window = 0;
	auto display = XOpenDisplay(nullptr);
	auto root_window = DefaultRootWindow(display);
	
	// get screen count
	cout << "\nscreen_coutn : "<<XScreenCount(display)<<endl;


	xdo_get_active_window(display,&window);

	cout << "get_active_window :" << window << endl;
//	string result;
//	result.clean();
//	result = to_string(window);
	
	// fullscreen
//	Atom wm_state = XInternAtom(display,"_NET_WM_STATE",true);
//	Atom wm_fullscreen = XInternAtom (display, "_NET_WM_STATE_FULLSCREEN",true);
	
//	XChangeProperty(display,window,wm_state,XA_ATOM,32,
//					PropModeReplace, (unsigned char *)&wm_fullscreen,1);

	//XMoveWindow(display,window,0,0);
	
	// test code fullscreen	
//	XChangeProperty(display,54525999,wm_state,XA_ATOM,32,
//					PropModeReplace, (unsigned char *)&wm_fullscreen,1);


	// send event fullscreen
	send_event_fullscreen(display,window);
	XMoveWindow(display,window,0,0);

	XCloseDisplay(display);
}

// 指定屏幕显示（需要安装xdotool和wmctrl工具）
//void Tool::choiceMonitor(const string& monitor){
//	string result;
//	result.clear();
//	string str_wmctrl="wmctrl -r x2go -badd,fullscreen";
//		
//	const char *search_window_name = m_xSession.m_strSession.c_str();
//
//	// regex_t name
//	regex_t name_re;
//
//	// compile regex window name
//	
//	compile_re(search_window_name,&name_re);
//
//
//	auto display = XOpenDisplay(nullptr);
//	auto root_window = DefaultRootWindow(display);
//
//	#ifndef NDEBUG
//	cout << "root_window : " << root_window << endl;
//	#endif 
//	
//	Window root_return, parent_return;
//	Window * child_list = nullptr;
//	unsigned int child_num = 0;
//	XQueryTree(display, root_window, &root_return, &parent_return, &child_list, &child_num);
//	
//	for(unsigned int i = 0; i < child_num; ++i) {
//		auto window = child_list[i];
//
//		if ( match_window_name(display,window,&name_re) ) {
//		
//			cout << "\n\nsession window handle :" << window << endl;
//			XRaiseWindow(display,window);
//			if (monitor == "0"){
//				
//				// 移动主屏
//				cout << "\n\nmonitor=0" << endl;
//			
//	//			XMoveWindow(display,window,100,100);
//			
//	//			XMapWindow(display,window);
//
//	//			XFlush(display);
//	
//				//execute_cmd(str_wmctrl.c_str());							// 全屏
//			} else if (monitor == "1"){
//				cout << "\n\nmonitor=1" << endl;
//				string resolution = getMainResolution();        			// 获取当前主屏幕的分辨率
//				int i = resolution.find_first_of("x");						// 获取横坐标
//				resolution = resolution.substr(0,i);
//				int num = atoi(resolution.c_str());
//				++num;
//				#ifndef NDEBUG
//				cout<<"\n\nresolution="<<num<<endl;
//				#endif
//				
//				// 移到扩展屏
//	//			XMoveWindow(display,window,num,0);
//
//	//			XMapWindow(display,window);
//
//	//			XFlush(display);
//
//				//execute_cmd(str_wmctrl.c_str());                            // 全屏
//			} else {
//				cerr<<"no this monitor"<<endl;
//				exit(-1);
//			}
//			break;
//		
//		
//		}
//
//					
//	}
//	XFree(child_list);
//	XCloseDisplay(display);
//}
// search window in name 
void search_window_name_main(int argc, char** argv)
{
	for ( int i = 0; i < argc; i++){
		cout << "arg[" << i << "] = " << argv[i] << endl;
	}

	char *search_window_name = argv[1];

	// regex_t name

	regex_t name_re;
	
	// compile regex window name
	
	compile_re(search_window_name,&name_re);


	auto display = XOpenDisplay(nullptr);
	auto root_window = DefaultRootWindow(display);


	#ifndef NDEBUG
	cout << "root_window : " << root_window << endl;
	#endif 
	
	Window root_return, parent_return;
	Window * child_list = nullptr;
	unsigned int child_num = 0;
	XQueryTree(display, root_window, &root_return, &parent_return, &child_list, &child_num);
	
	char **window_name_return;
	for(unsigned int i = 0; i < child_num; ++i) {
		auto window = child_list[i];

		// match window in name
		
		if ( match_window_name(display,window,&name_re) ) {
			cout << "match window : " << window << endl;	
	
		// get window name
		//XFetchName(display,window,window_name_return);
	
		//cout << "window_name_return:" << window_name_return << endl;
		
		//XFree(window_name_return);
//			XMoveResizeWindow(display,window,0,0,300,300);
//
//
//			XMapWindow(display,window);
//
//			XFlush(display);

	//		sleep(10);

		}

		


	
	//	XWindowAttributes attrs;
	//	XGetWindowAttributes(display, window, &attrs);
	//	
	//	std::cout << "#" << i <<":" << "(" << attrs.width << ", " << attrs.height << ")" << std::endl;
	}
	XFree(child_list);
	XCloseDisplay(display);

}

int main(int argc, char** argv)
{
	get_active_window_main(argc,argv);
	
	return 0;
}

#endif
