#include <Platform/Linux/LinuxWindow.hpp>

#if HYPERENGINE_PLATFORM_LINUX
	#include <Core/Logger.hpp>

	#include<stdio.h>
	#include<stdlib.h>
	#include<X11/X.h>
	#include<X11/Xlib.h>
	#include<GL/gl.h>
	#include<GL/glx.h>
	#include<GL/glu.h>

namespace Platform
{
	LinuxWindow::LinuxWindow(const WindowCreateInfo& createInfo)
	{
		
		Display                 *dpy;
		::Window                  root;
		GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		XVisualInfo             *vi;
		Colormap                cmap;
		XSetWindowAttributes    swa;
		::Window                  win;
		GLXContext              glc;
		XWindowAttributes       gwa;
		XEvent                  xev;
		
		dpy = XOpenDisplay(NULL);
		
		if(dpy == NULL) {
			printf("\n\tcannot connect to X server\n\n");
			exit(0);
		}
		
		root = DefaultRootWindow(dpy);
		
		vi = glXChooseVisual(dpy, 0, att);
		
		if(vi == NULL) {
			printf("\n\tno appropriate visual found\n\n");
			exit(0);
		}
		else {
			printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
		}
		
		
		cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
		
		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask;
		
		win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
		
		XMapWindow(dpy, win);
		XStoreName(dpy, win, "VERY SIMPLE APPLICATION");
		
		glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
		glXMakeCurrent(dpy, win, glc);
		
		glEnable(GL_DEPTH_TEST);
		
		while(1) {
			XNextEvent(dpy, &xev);
			
			if(xev.type == Expose) {
				XGetWindowAttributes(dpy, win, &gwa);
				glViewport(0, 0, gwa.width, gwa.height);
				//DrawAQuad();
				glXSwapBuffers(dpy, win);
			}
			
			else if(xev.type == KeyPress) {
				glXMakeCurrent(dpy, None, NULL);
				glXDestroyContext(dpy, glc);
				XDestroyWindow(dpy, win);
				XCloseDisplay(dpy);
				exit(0);
			}
		} /* this closes while(1) { */
	}
	
	void LinuxWindow::Update()
	{
	
	}
}
#endif
