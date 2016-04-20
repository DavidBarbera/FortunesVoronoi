#include <GL/glew.h> // Include the GLEW header file
#include <GL/glut.h> // Include the GLUT header file
#include <iostream>
#include <math.h>
#include <algorithm>
#include <time.h>


#include "Voronoi.h"
#include "Point.h"

void display(void);
void onEF(int n);
void reshape(int width, int height);

vor::Voronoi * v;
vor::Vertics * ver; // vrcholy //tops, the points or sites
vor::Vertics * dir; // smìry, kterými se pohybují // a settlement which moves, directions to make the points move
vor::Diagram * edg;	 // edge diagram

double w = 100;

int main(int argc, char **argv)
{
	using namespace vor;

	v = new Voronoi();
	ver = new Vertics();
	dir = new Vertics();

	srand(time(NULL));

	//Creates random points (or sites) with random directions
	for (int i = 0; i < 5; i++)
	{

		//ver->push_back(new VPoint(w * (double)rand() / (double)RAND_MAX, w * (double)rand() / (double)RAND_MAX));
		//ver->push_back(new VPoint(w*(double)i / 5, w*(double)i / 5));

		//dir->push_back(new VPoint( (double)rand()/(double)RAND_MAX - 0.5, (double)rand()/(double)RAND_MAX - 0.5)); 
		//dir->push_back(new VPoint(0, 0));
	}

	ver->push_back(new Point(w*(double)0.5, w*(double)0.9));
/*
	ver->push_back(new Point(w*(double)0.2, w*(double)0.8));
	ver->push_back(new Point(w*(double)0.35, w*(double)0.75));
	ver->push_back(new Point(w*(double)0.9, w*(double)0.91));
	ver->push_back(new Point(w*(double)0.15, w*(double)0.65));
	ver->push_back(new Point(w*(double)0.12, w*(double)0.35));
	ver->push_back(new Point(w*(double)0.85, w*(double)0.25));
	ver->push_back(new Point(w*(double)0.95, w*(double)0.55));
	ver->push_back(new Point(w*(double)0.65, w*(double)0.15));
	ver->push_back(new Point(w*(double)0.85, w*(double)0.1));
	ver->push_back(new Point(w*(double)0.1, w*(double)0.12));
*/
	ver->push_back(new Point(w*(double)0.8, w*(double)0.7));
	ver->push_back(new Point(w*(double)0.45, w*(double)0.5));
	ver->push_back(new Point(w*(double)0.7, w*(double)0.4));
	ver->push_back(new Point(w*(double)0.3, w*(double)0.2));


	for (int i = 0; i < 5; i++) dir->push_back(new Point(0, 0));

	//insert those random points (or sites) into Voronoi v, with bounding box 
	//	edg = v->GetEdges(ver, w, w);
	std::cout << "Voronoi is done!\n";

	/*	for(vor::Edges::iterator i = edg->begin(); i!= edg->end(); ++i)
	{
	if( (*i)->start == 0 )
	{
	std::cout << "chybi zacatek hrany! = missing first edge\n";
	continue;
	}
	if( (*i)->end == 0 )
	{
	std::cout << "chybi konec hrany! = missing last edge\n";
	continue;
	}
	}*/

	glutInit(&argc, argv); // Initialize GLUT
	glutInitDisplayMode(GLUT_SINGLE); // Set up a basic display buffer (only single buffered for now)
	glutInitWindowSize(300, 300); // Set the width and height of the window
	glutInitWindowPosition(100, 100); // Set the position of the window
	glutCreateWindow("Fortune's Voronoi Algorithm in 2D"); // Set the title for the window

	glutTimerFunc(100, onEF, 0);
	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering

	glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping

							  //glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses
							  //glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events

	glutMainLoop(); // Enter GLUT's main loop

	return 0;
}

void drawVoronoi()
{
	glColor3f(0, 0, 0);

	vor::Vertics::iterator j = dir->begin();
	for (vor::Vertics::iterator i = ver->begin(); i != ver->end(); ++i)
	{
		(*i)->x += (*j)->x * w / 50;
		(*i)->y += (*j)->y * w / 50;
		if ((*i)->x > w) (*j)->x *= -1;
		if ((*i)->x < 0) (*j)->x *= -1;

		if ((*i)->y > w) (*j)->y *= -1;
		if ((*i)->y < 0) (*j)->y *= -1;
		++j;
	}


	//edg = v->GetEdges(ver, w, w);
	
	//std::cout << "voronoi done";
	for (vor::Vertics::iterator i = ver->begin(); i != ver->end(); ++i)
	{
		glBegin(GL_QUADS);
		//std::cout << (*i)->x << "\n";
		glVertex2f(-1 + 2 * (*i)->x / w - 0.01, -1 + 2 * (*i)->y / w - 0.01);
		glVertex2f(-1 + 2 * (*i)->x / w + 0.01, -1 + 2 * (*i)->y / w - 0.01);
		glVertex2f(-1 + 2 * (*i)->x / w + 0.01, -1 + 2 * (*i)->y / w + 0.01);
		glVertex2f(-1 + 2 * (*i)->x / w - 0.01, -1 + 2 * (*i)->y / w + 0.01);
		glEnd();
	}

	edg = v->VoronoiDiagram(ver);
	glBegin(GL_LINES);
	for (vor::Diagram::iterator i = edg->begin(); i != edg->end(); ++i)
	{
		glVertex2f(-1 + 2 * (*i)->x / w, -1 + 2 * (*i)->y / w);
	}
	glEnd();
	
}


void display(void)
{
	std::cout << "display\n";
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
	glTranslatef(0.0f, 0.0f, -5.0f);

	glFlush();
}


void onEF(int n)
{

	glutTimerFunc(20, onEF, 0);
	glClear(GL_COLOR_BUFFER_BIT);//Clear the screen
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Clear the background of our window to red  

	drawVoronoi();
	glutSwapBuffers();
	//Draw everything to the screen
}

void reshape(int width, int height)
{

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
	glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
	glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
	gluPerspective(22.5, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
	glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}