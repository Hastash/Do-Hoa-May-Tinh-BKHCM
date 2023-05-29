#include <windows.h>
#include <math.h>
#include <iostream>
#include <GL/glut.h>
#include <glut.h>

#define PI 3.14159265358
#define COLORNUM 14

using namespace std;


class Point3
{
public:
	float x, y, z;
	void set(float dx, float dy, float dz)
	{
		x = dx; y = dy; z = dz;
	}
	void set(Point3& p)
	{
		x = p.x; y = p.y; z = p.z;
	}
	Point3() { x = y = z = 0; }
	Point3(float dx, float dy, float dz)
	{
		x = dx; y = dy; z = dz;
	}

};
class Color3
{
public:
	float r, g, b;
	void set(float red, float green, float blue)
	{
		r = red; g = green; b = blue;
	}
	void set(Color3& c)
	{
		r = c.r; g = c.g; b = c.b;
	}
	Color3() { r = g = b = 0; }
	Color3(float red, float green, float blue)
	{
		r = red; g = green; b = blue;
	}

};
class Point2
{
public:
	Point2() { x = y = 0.0f; } // constructor 1
	Point2(float xx, float yy) { x = xx; y = yy; } // constructor 2
	void set(float xx, float yy) { x = xx; y = yy; }
	float getX() { return x; }
	float getY() { return y; }
	void draw() {
		glBegin(GL_POINTS);
		glVertex2f((GLfloat)x, (GLfloat)y);
		glEnd();
	}
private:
	float 	x, y;
};

class Vector3
{
public:
	float x, y, z;
	void set(float dx, float dy, float dz)
	{
		x = dx; y = dy; z = dz;
	}
	void set(Vector3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}
	void flip()
	{
		x = -x; y = -y; z = -z;
	}
	void normalize();
	Vector3() { x = y = z = 0; }
	Vector3(float dx, float dy, float dz)
	{
		x = dx; y = dy; z = dz;
	}
	Vector3(Vector3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}
	Vector3 cross(Vector3 b);
	float dot(Vector3 b);
};

class IntRect
{
public:
	IntRect() { l = 0; r = 100; b = 0; t = 100; } // constructor
	IntRect(int left, int right, int bottom, int top)
	{
		l = left; r = right; b = bottom; t = top;
	}
	void set(int left, int right, int bottom, int top)
	{
		l = left; r = right; b = bottom; t = top;
	}
	void draw() {
		glRecti(l, b, r, t);
		glFlush();
	} // draw this rectangle using OpenGL
	int getWidth() { return (r - l); }
	int getHeight() { return (t - b); }
private:
	int	l, r, b, t;
};


class RealRect
{
public:
	RealRect() { l = 0; r = 100; b = 0; t = 100; } // constructor
	RealRect(float left, float right, float bottom, float top)
	{
		l = left; r = right; b = bottom; t = top;
	}
	void set(float left, float right, float bottom, float top)
	{
		l = left; r = right; b = bottom; t = top;
	}
	float  getWidth() { return (r - l); }
	float  getHeight() { return (t - b); }
	void draw() {
		glRectf(l, b, r, t);
		glFlush();
	};// draw this rectangle using OpenGL
private:
	float	l, r, b, t;
};

class VertexID
{
public:
	int vertIndex;
	int colorIndex;
};

class Face
{
public:
	int 		nVerts;
	VertexID* vert;
	Vector3 	facenorm;
	int 		colorFace;

	Face()
	{
		nVerts = 0;
		colorFace = 0;
		vert = NULL;
	}
	~Face()
	{
		if (vert != NULL)
		{
			delete[] vert;
			vert = NULL;
		}
		nVerts = 0;
	}
};

class Mesh
{
public:
	int numVerts;
	Point3* pt;

	int numFaces;
	Face* face;

public:
	Mesh()
	{
		numVerts = 0;
		pt = NULL;
		numFaces = 0;
		face = NULL;
	}
	~Mesh()
	{
		if (pt != NULL)
		{
			delete[] pt;
		}
		if (face != NULL)
		{
			delete[] face;
		}
		numVerts = 0;
		numFaces = 0;
	}

	void SetColor(int colorIdx);

	void CreateCuboid(float fSizeX, float fSizeY, float fSizeZ);
	void CreateShape1(int NUM, float Radius0, float Radius1, float Radius2, float h, float h1, float fAngle, float fAngle1, float shift);
	void CreateShape2(int NUM, float Radius0, float Radius1, float Height0, float Height1, float fAngle);
	void CreateCylinder(int NUM, float h, float Radius0);

	void DrawWireframe();
	void DrawColor();
	void Draw2Color(bool mode, float diffuse[], float diffuse1[]);
	void Draw();

	void CalculateFacesNorm();
};

float ColorArr[COLORNUM][3] = { {1.0, 0.0, 0.0},	{0.0, 1.0, 0.0},{0.0, 0.0, 1.0},{1.0, 1.0, 0.0},{1.0, 0.75, 0.8},{0.0, 1.0, 1.0},
							{0.3, 0.3, 0.3},{0.5, 0.5, 0.5},{0.9, 0.9, 0.9},{1.0, 0.5, 0.5},{0.5, 1.0, 0.5},
							{0.5, 0.5, 1.0},{0.792, 0.821, 0.984}, {0.0, 0.0, 0.0} };


void Mesh::CreateShape1(int NUM, float Radius0, float Radius1, float Radius2, float Height0, float Height1, float fAngle, float fAngle1, float shift)
{
	numVerts = 12 + 6 * NUM;
	pt = new Point3[numVerts];

	float Angle;
	float Angle1;
	int i, idx;
	float x, y, z;

	for (i = 0, Angle = -fAngle / 2, Angle1 = -fAngle1 / 2; i <= NUM; i++)
	{
		x = Radius0 * cos(Angle * PI / 180);
		z = Radius0 * sin(Angle * PI / 180);

		pt[i].set(x, 0, z);
		pt[i + 1 + NUM].set(x, Height0, z);

		x = Radius2 * cos(Angle * PI / 180);
		z = Radius2 * sin(Angle * PI / 180);

		pt[i + (1 + NUM) * 2].set(x, 0, z);
		pt[i + (1 + NUM) * 3].set(x, Height1, z);

		x = shift + Radius1 * cos(Angle1 * PI / 180);
		z = Radius1 * sin(Angle1 * PI / 180);

		pt[i + (1 + NUM) * 4].set(x, Height0, z);
		pt[i + (1 + NUM) * 5].set(x, Height1, z);

		Angle += fAngle / NUM;
		Angle1 += fAngle1 / NUM;
	}

	float p = 0.91;

	x = Radius2 * cos(-PI / 6) * p;
	z = Radius2 * sin(-PI / 6) * p;

	pt[5 * NUM + 5 + NUM + 1].set(x, Height0, z);
	pt[5 * NUM + 5 + NUM + 2].set(x, Height1, z);
	pt[5 * NUM + 5 + NUM + 3].set(x, 0, z);
	pt[5 * NUM + 5 + NUM + 4].set(x, Height0, -z);
	pt[5 * NUM + 5 + NUM + 5].set(x, Height1, -z);
	pt[5 * NUM + 5 + NUM + 6].set(x, 0, -z);

	numFaces = 10 + 6 * NUM;
	face = new Face[numFaces];

	idx = 0;

	face[idx].nVerts = 4;
	face[idx].colorFace = 1;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 6 * NUM + 8;
	face[idx].vert[1].vertIndex = 0;
	face[idx].vert[2].vertIndex = NUM + 1;
	face[idx].vert[3].vertIndex = 6 * NUM + 6;

	face[++idx].nVerts = 4;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 6 * NUM + 6;
	face[idx].vert[1].vertIndex = 4 * NUM + 4;
	face[idx].vert[2].vertIndex = 5 * NUM + 5;
	face[idx].vert[3].vertIndex = 6 * NUM + 7;

	face[++idx].nVerts = 4;
	face[idx].colorFace = 1;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 2 * NUM + 1;
	face[idx].vert[1].vertIndex = 6 * NUM + 9;
	face[idx].vert[2].vertIndex = 6 * NUM + 11;
	face[idx].vert[3].vertIndex = NUM;

	face[++idx].nVerts = 4;
	face[idx].colorFace = 1;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 2 * NUM + 2;
	face[idx].vert[1].vertIndex = 6 * NUM + 8;
	face[idx].vert[2].vertIndex = 6 * NUM + 7;
	face[idx].vert[3].vertIndex = 3 * NUM + 3;

	face[++idx].nVerts = 4;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 6 * NUM + 10;
	face[idx].vert[1].vertIndex = 6 * NUM + 5;
	face[idx].vert[2].vertIndex = 5 * NUM + 4;
	face[idx].vert[3].vertIndex = 6 * NUM + 9;

	face[++idx].nVerts = 4;
	face[idx].colorFace = 1;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 3 * NUM + 2;
	face[idx].vert[1].vertIndex = 6 * NUM + 11;
	face[idx].vert[2].vertIndex = 6 * NUM + 10;
	face[idx].vert[3].vertIndex = 4 * NUM + 3;

	face[++idx].nVerts = 3;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 3 * NUM + 3;
	face[idx].vert[1].vertIndex = 6 * NUM + 7;
	face[idx].vert[2].vertIndex = 5 * NUM + 5;

	face[++idx].nVerts = 3;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 6 * NUM + 6;
	face[idx].vert[1].vertIndex = NUM + 1;
	face[idx].vert[2].vertIndex = 4 * NUM + 4;

	face[++idx].nVerts = 3;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 4 * NUM + 3;
	face[idx].vert[1].vertIndex = 6 * NUM + 5;
	face[idx].vert[2].vertIndex = 6 * NUM + 10;

	face[++idx].nVerts = 3;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 6 * NUM + 9;
	face[idx].vert[1].vertIndex = 5 * NUM + 4;
	face[idx].vert[2].vertIndex = 2 * NUM + 1;

	for (int i = 0; i < NUM; i++)
	{
		face[++idx].nVerts = 4;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].vertIndex = i;
		face[idx].vert[1].vertIndex = i + 1;
		face[idx].vert[2].vertIndex = i + NUM + 2;
		face[idx].vert[3].vertIndex = i + NUM + 1;

		face[idx + NUM].nVerts = 4;
		face[idx + NUM].vert = new VertexID[face[idx + NUM].nVerts];
		face[idx + NUM].vert[0].vertIndex = i + 3 * NUM + 3;
		face[idx + NUM].vert[1].vertIndex = i + 5 * NUM + 5;
		face[idx + NUM].vert[2].vertIndex = i + 5 * NUM + 6;
		face[idx + NUM].vert[3].vertIndex = i + 3 * NUM + 4;

		face[idx + 2 * NUM].nVerts = 4;
		face[idx + 2 * NUM].vert = new VertexID[face[idx + 2 * NUM].nVerts];
		face[idx + 2 * NUM].vert[0].vertIndex = i + 3 * NUM + 3;
		face[idx + 2 * NUM].vert[1].vertIndex = i + 2 * NUM + 2;
		face[idx + 2 * NUM].vert[2].vertIndex = i + 2 * NUM + 3;
		face[idx + 2 * NUM].vert[3].vertIndex = i + 3 * NUM + 4;

		face[idx + 3 * NUM].nVerts = 4;
		face[idx + 3 * NUM].vert = new VertexID[face[idx + 3 * NUM].nVerts];
		face[idx + 3 * NUM].vert[0].vertIndex = i;
		face[idx + 3 * NUM].vert[1].vertIndex = i + 2 * NUM + 2;
		face[idx + 3 * NUM].vert[2].vertIndex = i + 2 * NUM + 3;
		face[idx + 3 * NUM].vert[3].vertIndex = i + 1;

		face[idx + 4 * NUM].nVerts = 4;
		face[idx + 4 * NUM].vert = new VertexID[face[idx + 4 * NUM].nVerts];
		face[idx + 4 * NUM].vert[0].vertIndex = i + NUM + 1;
		face[idx + 4 * NUM].vert[1].vertIndex = i + NUM + 2;
		face[idx + 4 * NUM].vert[2].vertIndex = i + 4 * NUM + 5;
		face[idx + 4 * NUM].vert[3].vertIndex = i + 4 * NUM + 4;

		face[idx + 5 * NUM].nVerts = 4;
		face[idx + 5 * NUM].colorFace = 1;
		face[idx + 5 * NUM].vert = new VertexID[face[idx + 5 * NUM].nVerts];
		face[idx + 5 * NUM].vert[0].vertIndex = i + 4 * NUM + 4;
		face[idx + 5 * NUM].vert[1].vertIndex = i + 4 * NUM + 5;
		face[idx + 5 * NUM].vert[2].vertIndex = i + 5 * NUM + 6;
		face[idx + 5 * NUM].vert[3].vertIndex = i + 5 * NUM + 5;
	}
};

void Mesh::CreateShape2(int NUM, float Radius0, float Radius1, float Height0, float Height1, float fAngle)
{
	numVerts = 4 * NUM + 6;
	pt = new Point3[numVerts];

	numFaces = 5 * NUM + 4;
	face = new Face[numFaces];

	int i, idx;
	float x, y, z;
	float Angle;

	pt[0].set(0, 0, 0);
	pt[1].set(0, Height0, 0);

	for (i = 0, Angle = -fAngle / 2; i <= NUM; i++)
	{
		x = Radius1 * cos(Angle * PI / 180);
		z = Radius1 * sin(Angle * PI / 180);
		pt[i + 2].set(x, 0, z);
		pt[i + NUM + 3].set(x, Height1, z);

		x = Radius0 * cos(Angle * PI / 180);
		z = Radius0 * sin(Angle * PI / 180);
		pt[i + (NUM + 1) * 2 + 2].set(x, Height0, z);
		pt[i + (NUM + 1) * 3 + 2].set(x, Height1, z);

		Angle += fAngle / NUM;
	}

	idx = 0;

	face[idx].nVerts = 4;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = NUM + 3;
	face[idx].vert[1].vertIndex = 2;
	face[idx].vert[2].vertIndex = 2 * NUM + 4;
	face[idx].vert[3].vertIndex = 3 * NUM + 5;

	face[++idx].nVerts = 4;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 0;
	face[idx].vert[1].vertIndex = NUM + 2;
	face[idx].vert[2].vertIndex = 3 * NUM + 4;
	face[idx].vert[3].vertIndex = 1;

	face[++idx].nVerts = 4;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 3 * NUM + 4;
	face[idx].vert[1].vertIndex = NUM + 2;
	face[idx].vert[2].vertIndex = 2 * NUM + 3;
	face[idx].vert[3].vertIndex = 4 * NUM + 5;

	face[++idx].nVerts = 4;
	face[idx].vert = new VertexID[face[idx].nVerts];
	face[idx].vert[0].vertIndex = 1;
	face[idx].vert[1].vertIndex = 2 * NUM + 4;
	face[idx].vert[2].vertIndex = 2;
	face[idx].vert[3].vertIndex = 0;

	for (i = 0; i < NUM; i++)
	{
		face[++idx].nVerts = 4;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].vertIndex = i + 3 * NUM + 5;
		face[idx].vert[1].vertIndex = i + 2 * NUM + 4;
		face[idx].vert[2].vertIndex = i + 2 * NUM + 5;
		face[idx].vert[3].vertIndex = i + 3 * NUM + 6;

		face[idx + NUM].nVerts = 4;
		face[idx + NUM].colorFace = 1;
		face[idx + NUM].vert = new VertexID[face[idx + NUM].nVerts];
		face[idx + NUM].vert[0].vertIndex = i + 2;
		face[idx + NUM].vert[1].vertIndex = i + NUM + 3;
		face[idx + NUM].vert[2].vertIndex = i + NUM + 4;
		face[idx + NUM].vert[3].vertIndex = i + 3;

		face[idx + 2 * NUM].nVerts = 4;
		face[idx + 2 * NUM].vert = new VertexID[face[idx + 2 * NUM].nVerts];
		face[idx + 2 * NUM].vert[0].vertIndex = i + 3 * NUM + 6;
		face[idx + 2 * NUM].vert[1].vertIndex = i + NUM + 4;
		face[idx + 2 * NUM].vert[2].vertIndex = i + NUM + 3;
		face[idx + 2 * NUM].vert[3].vertIndex = i + 3 * NUM + 5;

		face[idx + 3 * NUM].nVerts = 3;
		face[idx + 3 * NUM].vert = new VertexID[face[idx + 3 * NUM].nVerts];
		face[idx + 3 * NUM].vert[0].vertIndex = i + 3;
		face[idx + 3 * NUM].vert[1].vertIndex = 0;
		face[idx + 3 * NUM].vert[2].vertIndex = i + 2;

		face[idx + 4 * NUM].nVerts = 3;
		face[idx + 4 * NUM].vert = new VertexID[face[idx + 4 * NUM].nVerts];
		face[idx + 4 * NUM].vert[0].vertIndex = i + 2 * NUM + 4;
		face[idx + 4 * NUM].vert[1].vertIndex = 1;
		face[idx + 4 * NUM].vert[2].vertIndex = i + 2 * NUM + 5;
	}
};

void Mesh::CreateCuboid(float Width, float Height, float Length)
{
	numVerts = 8;
	pt = new Point3[numVerts];

	float a = Width / 2, b = Height / 2, c = Length / 2;

	pt[0].set(-a, -b, -c);
	pt[1].set(a, b, -c);
	pt[2].set(a, -b, -c);
	pt[3].set(-a, b, -c);
	pt[6].set(-a, -b, c);
	pt[5].set(-a, b, c);
	pt[4].set(a, b, c);
	pt[7].set(a, -b, c);

	numFaces = 6;
	face = new Face[numFaces];

	face[0].nVerts = 4;
	face[0].vert = new VertexID[face[0].nVerts];
	face[0].vert[0].vertIndex = 0;
	face[0].vert[1].vertIndex = 6;
	face[0].vert[2].vertIndex = 5;
	face[0].vert[3].vertIndex = 3;

	face[1].nVerts = 4;
	face[1].vert = new VertexID[face[1].nVerts];
	face[1].vert[0].vertIndex = 3;
	face[1].vert[1].vertIndex = 5;
	face[1].vert[2].vertIndex = 4;
	face[1].vert[3].vertIndex = 1;

	face[2].nVerts = 4;
	face[2].vert = new VertexID[face[2].nVerts];
	face[2].vert[0].vertIndex = 2;
	face[2].vert[1].vertIndex = 1;
	face[2].vert[2].vertIndex = 4;
	face[2].vert[3].vertIndex = 7;

	face[3].nVerts = 4;
	face[3].vert = new VertexID[face[3].nVerts];
	face[3].vert[0].vertIndex = 0;
	face[3].vert[1].vertIndex = 2;
	face[3].vert[2].vertIndex = 7;
	face[3].vert[3].vertIndex = 6;

	face[4].nVerts = 4;
	face[4].vert = new VertexID[face[4].nVerts];
	face[4].vert[0].vertIndex = 4;
	face[4].vert[1].vertIndex = 5;
	face[4].vert[2].vertIndex = 6;
	face[4].vert[3].vertIndex = 7;

	face[5].nVerts = 4;
	face[5].vert = new VertexID[face[5].nVerts];
	face[5].vert[0].vertIndex = 1;
	face[5].vert[1].vertIndex = 2;
	face[5].vert[2].vertIndex = 0;
	face[5].vert[3].vertIndex = 3;
}

void Mesh::CreateCylinder(int NUM, float fHeight, float fRadius)
{
	numVerts = NUM * 2 + 2;
	pt = new Point3[numVerts];

	int		i;
	int		idx;
	float	fAngle = 2 * PI / NUM;
	float	x, y, z;

	pt[0].set(0, fHeight / 2, 0);
	for (i = 0; i < NUM; i++)
	{
		x = fRadius * cos(fAngle * i);
		z = fRadius * sin(fAngle * i);
		y = fHeight / 2;
		pt[i + 1].set(x, y, z);
		pt[i + 1 + NUM].set(x, -y, z);
	}
	pt[numVerts - 1].set(0, -fHeight / 2, 0);

	numFaces = NUM * 3;
	face = new Face[numFaces];

	idx = 0;
	for (i = 0; i < NUM; i++)
	{
		face[idx].nVerts = 3;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].vertIndex = 0;
		if (i < NUM - 1)
			face[idx].vert[1].vertIndex = i + 2;
		else
			face[idx].vert[1].vertIndex = 1;
		face[idx].vert[2].vertIndex = i + 1;
		idx++;
	}

	for (i = 0; i < NUM; i++)
	{
		face[idx].nVerts = 3;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].vertIndex = numVerts - 1;
		if (i < NUM - 1)
			face[idx].vert[2].vertIndex = i + 2 + NUM;
		else
			face[idx].vert[2].vertIndex = 1 + NUM;
		face[idx].vert[1].vertIndex = i + 1 + NUM;
		idx++;
	}

	for (i = 0; i < NUM; i++)
	{
		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[face[idx].nVerts];

		face[idx].vert[0].vertIndex = i + 1;
		if (i < NUM - 1)
			face[idx].vert[1].vertIndex = i + 2;
		else
			face[idx].vert[1].vertIndex = 1;
		face[idx].vert[2].vertIndex = face[idx].vert[1].vertIndex + NUM;
		face[idx].vert[3].vertIndex = face[idx].vert[0].vertIndex + NUM;

		idx++;
	}
}

void Mesh::DrawWireframe()
{
	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int f = 0; f < numFaces; f++)
	{
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++)
		{
			int iv = face[f].vert[v].vertIndex;
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::DrawColor()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int f = 0; f < numFaces; f++)
	{
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++)
		{
			int iv = face[f].vert[v].vertIndex;
			int ic = face[f].vert[v].colorIndex;

			glColor3f(ColorArr[ic][0], ColorArr[ic][1], ColorArr[ic][2]);
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int f = 0; f < numFaces; f++)
	{
		if (face[f].colorFace == 0)
		{
			glBegin(GL_POLYGON);
			for (int v = 0; v < face[f].nVerts; v++)
			{
				int iv = face[f].vert[v].vertIndex;

				glNormal3f(face[f].facenorm.x, face[f].facenorm.y, face[f].facenorm.z);
				glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
			}
			glEnd();
		}
	}
}

void Mesh::Draw2Color(bool mode, float diffuse[], float diffuse1[])
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int f = 0; f < numFaces; f++)
	{
		(face[f].colorFace == 1 && mode) ? glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse1) : glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++)
		{
			int iv = face[f].vert[v].vertIndex;

			glNormal3f(face[f].facenorm.x, face[f].facenorm.y, face[f].facenorm.z);
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::SetColor(int colorIdx) {
	for (int f = 0; f < numFaces; f++)
	{
		for (int v = 0; v < face[f].nVerts; v++)
		{
			face[f].vert[v].colorIndex = colorIdx;
		}
	}
}

void Mesh::CalculateFacesNorm()
{
	int iv, next;
	for (int f = 0; f < numFaces; f++)
	{
		float vN[] = { 0, 0, 0 };
		for (int v = 0; v < face[f].nVerts; v++)
		{
			iv = face[f].vert[v].vertIndex;
			next = face[f].vert[(v + 1) % face[f].nVerts].vertIndex;
			vN[0] += (pt[iv].y - pt[next].y) * (pt[iv].z + pt[next].z);
			vN[1] += (pt[iv].z - pt[next].z) * (pt[iv].x + pt[next].x);
			vN[2] += (pt[iv].x - pt[next].x) * (pt[iv].y + pt[next].y);
		}
		face[f].facenorm.set(vN[0], vN[1], vN[2]);
		face[f].facenorm.normalize();
	}
}

Vector3 Vector3::cross(Vector3 b)
{
	Vector3 c(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
	return c;
}
float Vector3::dot(Vector3 b)
{
	return x * b.x + y * b.y + z * b.z;
}
void Vector3::normalize()
{
	float temp = sqrt(x * x + y * y + z * z);
	x = x / temp;
	y = y / temp;
	z = z / temp;
}

GLfloat* setViewMatrix(Vector3& eye, Vector3& look, Vector3& up)
{
	GLfloat* VIEW = new GLfloat[16];
	Vector3 n, u, v, d;

	n.set(eye.x - look.x, eye.y - look.y, eye.z - look.z);
	u = up.cross(n);
	v = n.cross(u);

	n.normalize();
	u.normalize();
	v.normalize();

	d.set(-eye.dot(u), -eye.dot(v), -eye.dot(n));

	VIEW[0] = u.x; VIEW[1] = v.x; VIEW[2] = n.x; VIEW[3] = 0;
	VIEW[4] = u.y; VIEW[5] = v.y; VIEW[6] = n.y; VIEW[7] = 0;
	VIEW[8] = u.z; VIEW[9] = v.z; VIEW[10] = n.z; VIEW[11] = 0;
	VIEW[12] = d.x; VIEW[13] = d.y; VIEW[14] = d.z; VIEW[15] = 1;

	return VIEW;
}

void setupMaterial(float ambient[], float diffuse[], float specular[], float shiness)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiness);
}

float camera_angle = 150;
float camera_height = 40;
float camera_dis = 35;

bool orthoMode = false;
bool wireMode = false;
bool running = false;
bool draw2Color = false;

int	screenWidth = 1000;
int screenHeight = 500;

float H1_S1 = 1.54;
float H2_S1 = 2.75;
float R_Cylinder = 0.54;
float R2_S1 = 3.5;
float R3_S1 = 8.2;
float A_S1 = 60;
float A1_S1 = 66;

float POS_Cuboid = 5.1;
float R1_S1 = sqrt(3.0 / 4.0) * (POS_Cuboid - 2 * R_Cylinder);
float W_Base = 21.3;
float H_Base = 2.5;

float H_Cuboid = 1.06;
float H_Cylinder = 4.25;

float H1_S2 = H_Cuboid;
float H2_S2 = 2.0;
float R1_S2 = 2.9;
float R2_S2 = R2_S1;
float A_S2 = 77;

Mesh SHAPE1;
Mesh SHAPE2;
Mesh CUBOID;
Mesh CYLINDER;
Mesh BASE;

float preDW = 0;
float RW = 0;
float DW = 0;
float currentAngle = 0;

GLfloat PROJ[16];
GLfloat* V = new GLfloat[16];
GLfloat R[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
GLfloat T[16] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

GLfloat diffuse_red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat diffuse_shape2[] = { 0.8, 0.8, 0.0, 1.0 };
GLfloat diffuse_shape1[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat diffuse_base[] = { 1.0, 0.0, 0.5, 1.0 };

void mySpecialKeyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		camera_height += 3.0;
		break;

	case GLUT_KEY_DOWN:
		camera_height -= 3.0;
		break;

	case GLUT_KEY_RIGHT:
		camera_angle -= 3.0;
		break;

	case GLUT_KEY_LEFT:
		camera_angle += 3.0;
		break;

	default:
		break;
	}

	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	if (key == '1') {
		RW += 1;
		if (RW >= 360) {
			RW = 0;
		}
	}
	else if (key == '2') {
		RW -= 1;
		if (RW <= -360) {
			RW = 0;
		}
	}
	else if (key == '3')
		draw2Color = !draw2Color;

	else if (key == '+')
		camera_dis += 1.5;

	else if (key == '-')
		camera_dis -= 1.5;

	else if (key == 'w' || key == 'W')
		wireMode = !wireMode;

	else if (key == 'v' || key == 'V')
		orthoMode = !orthoMode;

	glutPostRedisplay();
}

void drawAll()
{
	GLfloat diffuse_white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat shiness = 40.0;

	Vector3 v0(POS_Cuboid + POS_Cuboid / 2 * cos(PI + RW * PI / 180), 0, POS_Cuboid / 2 * sin(PI + RW * PI / 180));
	Vector3 v1(cos(-PI / 6), 0, sin(-PI / 6));
	v0.normalize();
	v1.normalize();

	currentAngle = acos(v0.dot(v1)) * 180.0 / PI;
	if ((RW < -60 && RW > -300) || (RW > 60 && RW < 300)) {
		DW += (currentAngle - preDW);
		running = true;
	}
	else if ((abs(RW) == 300 || abs(RW) == 60) && running) {
		DW += (currentAngle - preDW);
	}
	else running = false;
	preDW = currentAngle;

	if (DW >= 360) DW = 0;
	if (DW <= -360) DW = 0;

	R[0] = cos(DW / 180 * PI);
	R[2] = -sin(DW / 180 * PI);
	R[8] = sin(DW / 180 * PI);
	R[10] = cos(DW / 180 * PI);

	T[12] = 2 * R_Cylinder; T[13] = H_Base; T[14] = 0;

	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glRotatef(60 * i, 0, 1, 0);
		glMultMatrixf(R);
		glMultMatrixf(T);
		setupMaterial(ambient, diffuse_shape1, specular, shiness);
		if (wireMode)
			SHAPE1.DrawWireframe();
		else {
			SHAPE1.Draw();
			SHAPE1.Draw2Color(draw2Color, diffuse_shape1, diffuse_white);
		}
		glPopMatrix();
	}
	//Base
	T[12] = 0; T[13] = H_Base / 2; T[14] = 0;

	glPushMatrix();
	glMultMatrixf(T);
	glMultMatrixf(R);
	setupMaterial(ambient, diffuse_base, specular, shiness);
	if (wireMode)
		BASE.DrawWireframe();
	else {
		BASE.Draw();
		BASE.Draw2Color(draw2Color, diffuse_base, diffuse_white);
	}
	glPopMatrix();
	//////////

	R[0] = cos(RW / 180 * PI);
	R[2] = -sin(RW / 180 * PI);
	R[8] = sin(RW / 180 * PI);
	R[10] = cos(RW / 180 * PI);
	//Shape2;

	T[12] = POS_Cuboid; T[13] = H_Base + H1_S1; T[14] = 0;
	glPushMatrix();
	glMultMatrixf(T);
	glMultMatrixf(R);
	setupMaterial(ambient, diffuse_shape2, specular, shiness);
	if (wireMode)
		SHAPE2.DrawWireframe();
	else {
		SHAPE2.Draw();
		SHAPE2.Draw2Color(draw2Color, diffuse_shape2, diffuse_white);
	}
	glPopMatrix();
	///////////////////////

	//Cuboid
	T[12] = POS_Cuboid; T[13] = H1_S1 + H_Cuboid / 2 + H_Base; T[14] = 0;

	glPushMatrix();
	glMultMatrixf(T);
	glMultMatrixf(R);
	setupMaterial(ambient, diffuse_shape2, specular, shiness);
	if (wireMode)
		CUBOID.DrawWireframe();
	else {
		CUBOID.Draw();
		CUBOID.Draw2Color(draw2Color, diffuse_shape2, diffuse_white);
	}
	glPopMatrix();
	//////
	//Cylinder
	glPushMatrix();
	T[12] = POS_Cuboid; T[13] = 0; T[14] = 0;
	glMultMatrixf(T);
	glMultMatrixf(R);

	T[12] = -POS_Cuboid / 2; T[13] = H_Cylinder / 2 + H_Base; T[14] = 0;
	glMultMatrixf(T);
	setupMaterial(ambient, diffuse_red, specular, shiness);
	if (wireMode)
		CYLINDER.DrawWireframe();
	else {
		CYLINDER.Draw();
		CYLINDER.Draw2Color(draw2Color, diffuse_red, diffuse_white);
	}
	glPopMatrix();
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float fHalfSize = 0.29;
	float ar = (float)screenWidth / (float)screenHeight;

	GLfloat left, right, bot, top, znear, zfar;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (orthoMode) {
		left = -22.0, right = 22.0, bot = -11.0, top = 11.0, znear = -100.0, zfar = 100.0;
		PROJ[0] = 2.0 / (right - left);
		PROJ[1] = PROJ[2] = PROJ[3] = PROJ[4] = PROJ[6] = PROJ[7] = PROJ[8] = PROJ[9] = PROJ[11] = 0;
		PROJ[5] = 2.0 / (top - bot);
		PROJ[10] = 2.0 / (znear - zfar);
		PROJ[12] = -(right + left) / (right - left);
		PROJ[13] = -(top + bot) / (top - bot);
		PROJ[14] = -(zfar + znear) / (zfar - znear);
		PROJ[15] = 1.0;
	}
	else {
		left = -ar * fHalfSize * 1.0, right = ar * fHalfSize * 1.0, bot = -fHalfSize * 1.0, top = fHalfSize * 1.0, znear = 1.0, zfar = 1000;
		PROJ[0] = 2.0 * znear / (right - left);
		PROJ[1] = PROJ[2] = PROJ[3] = PROJ[4] = PROJ[6] = PROJ[7] = PROJ[12] = PROJ[13] = PROJ[15] = 0;
		PROJ[5] = 2.0 * znear / (top - bot);
		PROJ[8] = (right + left) / (right - left);
		PROJ[9] = (top + bot) / (top - bot);
		PROJ[10] = -(zfar + znear) / (zfar - znear);
		PROJ[11] = -1.0;
		PROJ[14] = -2.0 * znear * zfar / (zfar - znear);
	}

	glLoadMatrixf(PROJ);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Vector3 cameraPos(camera_dis * sinf(camera_angle * PI / 180), camera_height, camera_dis * cosf(camera_angle * PI / 180));
	Vector3 lookAt(0, 4, 0);
	Vector3 d0(sinf(camera_angle * PI / 180), 0, cosf(camera_angle * PI / 180));
	Vector3 up(0, 1, 0);

	Vector3 cameraOrtho(0, 10, 0);
	Vector3 lookAtOrtho(0, 0, 0);
	Vector3 upOrtho(1, 0, 0);
	if (orthoMode)
		V = setViewMatrix(cameraOrtho, lookAtOrtho, upOrtho);
	else
		V = (camera_dis == 0) ? setViewMatrix(cameraPos, lookAt, d0) : setViewMatrix(cameraPos, lookAt, up);

	glLoadMatrixf(V);

	glViewport(0, 0, screenWidth, screenHeight);

	if (wireMode)
		glDisable(GL_LIGHTING);
	else
		glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);

	GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient0[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_position0[] = { -6.0, 9.0, -5.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	glEnable(GL_LIGHT1);

	GLfloat light_position1[] = { 6.0, 9.0, 5.0, 0.0 };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glShadeModel(GL_FLAT);

	drawAll();

	glFlush();

	glutSwapBuffers();
}

void myInit()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

int main(int argc, char* argv[])
{
	cout << "1     : Quay nguoc chieu kim dong ho" << endl;
	cout << "2     : Quay cung chieu kim dong ho" << endl;
	cout << "3     : Bat/tat che do to mau nang cao" << endl;
	cout << "V, v  : Chuyen doi giua 2 che do nhin" << endl;
	cout << "W, w  : Chuyen doi qua lai giua che do khung day va to mau" << endl;
	cout << "+     : Tang khoang cach camera" << endl;
	cout << "-     : Giam khoang cach camera" << endl;
	cout << "up arrow  : Tang chieu cao camera" << endl;
	cout << "down arrow: Giam chieu cao camera" << endl;
	cout << "<-        : Quay camera theo chieu kim dong ho" << endl;
	cout << "->        : Quay camera nguoc chieu kim dong ho" << endl;

	glutInit(&argc, (char**)argv);							  
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);			  
	glutInitWindowPosition(100, 100);						  
	glutCreateWindow("Duong Hanh Minh - 2020045");			

	CUBOID.CreateCuboid(POS_Cuboid, H_Cuboid, 2 * R_Cylinder);
	CUBOID.CalculateFacesNorm();

	CYLINDER.CreateCylinder(36, H_Cylinder, R_Cylinder);
	CYLINDER.CalculateFacesNorm();

	SHAPE1.CreateShape1(36, R1_S1, R2_S1, R3_S1, H1_S1, H2_S1, A_S1, A1_S1, POS_Cuboid - 2 * R_Cylinder);
	SHAPE1.CalculateFacesNorm();

	SHAPE2.CreateShape2(36, R1_S2, R2_S2, H1_S2, H2_S2, A_S2);
	SHAPE2.CalculateFacesNorm();

	BASE.CreateCuboid(W_Base, H_Base, W_Base);
	BASE.CalculateFacesNorm();

	myInit();

	glutDisplayFunc(myDisplay);
	glutSpecialFunc(mySpecialKeyboard);
	glutKeyboardFunc(myKeyboard);
	glutMainLoop();

	return 0;
}