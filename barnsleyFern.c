#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int Width = 1380;
const int Height=740;
// for translating xy-coordinate into screen coordinate
int pointX(int);
int pointY(int);
//factor for point (since the points are pixel value so it needs to be scaled)
const int factor = 60;
// for star size
enum size {large=5, medium = 3, small = 2, verysmall = 1};

//draw
void BarnsleyFern(Display *, Drawable, GC);
void Star(Display *, Drawable, GC);
void StarSize(Display *, Drawable, GC, int size);

int RoundToInt(float);

int main() {
    srand(time(NULL));
    // Open connection to the X server
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        printf("Unable to open X display\n");
        return 1;
    }

    // Get the default screen
    int screen = DefaultScreen(display);

    // Create a simple window
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 
                                        10, 10, Width, Height, 1, 
                                        BlackPixel(display, screen), 
                                        0xDDDDDD);

    // Select the events you want to listen to
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    // Map the window (make it visible)
    XMapWindow(display, window);

    // Create a graphics context
    GC FractalBg = XCreateGC(display, window, 0, NULL);
    // Set the foreground color
    XSetForeground(display, FractalBg, 0x0e1238);

    //for fractal points
    GC point = XCreateGC(display,window,0,NULL);
    XSetForeground(display, point, 0x20d829);

    //for stars
    GC stars = XCreateGC(display,window,0,NULL);
    XSetForeground(display, stars, 0xffffff);


    // Event loop
    XEvent event;
    while (1) {
        XNextEvent(display, &event);

        // Draw a rectangle(for canvas purpose) and points when the window is exposed
        if (event.type == Expose) {
            XFillRectangle(display, window, FractalBg, 0, 0, Width, Height);
            Star(display, window, stars);
            BarnsleyFern(display, window, point);
        }

        // Exit the loop when a key is pressed
        if (event.type == KeyPress) {
            break;
        }
    }

    // Clean up
    XFreeGC(display, FractalBg);
    XFreeGC(display,point);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}


// these pointX and pointY are to translate 2d xy-coordinate into screen coordinate
int pointX(int x){
    return Width/2 + x;
}
int pointY(int y){
    return Height - y;
}

int RoundToInt(float x){
    return (x>=0) ? (int) (x + .5) : (int)(x -.5);
}


//the Barnsley Fern is given by f(x,y)=|a   b| | x |   | e | 
//                                     |c   d| | y | + | f |
// where    a   |   b       |   c   |   d   |   e   |   f     are the values for f1,f2,f3,f4  i.e fi(x,y) for i= 1 to 4
//f1        0   |   0       |   0   |   .16 |   0   |   0
//f2        .85 |   .04     |   -.04|   .85 |   0   |   1.6
//f3        .20 |   -.26    |   .23 |   .22 |   0   |   1.6
//f4        -.15|   .28     |   .26 |   .24 |   0   |   .44
          

void BarnsleyFern(Display *display, Drawable window, GC point){
    int r;
    float x = 0;
    float y = 0;
    int i = 0;
    float xn = 0;
    float yn = 0;
    while(i < 100000){
        r = rand() % 1000;
        if(r <= 10){
            //f1(x,y)
            xn = 0;
            yn = 0.16*y;
        }
        else if(r <= 860){
            // f2(x,y)
            xn = 0.85*x + 0.04*y;
            yn = -0.04 * x + 0.85 * y + 1.6;
        }
        else if(r <= 930){
            //f3(x,y)
            xn = 0.2*x - 0.26*y;
            yn = 0.23*x + 0.22*y + 1.6;
        }
        else{
            //f4(x,y)
            xn = -0.15*x + 0.28*y;
            yn = 0.26*x + 0.24*y + 0.44;
        }
        x = xn;
        y = yn;
        i++;
        //draw the points with scaling
        XDrawPoint(display, window, point, pointX(RoundToInt(x*factor)), pointY(RoundToInt(y*factor)));    
    }
}

void Star(Display *display, Drawable window, GC stars){
    int starSizeProbability;//5% large, 20% medium, 35% small, 40% very small
    for(int i =0 ; i<300; i++){
        starSizeProbability = rand()%100;
        if(starSizeProbability <=5)
            StarSize(display, window, stars, large);
        else if(starSizeProbability <=25)
            StarSize(display, window, stars, medium);
        else if(starSizeProbability <=60)
            StarSize(display, window, stars, small);
        else
            StarSize(display, window, stars, verysmall);
    }
}

void StarSize(Display *display, Drawable window, GC stars, int size){
    int x = rand()%Width;
    int y = rand()%Height;
    XDrawPoint(display, window, stars, x, y);

    for(int i =1 ; i < size; i++){
        XDrawPoint(display, window, stars, x+i, y);
        XDrawPoint(display, window, stars, x, y+i);
        XDrawPoint(display, window, stars, x-i, y);
        XDrawPoint(display, window, stars, x, y-i);
        XDrawPoint(display, window, stars, x+i, y+i);
        XDrawPoint(display, window, stars, x-i, y-i);
        XDrawPoint(display, window, stars, x+i, y-i);
        XDrawPoint(display, window, stars, x-i, y+i);
        if(i == size){
            XDrawPoint(display, window, stars, x+i+1, y);
            XDrawPoint(display, window, stars, x, y+i+1);
            XDrawPoint(display, window, stars, x-i-1, y);
            XDrawPoint(display, window, stars, x, y-i-1);
        }

    }
}


