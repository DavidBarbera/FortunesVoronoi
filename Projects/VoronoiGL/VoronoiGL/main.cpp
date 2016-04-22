// Based on http://blog.ivank.net/fortunes-algorithm-and-implementation.html in order to test the algorithm
//

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
vor::Vertics * ver;  //tops, the points or sites
vor::Vertics * dir;  // a settlement which moves, directions to make the points move
vor::Edges * edg;	 // edge diagram
vor::Diagram * graph;

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

	ver->push_back(new Point(w*(double)0.2, w*(double)0.8));
   /// ver->push_back(new Point(w*(double)0.2, w*(double)0.85));

//	ver->push_back(new Point(w*(double)0.35, w*(double)0.75));
/*	ver->push_back(new Point(w*(double)0.9, w*(double)0.91));
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


	for (int i = 0; i < ver->size(); i++) dir->push_back(new Point(0, 0));


	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_SINGLE); 
	glutInitWindowSize(600, 600); 
	glutInitWindowPosition(100, 100); 
	glutCreateWindow("Fortune's Voronoi Algorithm in 2D"); 

	glutTimerFunc(100, onEF, 0);
	glutDisplayFunc(display); 

	glutReshapeFunc(reshape); 

	glutMainLoop(); 

	return 0;
}

void drawVoronoi()
{
	glColor3f(0, 0, 0);
// this is just to add velocities to our points and simulate hiting the borders of the screen
// At the moment velocity is zero, until algorithm works properly.
// from http://blog.ivank.net/fortunes-algorithm-and-implementation.html
// can be improved to simulated forces and tensions.
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


	
	// Rendering of points as little quads:
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

//This part is based on http://blog.ivank.net/fortunes-algorithm-and-implementation.html
// computes fast, but doesn't allow to store info about the diagram.
	 edg = v->VoronoiDiagram(ver, w, w);
	glBegin(GL_LINES);
	for (vor::Edges::iterator i = edg->begin(); i != edg->end(); ++i)
	{
		glVertex2f(-1 + 2 * (*i)->start->x / w, -1 + 2 * (*i)->start->y / w);
		glVertex2f(-1 + 2 * (*i)->end->x / w, -1 + 2 * (*i)->end->y / w);
	}
	glEnd();

// This part uses DCEL, would be easier to construct Delaunay Triangulation and deal with the colouring of regions once algorithm works.
/*	graph = v->VoronoiDiagram(ver, w, w);
	glBegin(GL_LINES);
	for (vor::Diagram::iterator i = graph->begin(); i != graph->end(); ++i)
	{
		glVertex2f(-1 + 2 * (*i)->x / w, -1 + 2 * (*i)->y / w);
		//glVertex2f(-1 + 2 * (*i)->x / w, -1 + 2 * (*i)->y / w);
	}
	glEnd();
*/


}


void display(void)
{
	glLoadIdentity();  
	glTranslatef(0.0f, 0.0f, -5.0f);

	glFlush();
}


void onEF(int n)
{

	glutTimerFunc(20, onEF, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 

	drawVoronoi();

	glutSwapBuffers();
	
}

void reshape(int width, int height)
{

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); 
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	gluPerspective(22.5, (GLfloat)width / (GLfloat)height, 1.0, 100.0); 
	glMatrixMode(GL_MODELVIEW);
}