// BasicScene.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "BasicScene.h"
#include <SDL.h>
#include <glew.h>

int gRotation = 0;

void render(SDL_Window* win);

void InitScene(int width, int height, SDL_Window* win);

int main(int argc, char* argv[]) {
	SDL_Window* win;
	int width = 1920;
	int height = 1280;
	bool done = false;
	int flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
	win = SDL_CreateWindow("Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
	SDL_GLContext glContext = SDL_GL_CreateContext(win);

	InitScene(width, height, win);

	while (!done) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
				done = true;
			}
		}

		render(win);
		Sleep(10);
	}

	SDL_Quit();
	return 0;
}

void InitScene(int width, int height, SDL_Window* win)
{
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		char* msg = (char*)glewGetErrorString(error);
		MessageBoxA(NULL, msg, "Error caption", MB_ICONERROR);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glClearColor(0.5f, 1.0f, 0.1f, 1.0f);

	// -------------- Inicializa escena GL
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float fieldOfView = 75.0f;
	// The following code is a fancy bit of math that is eqivilant to calling:
	// gluPerspective( fieldOfView/2.0f, width/height , 0.1f, 255.0f )
	// We do it this way simply to avoid requiring glu.h
	GLfloat zNear = 0.5f;
	GLfloat zFar = 5000.0f;
	GLfloat aspect = float(width) / float(height);
	GLfloat fH = tan(fieldOfView / 360.0 * 3.14159) * zNear;
	GLfloat fW = fH * aspect;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// -------------------

	// Inicialize GL
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
}

void render(SDL_Window* win) {
	// clear screen
	glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	// world transformations
	glLoadIdentity();
	glTranslatef(0, -5, -10);
	glRotatef(2.0 * gRotation, 0.0f, 1.0f, 0.0f);
	gRotation += 1;

	// draw
	glBegin(GL_TRIANGLES);
	glColor3f(0.5f, 0.1f, 0.1f); glVertex3f(-2.0f, 0.0f, -2.0);
	glColor3f(0.5f, 0.4f, 0.1f); glVertex3f( 2.0f, 0.0f, -2.0);
	glColor3f(0.1f, 0.1f, 0.3f); glVertex3f( 0.0f, 3.0f,  0.0);

	glColor3f(0.5f, 0.1f, 0.1f); glVertex3f(-2.0f, 0.0f, -2.0);
	glColor3f(0.5f, 0.4f, 0.1f); glVertex3f(-2.0f, 0.0f,  2.0);
	glColor3f(0.1f, 0.1f, 0.3f); glVertex3f( 0.0f, 3.0f,  0.0);

	glColor3f(0.5f, 0.1f, 0.1f); glVertex3f(-2.0f, 0.0f,  2.0);
	glColor3f(0.5f, 0.4f, 0.1f); glVertex3f( 2.0f, 0.0f,  2.0);
	glColor3f(0.1f, 0.1f, 0.3f); glVertex3f( 0.0f, 3.0f,  0.0);

	glColor3f(0.5f, 0.1f, 0.1f); glVertex3f(2.0f,  0.0f, -2.0);
	glColor3f(0.5f, 0.4f, 0.1f); glVertex3f(2.0f,  0.0f,  2.0);
	glColor3f(0.1f, 0.1f, 0.3f); glVertex3f(0.0f,  3.0f,  0.0);
	glEnd();

	// show image
	SDL_GL_SwapWindow(win);
}
