////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//



#include "Point.h"
#include "Cloud.h"

#include <iostream>
#include <math.h>
#include <algorithm>
#include <time.h>


#define WIDTH 100
#define HEIGHT 100
#define FACTOR 100
#define THICKNESS 0.005

namespace octet {
  /// Scene containing a box with octet.
  class FortunesVoronoi : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;

	cloud::Sites * sites;

	




  public:
    /// this is called when we construct the class before everything is initialised.
    FortunesVoronoi(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
     app_scene =  new visual_scene();
     
	  sites = new ::cloud::Sites();
	/*  srand(time(NULL));

	  for (int i = 0; i < 5; i++)
	  {
		  sites->push_back(new Point((FACTOR - 2 * THICKNESS) * (double)rand() / (double)RAND_MAX, (FACTOR - 2 * THICKNESS) * (double)rand() / (double)RAND_MAX));
	  }*/
	  //sites->push_back(new Point())
	  //*******************
	  sites->push_back(new Point((FACTOR - 2 * THICKNESS)*(double)0.5 + THICKNESS, (FACTOR - 2 * THICKNESS)*(double)0.9 + THICKNESS));
	  sites->push_back(new Point((FACTOR - 2 * THICKNESS)*(double)0.8 + THICKNESS, (FACTOR - 2 * THICKNESS)*(double)0.7 + THICKNESS));
	  sites->push_back(new Point((FACTOR - 2 * THICKNESS)*(double)0.45 + THICKNESS, (FACTOR - 2 * THICKNESS)*(double)0.5 + THICKNESS));
	  sites->push_back(new Point((FACTOR - 2 * THICKNESS)*(double)0.7 + THICKNESS, (FACTOR - 2 * THICKNESS)*(double)0.4 + THICKNESS));
	  sites->push_back(new Point((FACTOR - 2 * THICKNESS)*(double)0.3 + THICKNESS, (FACTOR - 2 * THICKNESS)*(double)0.2 + THICKNESS));

	  

	  }


  void drawSomething()
  {
	  glClearColor(0, 0, 0, 1);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	  for (::cloud::Sites::iterator i = sites->begin(); i != sites->end(); ++i)
	  {
		  glColor3f(1, 1, 0);
		//  glEnable(GL_POINT_SMOOTH);
		  glBegin(GL_QUADS);
		  //glPointSize(5.0f);
		  //glVertex2f((*i)->x, (*i)->y);
		  //std::cout << (*i)->x << "\n";
		  glVertex2f(-1 + 2 * (*i)->x / (FACTOR) - THICKNESS, -1 + 2 * (*i)->y / (FACTOR) -THICKNESS);
		  glVertex2f(-1 + 2 * (*i)->x / (FACTOR) +THICKNESS, -1 + 2 * (*i)->y / (FACTOR) -THICKNESS);
		  glVertex2f(-1 + 2 * (*i)->x / (FACTOR) +THICKNESS, -1 + 2 * (*i)->y / (FACTOR) +THICKNESS);
		  glVertex2f(-1 + 2 * (*i)->x / (FACTOR) -THICKNESS, -1 + 2 * (*i)->y / (FACTOR) +THICKNESS);
		  
		  glEnd();
	  }
  }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);


	  drawSomething();


      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);

    }
  };
}
