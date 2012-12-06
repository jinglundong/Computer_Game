/*****************************************************************
 *	
 *	Computer Game, Spring 2012
 *	New York University
 *
 *	This is the demo for light, blending, and texture maping.
 *  Created by Jiakzai Zhang	Verson: Mar 05 2012	
 *	Introduction: 
 *	
 *		Keyboard: 
 *			m : Change the type of drawing
 *			l : Open or Close the light
 *			w : Go Forward to the direction of camera
 *          s : Go Back to the direction of camera
 *			a : Do rotate
 *			d : Do rotate
 *			e : Switch explode mode
 *			f : if camera follows the plane
 *			F1: Away from the point of view 
 *			F2: Close to the point of view
 *		
 ******************************************************************/
#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <math.h>
#	include <gl\glew.h>
#	include "3dsModel.h"
#include <vector>
#include <sstream>
#include "Spider.h"
#include "Spiders.h"
#include <time.h>
#include <algorithm>

#ifdef __APPLE__
#	include <GLUT\glew.h>
#	include <GLUT\glut.h>
#elif defined(__LINUX__)
#	include <GL/glew.h>
#	include <GL/glut.h>	
#	include <unistd.h>
#	include <getopt.h>
#	include "3dsModel.h"
#	include "Tga.h"
#else
#	include <gl\glew.h>
#	include <gl\glut.h>	
#	include <iostream>
#	include "Tga.h"
#	pragma comment (lib, "glew32.lib")
//start of leaking detection
//#include <crtdbg.h>
//end of leaking detection
using namespace std;
#endif
#include "RgbImage.h"
#include "TextureFont.h"

#define GLUT_WHEEL_UP 3
#define GLUT_WHEEL_DOWN 4

/* offset of vertex structure */
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

/* windows size */
int width = 640;
int height = 480;

/* Param of keys */
bool keys[256] = {false};
bool specialKeys[256] = {false};
bool lp = false;		//key l press?
bool mp = false;		//key m press?
bool fp = false;		//key f press?
bool cp = false;		//key c press?
bool sp = false;		//key space press?
bool ep = false;		//key e press?

/* Functional Switches */
bool light = true;		 // Light
bool wireFrameMode = false;	 // Draw type
bool followPlane = true; // Camera follow the fight
bool cockPitFlag = false;	
bool newMissileFlag = false;	
bool explodeModeFlag = false;

/* Vars of texture */
GLuint texture[3];		// to save the four textures
const char *flatHeightImage = "data/flatHeightMap.bmp";
const char *heightImage = "data/heightMap.bmp";
const char *fontImage   = "data/Font.bmp";
const char *filenames[]  = {
    "data/Bison.bmp",  
    "data/Canyon.bmp",  
    "data/Grass.bmp",  
    "data/LavaCracks.bmp",  
    "data/LunarTarget.bmp",  
    "data/MountainFaults.bmp",
	"data/metal.bmp",
	"data2/fire1.bmp"
};

// which textures are selected
int terrainTexIndex = 0;
int planeTexIndex = -1;                // just the simple green wing .. no texture
int spiderAmount = 0;
const char *terrainTexFile;
const char *planeTexFile;
const char *explodeFile;

/* Param of Height Map */
const int sizeMap = 512;
//const int sizeMap = 256;
//const int sizeMap = 128;
float SPIDERSPEED = 0.001;		
bool RANDOMSIZEFLAG = false;
float aveSpiderSize = 5.0;
int mineAmount ;
bool SHOWPLANEFLAG = false;
bool FLATFLAG = true;

//////////////////////////////////////////////////////////////////////////////////////////////
//  Parameters of the Terrain Map
const float heightScaleFactor =  15.0;         // height scaling factor
const float xTerrainDimension = 1;        
const float zTerrainDimension = 1;        

/* Parameters of the Camera */
const float	c = 3.1415926/180.0f;	// Transform from angle to radian
float phi = 90;				// Rotate angle, phi in the Cartesian space
float oldmy = -1, oldmx = -1;		// Save the last location of mouse
float r = 20.0f;			// Radius, distance between location of camera and view point 
float theta = 0.0f;			// Theta in the Cartesian space

GLfloat Vx = 0.0f,	Vy = 1.0f,	 Vz = 0.0f;		// View-up vector.
GLfloat xCam = 0.0f,	yCam = 0.0f,	 zCam = 0.0f;		// Location of camera related the look at point
GLfloat xrefCam  = 0.0f,yrefCam  = 25.0f,zrefCam  = 0.0f;	// Look-at point.

GLfloat LightAmbient[]	= { 0.5f, 0.5f, 0.5f, 1.0f };	// Param of Environment light
GLfloat LightDiffuse[]	= { 1.0f, 1.0f, 1.0f, 1.0f };	// Param of Diffuse light
GLfloat LightSpecular[]	= { 1.0f, 1.0f, 1.0f, 1.0f };	// Param of Specular light
GLfloat LightPosition[] = { 5.0f, 10.0f, 5.0f, 1.0f };	// The position of light

/* Param of Plane */
GLfloat moveSpeed = 0.03f;
GLfloat rotateSpeed = 0.05f;
GLfloat spinRotateSpeed = 2.0f;
GLfloat phiPlane = 90;	
bool animationL = false;
bool animationR = false;
bool animationU = false;
bool animationD = false;
float animationRotate = 0.0;
float animationRotateZ = 0.0;
float spinRotate = 0.0;

/* Save Intro string */
char stringInf[80];

/* display list */
int mysphereID, mysphereID2;

/* frame number of plane explosion */
int planeExplodeFrame = 1 ;

/* Define the vertex structure */
struct myVertex {
    GLfloat x, y, z;               // x,y,z coordinates of the vertex
    GLfloat nx, ny, nz;            // normal vector of the vertex point
    GLfloat s0, t0;                // 2D Texture coordinates
};

class Mine{
	float x, y, z;
	int explodeFrame;
	bool visible;
	bool explodingFlag;
	bool explodingFinishFlag;			//if it is true, should delete this mine
	float radian;
public:
	Mine(float x, float y, float z){ this->x = x; this->y = y; this->z = z; this->explodingFlag = false; this->visible = true; this->explodingFinishFlag = false; radian = 3.0;};
	Mine(){};
	void explode();								//view
	bool isTriggered(Spiders *spiders);			//if spider triger this mine, set it state to die.
	void draw();
	bool isExploding(){return explodingFlag;};
	bool isExplodingFinish(){return explodingFinishFlag;};
};

void Mine::draw(){
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glTranslatef(this->x, this->y, this->z);
	if (this->visible){
		if (!this->explodingFlag){
			glPushMatrix();
			glRotatef(90, 0, 1, 0);
			GLUquadricObj *sphere=NULL;
			sphere = gluNewQuadric();
			gluQuadricDrawStyle(sphere, GLU_FILL);
			gluQuadricTexture(sphere, GL_TRUE);
			gluQuadricNormals(sphere, GLU_SMOOTH);
			glColor3f(0.1, 0.3, 0.1);
			glScalef(radian, 1, radian);
			gluSphere(sphere, 1.0, 20, 20);
			gluDeleteQuadric(sphere);
			glPopMatrix();
		}
		else{
			if (!explodingFinishFlag){
				this->explode();
			}
		}
	}
	else{
		if (this->explodingFlag && !explodingFinishFlag){
			this->explode();
		}
	}
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void Mine::explode(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]);

	glPushMatrix();
	float r = 1 + 0.05 * this->explodeFrame;
	glScalef(r, r, r);
	glColor3f(1, 1, 0);

	GLUquadricObj *sphere=NULL;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluSphere(sphere, 1.0, 20, 20);
	gluDeleteQuadric(sphere);
	if (this->explodeFrame<50){
		this->explodeFrame++;
	}
	else{
		this->explodingFinishFlag = true;
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	//To fix the bug with font
}

bool Mine::isTriggered(Spiders *spiders){
	bool result = false;
	float tmpSize = 0;
	MyVector tmpPos = MyVectorBuilder::createMyVectorBuilder()
		.withX(this->x)
		.withY(this->y)
		.withZ(this->z)
		.build();
	for (int i =0; i<(int) spiders->getSpiderList().size(); i++){
		tmpSize = spiders->getSpiderList().at(i).getSize();
		if (abs(this->x - spiders->getSpiderList().at(i).getPosition().getX()) > tmpSize + this->radian){
			continue;
		}
		if (abs(this->z - spiders->getSpiderList().at(i).getPosition().getZ()) > tmpSize + this->radian){
			continue;
		}
		if (tmpPos.distance(spiders->getSpiderList().at(i).getPosition()) < tmpSize + this->radian){
			spiders->die(i, 0);
			if (!this->explodingFlag){
				this->explodeFrame = 1;
			}
			this->explodingFlag = true;
			result = true;
		}
	}

	return result;
}

class missile{
	float x, y, z;
	float vx, vy, vz;
	float t;
	int explodeFrame;
	bool forwardFlag;
public:
	missile(float x, float y, float z, float HorizontalVelocity, float phi);
	void forward();
	void explode();
	void tgaExplode();
	bool isColliding(float height);
	void drawMissile();
	float getX(){return x;};
	float getZ(){return z;};
	void toString();
	bool needToExit(){return this->explodeFrame>120;};
	bool tgaNeedToExit(){return this->explodeFrame>=170;};
	int getExplodeFrame();
};

	/* List of missiles*/
	vector<missile*> missiles;

	/* List of spiders*/
	Spiders spiders;

	/* List of mines*/
	vector<Mine> mines;
	int mineFrame = 0;		//don't have to detect trigger every draw cycle

void missile::tgaExplode(){
	int frame = this->explodeFrame / 10 +1;
	if (frame < 1 || frame >17){
		cout<<"invalid frame number";
		std::exit(1);
	}
	TGA tgaExplode;
	string frameNumber, fileName;
	std::stringstream ss;
	ss<<frame;
	if (frame < 10){
		frameNumber.append("0");
	}
	frameNumber.append(ss.str());
	fileName.append("data2/explode ");
	fileName.append(frameNumber);
	fileName.append(".tga");
	const char * c = fileName.c_str();
	tgaExplode.Load(c);
	glEnable(GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glPushMatrix();
	glScalef(100,100,100);
	tgaExplode.Draw();
	glPopMatrix();
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	//To fix the bug with font
	explodeFrame++;
}

void missile::toString(){
	cout<<"x:"<<x<<"y"<<y<<"z"<<z<<"\n";
	cout<<"vx:"<<vx<<"vy:"<<vy<<"vz:"<<vz<<"\n";
	cout<<"t:"<<t<<"\n";
}

void missile::forward(){
	if (forwardFlag){
		x += vx;	
		y += vy;
		z += vz;
		vy = -0.5 * 0.09 * t * t;
		t += 0.01;
	}
}

void planeExplode(int frame){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]);

	glPushMatrix();
	float r = 1.5 * pow((float)1.01,(frame+1));
	glScalef(r, r, r);
	glCallList(mysphereID2);
	glPopMatrix();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	//To fix the bug with font
}

void initMines(){
	for (int i = 0 ; i < mineAmount; i++){
		mines.push_back(Mine(rand()%100-50,	0,	rand()%100-50));
		//mines.push_back(Mine(0, 0 , 0));
	}
}

void missile::explode(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]);

	glPushMatrix();
	glTranslatef(x, y, z);
	float r = pow((float)1.02,(this->explodeFrame+1));
	//r = 1 + 0.05 * this->explodeFrame;
	glScalef(r, r, r);
	glCallList(mysphereID2);
	this->explodeFrame++;
	glPopMatrix();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	//To fix the bug with font
}

bool isPlaneColliding(float height){
	if (yrefCam < height){
		return true;
	}
	else{
		return false;
	}
}

bool missile::isColliding(float height){
	if (height >= y){
		this->forwardFlag = false;
		return true;
	}
	else{
		return false;
	}
}

float getYFromXZ(float x, float z, myVertex* terrainVertex){
	int vindex = (int) (x + sizeMap/2) * sizeMap + (int) z + sizeMap/2;
	return terrainVertex[vindex].y;
}

//bool myfunction (int a,int b) { return (a<b); }

void DrawMissiles(vector<missile*> missiles, myVertex* terrainVertex, Spiders* spiders){
	vector<missile*>::iterator it;
	vector<int> needToExit;
	int i = 0, j = 0;
	float tmpSize;
	for (it = missiles.begin(); it != missiles.end(); it++){
		if ((*it)->needToExit()){
			continue;
		}
		float terrainY = getYFromXZ ((*it)->getX(), (*it)->getZ(), terrainVertex);		
		if ((*it)->isColliding(terrainY)){
			//Utilize "explode XX.tga"	
			if (explodeModeFlag){				
				if (!(*it)->tgaNeedToExit()){
					(*it)->tgaExplode();
				}
				else{						
					needToExit.push_back(i);
				}
			}
			//Utilize "fire1.bmp"
			else{								
				if (!(*it)->needToExit()){						
					(*it)->explode();
				}
				else{						
					needToExit.push_back(i);
				}
			}
			tmpSize = 0;
			MyVector tmpPos = MyVectorBuilder::createMyVectorBuilder()
				.withX((*it)->getX())
				.withY(0)
				.withZ((*it)->getZ())
				.build();
			if ((*it)->getExplodeFrame() == 2){
				for (j = 0; j<(int)spiders->getSpiderList().size();j++){
					tmpSize = spiders->getSpiderList().at(i).getSize();
					if (abs((*it)->getX() - spiders->getSpiderList().at(j).getPosition().getX()) > tmpSize + 20){
						continue;
					}
					if (abs((*it)->getZ() - spiders->getSpiderList().at(j).getPosition().getZ()) > tmpSize + 20){
						continue;
					}
					if (tmpPos.distance(spiders->getSpiderList().at(j).getPosition()) < tmpSize + 20){
						spiders->die(j, 1);
					}
				}
			}
		}
		else{
			(*it)->drawMissile();
			(*it)->forward();
		}	
		i++;
	}
	//clear missiles that already exit
	//sort (needToExit.begin(), needToExit.end(), myfunction);
	//for (i = 0; i < needToExit.size(); i++){
	//	missiles.erase(missiles.begin()+needToExit.at(i)-i);
	//}
	//needToExit.clear();
}

int missile::getExplodeFrame(){
	return this->explodeFrame;
}
missile::missile(GLfloat x, GLfloat y, GLfloat z, GLfloat HorizontalVelocity, GLfloat phi){
	this->x = x;
	this->y = y;
	this->z = z;
	this->vx = -20*moveSpeed*cos(c*phi);
	this->vy = 0;
	this->vz = -20*moveSpeed*sin(c*phi);
	this->t = 0;
	this->explodeFrame = 1;
	this->forwardFlag = true;
}

void initFireBall(){
	GLUquadricObj *sphere=NULL;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	 //Making a display list
	mysphereID2 = glGenLists(1);
	glNewList(mysphereID2, GL_COMPILE);
	glColor4f(1.0f, 1.0f, 0.8f, 1.0f);
	glScalef(0.5, 0.5, 0.5);
	gluSphere(sphere, 2.0, 20, 20);
	glEndList();
	gluDeleteQuadric(sphere);
}

void missile::drawMissile(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glEnable( GL_TEXTURE_2D );  
	glEnable(GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc ( GL_GREATER, 0 );
	
	glPushMatrix();
	glTranslatef(x, y, z);
	glCallList(mysphereID2);
	glPopMatrix();  
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	//To fix the bug with font
	glDisable(GL_ALPHA_TEST);
}

/* Param of Terrain */
const int nVertex = sizeMap*sizeMap;
const int nFaces  = (sizeMap-1)*(sizeMap-1);
myVertex terrainVertex[nVertex];
GLuint terrainIndex[nFaces*4];

GLuint VertexVBOID = 0;
GLuint IndexVBOID = 0;

/*Init NicePlane*/
string fileName ("data2/plane.3ds");
CModel3DS nicePlane(fileName, (GLushort)5);

/*Init CockPit*/
TGA tgaCockPit;

GLvoid InitCockPit(){
	tgaCockPit.Load("data2/cockpit.tga");
}

GLvoid cockPit(){
	glEnable(GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	tgaCockPit.Draw();
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	//To fix the bug with font
}

GLvoid InitNicePlane(){
	nicePlane.GetFaces();
	nicePlane.initializeVBO();
	nicePlane.loadVBO();
}

GLvoid DrawNicePlane(){
	float terrainY = getYFromXZ (xrefCam, zrefCam, terrainVertex);	
	if (!isPlaneColliding(terrainY)){
		glPushMatrix();
		glRotatef(-90.0,1,0,0);
		glRotatef(-90.0,0,0,1);
		nicePlane.Draw();
		glPopMatrix();
	}
	else{
		if (planeExplodeFrame < 150){
			planeExplode(planeExplodeFrame);
			planeExplodeFrame++;
		}
	}
}

GLvoid InitPlane(){
	;
}

GLvoid DrawPlane()                
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Student:
    //
    // Draw a plane here that is not just simply a box or two boxes. 
    // Try to select some shape and bind a texture to it. If you pick another texture other
    // than those provided here (preferrable) then provide the texture with your submission.
    // if you select one from lab3, then please just point it out in your email and don't submit the
    // texture.
    // As in lab3, remember to
    // a) load texture (you need to modify a few things in the program for that
    // b) bind texture and enable and disable a few things

	GLUquadricObj *sphere=NULL;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	 
	 //Making a display list
	int mysphereID;
	mysphereID = glGenLists(1);
	glNewList(mysphereID, GL_COMPILE);
	glColor4f(0.8f, 0.9f, 0.9f, 1.0f);
	glScalef(1.0, 0.2, 1.0);
	gluSphere(sphere, 4.0, 20, 20);
	glColor4f(0.5f, 0.6f, 0.6f, 0.5f);
	glScalef(0.8, 3.5, 0.8);
	gluSphere(sphere, 2.0, 20, 20);
	glEndList();
	gluDeleteQuadric(sphere);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glEnable( GL_TEXTURE_2D );  
	glEnable(GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc ( GL_GREATER, 0 );
	glPushMatrix();
	glRotatef(90,0,1,0);
	spinRotate += spinRotateSpeed;
	if (spinRotate > 360){
		spinRotate = spinRotate-360;
	}
	glRotatef(spinRotate, 0, 1, 0);
	glCallList(mysphereID);
	glPopMatrix();  
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	//To fix the bug with font
	glDisable(GL_ALPHA_TEST);
}

GLvoid DrawSimplePlane(void)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Student:
    //
    // this is just a simple wing structure ( no texture etc. ) 
    // to help you get the game initially going
    //

    /* now draw the wings */	
    glRotatef(90,0,1,0);
    glDisable( GL_TEXTURE_2D );  // incase somebody left it one
    glDisable(GL_BLEND);         // incase somebody left it one

    glBegin(GL_QUADS);          // Start Drawing Quads
    {
        // Front Face
        glVertex3f(-4.0f, -0.3f,  0.6f);  // Bottom Left Of The Texture and Quad
        glVertex3f( 4.0f, -0.3f,  0.6f);  // Bottom Right Of The Texture and Quad
        glVertex3f( 4.0f,  0.3f,  0.6f);  // Top Right Of The Texture and Quad
        glVertex3f(-4.0f,  0.3f,  0.6f);  // Top Left Of The Texture and Quad
        // Back Face
        glVertex3f(-4.0f, -0.3f, -0.6f);  // Bottom Right Of The Texture and Quad
        glVertex3f(-4.0f,  0.3f, -0.6f);  // Top Right Of The Texture and Quad
        glVertex3f( 4.0f,  0.3f, -0.6f);  // Top Left Of The Texture and Quad
        glVertex3f( 4.0f, -0.3f, -0.6f);  // Bottom Left Of The Texture and Quad
        // Top Face
        glVertex3f(-4.0f,  0.3f, -0.6f);  // Top Left Of The Texture and Quad
        glVertex3f(-4.0f,  0.3f,  0.6f);  // Bottom Left Of The Texture and Quad
        glVertex3f( 4.0f,  0.3f,  0.6f);  // Bottom Right Of The Texture and Quad
        glVertex3f( 4.0f,  0.3f, -0.6f);  // Top Right Of The Texture and Quad
        // Bottom Face
        glVertex3f(-4.0f, -0.3f, -0.6f);  // Top Right Of The Texture and Quad
        glVertex3f( 4.0f, -0.3f, -0.6f);  // Top Left Of The Texture and Quad
        glVertex3f( 4.0f, -0.3f,  0.6f);  // Bottom Left Of The Texture and Quad
        glVertex3f(-4.0f, -0.3f,  0.6f);  // Bottom Right Of The Texture and Quad
        // Right face
        glVertex3f( 4.0f, -0.3f, -0.6f);  // Bottom Right Of The Texture and Quad
        glVertex3f( 4.0f,  0.3f, -0.6f);  // Top Right Of The Texture and Quad
        glVertex3f( 4.0f,  0.3f,  0.6f);  // Top Left Of The Texture and Quad
        glVertex3f( 4.0f, -0.3f,  0.6f);  // Bottom Left Of The Texture and Quad
        // Left Face
        glVertex3f(-4.0f, -0.3f, -0.6f);  // Bottom Left Of The Texture and Quad
        glVertex3f(-4.0f, -0.3f,  0.6f);  // Bottom Right Of The Texture and Quad
        glVertex3f(-4.0f,  0.3f,  0.6f);  // Top Right Of The Texture and Quad
        glVertex3f(-4.0f,  0.3f, -0.6f);  // Top Left Of The Texture and Quad
    }
    glEnd();                    // Done Drawing Quads
}

GLvoid BuildFlatHeightMap(){
    RgbImage heightMap( flatHeightImage );
 
    printf("HeightMap Dimensions<%ld %ld>\n", heightMap.GetNumRows(), heightMap.GetNumCols()); 
   
    /* Make the Data of the terrain */
	float tempRateX = sizeMap/xTerrainDimension;
	float tempRateZ = sizeMap/zTerrainDimension;
    for (int x = 0; x < sizeMap; x++){
	for(int z = 0; z < sizeMap; z++){
	    float currentheight1 = 0.0f;
	    float currentheight2 = 0.0f;
	    float currentheight3 = 0.0f;
	    // the heights in RGB are all the same, so we pick one.
	    heightMap.GetRgbPixel(x, z, &currentheight1, &currentheight2, &currentheight3);

	    int vindex = x*sizeMap+z;
	    float height = heightScaleFactor*currentheight1;

	    terrainVertex[vindex].y = height;
	    terrainVertex[vindex].x = (float) (x - sizeMap/2)*xTerrainDimension;          //   <<=============== fix calculations
	    terrainVertex[vindex].z = (float) (z - sizeMap/2)*zTerrainDimension;          //   <<=============== fix calculations

		//To enhance the performance, use 4 terrain texture for whole map.

	    terrainVertex[vindex].s0 = abs((x % (int)(sizeMap / xTerrainDimension*2+1)) - (float)(tempRateX))/(float)(tempRateX);         //   <================ fix calculations
	    terrainVertex[vindex].t0 = abs((z % (int)(sizeMap / zTerrainDimension*2+1)) - (float)(tempRateZ))/(float)(tempRateZ);         //   <================ fix calculations
		//terrainVertex[vindex].t0 = (float)z / (sizeMap - 1);         //   <================ fix calculations		
		
		if (x < (sizeMap - 1) && z < (sizeMap - 1))
	    {
		int index = 4*(z+ x*(sizeMap-1));               //   <================ fix calculations
		terrainIndex[index]     = vindex;        //   <================ fix calculations      
		terrainIndex[index + 1] = vindex + sizeMap;        //   <================ fix calculations      
		terrainIndex[index + 2] = vindex + sizeMap + 1;        //   <================ fix calculations      
		terrainIndex[index + 3] = vindex + 1;        //   <================ fix calculations      
	    }
	}
    }
    
    /* Calculate the normal vectors for all faces/vertexes */
    for(int i = 0; i < nFaces; i++){
		struct vector {
			float x, y, z;
			float length;
		};
		vector v1, v2, norm;
		v1.x = terrainVertex[terrainIndex[4*i+1]].x - terrainVertex[terrainIndex[4*i]].x;
		v1.y = terrainVertex[terrainIndex[4*i+1]].y - terrainVertex[terrainIndex[4*i]].y;
		v1.z = terrainVertex[terrainIndex[4*i+1]].z - terrainVertex[terrainIndex[4*i]].z;
		v2.x = terrainVertex[terrainIndex[4*i+3]].x - terrainVertex[terrainIndex[4*i]].x;
		v2.y = terrainVertex[terrainIndex[4*i+3]].y - terrainVertex[terrainIndex[4*i]].y;
		v2.z = terrainVertex[terrainIndex[4*i+3]].z - terrainVertex[terrainIndex[4*i]].z;
		vector A;
		A = v1;
        v1 = v2;
        v2 = A;
		norm.x = v1.y * v2.z - v1.z * v2.y;
		norm.y = v1.z * v2.x - v1.x * v2.z;
		norm.z = v1.x * v2.y - v1.y * v2.x;
		norm.length = pow ((norm.x * norm.x + norm.y * norm.y + norm.z * norm.z), (float) 0.5);
		norm.x /= norm.length;
		norm.y /= norm.length;
		norm.z /= norm.length;
		for (int j = 0; j < 4; j++){
			terrainVertex[terrainIndex[4*i+j]].nx = norm.x; 
			terrainVertex[terrainIndex[4*i+j]].ny = norm.y;
			terrainVertex[terrainIndex[4*i+j]].nz = norm.z; 
		}
    }
    
    glewInit();
    
    glGenBuffers(1, &VertexVBOID);
    glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex)*nVertex, &terrainVertex[0].x, GL_STATIC_DRAW);

    glGenBuffers(1, &IndexVBOID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*4*nFaces,terrainIndex, GL_STATIC_DRAW);
}

GLvoid BuildHeightMap()
{
    RgbImage heightMap( heightImage );
 
    printf("HeightMap Dimensions<%ld %ld>\n", heightMap.GetNumRows(), heightMap.GetNumCols()); 
   
    /* Make the Data of the terrain */
    for (int x = 0; x < sizeMap; x++){
	for(int z = 0; z < sizeMap; z++){
	    float currentheight1 = 0.0f;
	    float currentheight2 = 0.0f;
	    float currentheight3 = 0.0f;
	    // the heights in RGB are all the same, so we pick one.
	    heightMap.GetRgbPixel(x, z, &currentheight1, &currentheight2, &currentheight3);

	    int vindex = x*sizeMap+z;
	    float height = heightScaleFactor*currentheight1;

	    terrainVertex[vindex].y = height;
	    terrainVertex[vindex].x = (float) (x - sizeMap/2);          //   <<=============== fix calculations
	    terrainVertex[vindex].z = (float) (z - sizeMap/2);          //   <<=============== fix calculations

		//To enhance the performance, use 4 terrain texture for whole map.
	    terrainVertex[vindex].s0 = (float)x / (sizeMap - 1);         //   <================ fix calculations
	    terrainVertex[vindex].t0 = (float)z / (sizeMap - 1);         //   <================ fix calculations		

		if (x < (sizeMap - 1) && z < (sizeMap - 1))
	    {
		int index = 4*(z+ x*(sizeMap-1));               //   <================ fix calculations
		terrainIndex[index]     = vindex;        //   <================ fix calculations      
		terrainIndex[index + 1] = vindex + sizeMap;        //   <================ fix calculations      
		terrainIndex[index + 2] = vindex + sizeMap + 1;        //   <================ fix calculations      
		terrainIndex[index + 3] = vindex + 1;        //   <================ fix calculations      
	    }
	}
    }
    
    /* Calculate the normal vectors for all faces/vertexes */
    for(int i = 0; i < nFaces; i++){
		struct vector {
			float x, y, z;
			float length;
		};
		vector v1, v2, norm;
		v1.x = terrainVertex[terrainIndex[4*i+1]].x - terrainVertex[terrainIndex[4*i]].x;
		v1.y = terrainVertex[terrainIndex[4*i+1]].y - terrainVertex[terrainIndex[4*i]].y;
		v1.z = terrainVertex[terrainIndex[4*i+1]].z - terrainVertex[terrainIndex[4*i]].z;
		v2.x = terrainVertex[terrainIndex[4*i+3]].x - terrainVertex[terrainIndex[4*i]].x;
		v2.y = terrainVertex[terrainIndex[4*i+3]].y - terrainVertex[terrainIndex[4*i]].y;
		v2.z = terrainVertex[terrainIndex[4*i+3]].z - terrainVertex[terrainIndex[4*i]].z;
		vector A;
		A = v1;
        v1 = v2;
        v2 = A;
		norm.x = v1.y * v2.z - v1.z * v2.y;
		norm.y = v1.z * v2.x - v1.x * v2.z;
		norm.z = v1.x * v2.y - v1.y * v2.x;
		norm.length = pow ((norm.x * norm.x + norm.y * norm.y + norm.z * norm.z), (float) 0.5);
		norm.x /= norm.length;
		norm.y /= norm.length;
		norm.z /= norm.length;
		for (int j = 0; j < 4; j++){
			terrainVertex[terrainIndex[4*i+j]].nx = norm.x; 
			terrainVertex[terrainIndex[4*i+j]].ny = norm.y;
			terrainVertex[terrainIndex[4*i+j]].nz = norm.z; 
		}
    }
    
    glewInit();
    
    glGenBuffers(1, &VertexVBOID);
    glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex)*nVertex, &terrainVertex[0].x, GL_STATIC_DRAW);

    glGenBuffers(1, &IndexVBOID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*4*nFaces,terrainIndex, GL_STATIC_DRAW);
}

GLvoid DrawTerrain()
{
    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);      // reset the color
    glClearColor(0.5f, 0.8f, 1.0f, 0.0f);   // determine the color

    glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Student:
    // 
    // Study and complete the argument list of the following 3 functions.
    // they are going in concert with the 5 functions above. Use the BUFFER_OFFSET() macro
    // defined at the top (it should be clear for what once you read up on these functions
    
    glVertexPointer( 3, GL_FLOAT, sizeof(myVertex), BUFFER_OFFSET(0));
    glNormalPointer( GL_FLOAT, sizeof(myVertex), BUFFER_OFFSET(sizeof(GL_FLOAT)*3));
    glTexCoordPointer( 2, GL_FLOAT, sizeof(myVertex), BUFFER_OFFSET(sizeof(GL_FLOAT)*6) ); 
    

    // finally draw the elements ..... (no more required in this functon beyond

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Student:
    // 
    // You can only enable the DrawElement functions after you fixed the above 3 functions
    
    glDrawElements(GL_QUADS, (nFaces)*4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

GLvoid DrawThePlane()
{
    glPushMatrix();
    {
	glTranslatef(xrefCam,yrefCam,zrefCam);
	glRotatef(-phiPlane,0, 1, 0);
	glRotatef(animationRotate, 1, 0, 0);
	glRotatef(animationRotateZ, 0, 0, 1);
	if( animationL == true && animationRotate >= 0){
	    if(animationRotate < 45){
		animationRotate += 0.1;
	    }
	} else {
	    if(animationRotate > 0)
		animationRotate -= 0.1;
	}
	if( animationR == true  && animationRotate <= 0){
	    if(animationRotate > -45){
		animationRotate -= 0.1;
	    }
	} else {
	    if(animationRotate < 0)
		animationRotate += 0.1;
	}
	if ( animationD == true&& animationRotateZ >=0) {
		if (animationRotateZ < 45){
			animationRotateZ +=0.1;
		}
	}
	else{
		if(animationRotateZ >0)
			animationRotateZ -= 0.1;
	}
	if( animationU == true  && animationRotateZ <= 0){
	    if(animationRotateZ > -45){
		animationRotateZ -= 0.1;
	    }
	} else {
	    if(animationRotateZ < 0)
		animationRotateZ += 0.1;
	}
	
	glColor4f(0.0f, 1.0f, 0.0f, 0.0f);  // just make the plane green
	if (SHOWPLANEFLAG){
		if (planeTexIndex == -1){
			if (!cockPitFlag){
				DrawNicePlane();
			}
			else{
				cockPit();
			}
		}
		else
			DrawPlane();
		
		}
	}

    glPopMatrix();
	
}

void DrawDashBoard()
{
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);				// Select The Type Of Blending
    /* Information Display Text List */
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glColor3f(1.0f,1.0f,1.0f);

    sprintf(stringInf, "Heading:%.1f | Height:%.1f | Camera Follow[f]:%d | Light[l]:%d", 
	    phiPlane, yrefCam, followPlane, light);
    myPrint(texture, 2,10,460,stringInf,0);
	
	sprintf(stringInf, "Spiders Alive:%d | Shot by Missiles:%d | Explode by Mines:%d", 
		spiders.getAliveAmount(), spiders.getKilledByMissiles(), spiders.getKilledByMines() );
	myPrint(texture, 2,10,440,stringInf,0);
}

void loadTextureFromFile()
{
    /* Load Image */
    RgbImage theTexMap1( terrainTexFile );
    RgbImage theTexMap2( planeTexFile );
	RgbImage theTexMap3( explodeFile);
    RgbImage fontMap( fontImage );
    
    if (theTexMap1.GetErrorCode() != 0) {
	printf("inputfile <%s> not available or error in reading\n", terrainTexFile);
	std::exit(0);
    }
    if (theTexMap2.GetErrorCode() != 0) {
	printf("inputfile <%s> not available or error in reading\n", planeTexFile);
	std::exit(0);
    }
	if (theTexMap3.GetErrorCode() != 0) {
	printf("inputfile <%s> not available or error in reading\n", planeTexFile);
	std::exit(0);
    }
    if (fontMap.GetErrorCode() != 0) {
	printf("inputfile <%s> not available or error in reading\n", fontImage);
	std::exit(0);
    }
    
    glGenTextures(4, &texture[0]);
    
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, theTexMap1.GetNumCols(), theTexMap1.GetNumRows(), 
		 0, GL_RGB, GL_UNSIGNED_BYTE, theTexMap1.ImageData());
    
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, theTexMap2.GetNumCols(), theTexMap2.GetNumRows(), 
		 0, GL_RGB, GL_UNSIGNED_BYTE, theTexMap2.ImageData());
    
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);		// GL line
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		// GL line
    glTexImage2D(GL_TEXTURE_2D, 0, 3, fontMap.GetNumCols(), fontMap.GetNumRows(), 
		 0, GL_RGB, GL_UNSIGNED_BYTE, fontMap.ImageData());

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, theTexMap3.GetNumCols(), theTexMap3.GetNumRows(), 
		 0, GL_RGB, GL_UNSIGNED_BYTE, theTexMap3.ImageData());
}

void initSpiders(){
	srand ( time(NULL) );
	int tempX, tempZ, tempSize;
	for (int i = 0; i < spiderAmount; i++){
		tempX = rand();
		tempZ = rand();
		if (RANDOMSIZEFLAG){
			tempSize = rand()%4 -2;
		}
		else{
			tempSize = 0;
		}
		spiders.pushBackSpider(SpiderBuilder::createSpiderBuilder()
			.withMoveSpeedPara(SPIDERSPEED)
			.withVelocity(tempX%11-5, 0, tempZ%11-5)
			.withPosition(-3*aveSpiderSize*(i%5), 0, -3*aveSpiderSize*i/5)
			//.withAcceleration(0.005*SPIDERSPEED, 0.0*SPIDERSPEED, 0.002*SPIDERSPEED)
			.withDistanceBeforeRedirect(rand()%30+10)
			.withColor(rand()%3+1)
			.withSize(aveSpiderSize + tempSize)
			.build());
	}
}

int InitGL()
{
    loadTextureFromFile();

    BuildFont(texture,2);
	if (FLATFLAG){
		BuildFlatHeightMap();
	}
	else{
		BuildHeightMap();
	}

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);				// Select The Type Of Blending
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);	// set the Environment Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);	// set the Diffuse Light
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);// set the Position of Diffuse Light
    glEnable(GL_LIGHT1);
    
    if(light == true)
	glEnable(GL_LIGHTING);
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable( GL_COLOR_MATERIAL);
	
	GLUquadricObj *sphere=NULL;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);
    return true;
}

void drawMines(){
	glPushMatrix();
	for (int i = 0; i< mines.size(); i++){
		mines.at(i).draw();
	}
	glPopMatrix();
	int j;
	if (mineFrame % 50 == 0){
		for (j = 0; j < mines.size(); j++){
			if (mines.at(j).isTriggered(&spiders) || mines.at(j).isExploding()){
				mines.at(j).explode();
			}
			if (mines.at(j).isExplodingFinish()){
				mines.erase(mines.begin()+j);
				j--;
			}
		}
	}
	mineFrame ++;
	if (mineFrame >= 499){
		mineFrame = 0;
	}
}

/*
 * Draw the texture in the OpenGL graphics window
 */
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Select the model of drawing */
    if(wireFrameMode == false)
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
    else
	glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
    
    /* setup the camera */
    if (followPlane == true) {
	phi = phiPlane;
    }

	if (cockPitFlag){
		phi = phiPlane;
		theta = animationRotateZ;
	}
    
    GLfloat vwx = r*cos(c*theta)*cos(c*phi)+xCam;
    GLfloat vwy = r*sin(c*theta)+yCam;
    GLfloat vwz =  r*cos(c*theta)*sin(c*phi)+zCam;

    if ( (int)abs(theta)%360 > 90 && (int)abs(theta)%360 < 270)
	Vy =-1.0; 
    else
	Vy = 1.0;
    
	if (cockPitFlag){
		Vx = sin(-1*c*animationRotate);
		Vy = cos(-1*c*animationRotate);
	}

    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60, 1, 1, 200);
	if (cockPitFlag){
		gluLookAt(xrefCam, yrefCam, zrefCam, xrefCam-vwx, yrefCam-vwy, zrefCam-vwz, Vx, Vy, Vz); 
	}
	else {
		gluLookAt(vwx+xrefCam, vwy+yrefCam, vwz+zrefCam, xrefCam, yrefCam, zrefCam, Vx, Vy, Vz); 
	}

    DrawTerrain();

    DrawThePlane();
	
	DrawMissiles(missiles, terrainVertex, &spiders);
	drawMines();

	spiders.drawAll(texture[3]);
	spiders.collisionAvoidWanderAll((float)sizeMap/2*xTerrainDimension, (float)sizeMap/2*zTerrainDimension);
	
	DrawDashBoard();

    glFlush();
    glutSwapBuffers();
}

void resizeWindow(int width, int height)
{
    if (height == 0){
	height = 1;
    }
    glViewport(0,0,width,height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard (unsigned char key, int x, int y)
{
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y){
    keys[key] = false;
}

void processSpecialKeys(int key, int x, int y) 
{   
    specialKeys[key] = true;
}

void processSpecialUpKeys(int key, int x, int y)
{
    specialKeys[key] = false;
}

void idle(void)
{
	float terrainY = getYFromXZ (xrefCam, zrefCam, terrainVertex);	
	bool isColliding = isPlaneColliding(terrainY);
    if( keys[27] == true ){
		std::exit(0);
	}
	if (keys[' '] && !sp){
		sp = true;
		missiles.push_back(new missile(xrefCam, yrefCam, zrefCam, moveSpeed, phiPlane));
	}
	if (!keys[' ']){ sp = false;}

	if (keys['e'] && !ep){
		ep = true;
		explodeModeFlag = !explodeModeFlag;
	}
	if (!keys['e']) { ep = false;};

	if (keys['c'] && !cp){
		cp =true;
		cockPitFlag = !cockPitFlag;
	}
	if (!keys['c']){ cp = false;}
    if( keys['m'] && !mp){
	mp = true;
	wireFrameMode = !wireFrameMode;
    }
    if (!keys['m']){
	mp = false;
    }
    if( keys['l'] && !lp){
	lp = true;
	light = !light;
	if (light)	{ glEnable(GL_LIGHTING);  }
	else		{ glDisable(GL_LIGHTING); }
    }
    if(!keys['l']){ lp = false; }
    
    if( keys['f'] && !fp){
	fp = true;
	followPlane = !followPlane;
    }
    if(!keys['f']){ fp = false; }
    
    if( keys['w'] == true && !isColliding){
	xrefCam -= moveSpeed*cos(c*phiPlane);
	zrefCam -= moveSpeed*sin(c*phiPlane);
    }
    if( keys['s'] == true && !isColliding){
	xrefCam += moveSpeed*cos(c*phiPlane);
	zrefCam += moveSpeed*sin(c*phiPlane);
    }
    if( keys['a'] == true && !isColliding){
	animationL = true;
    }
    if( !keys['a']){ animationL = false; }
    
    if( keys['d'] == true && !isColliding){
	animationR = true;
    }
    if( !keys['d'] ){ animationR = false;}

	if (animationRotate > 1 && keys['a'] == true){
		phiPlane = phiPlane - rotateSpeed;
	}
	if (animationRotate < -1 && keys['w'] == true){
		phiPlane = phiPlane + rotateSpeed;
	}
	if (phiPlane < 0.0) phiPlane += 360.0;
	if (phiPlane >= 360.0) phiPlane -= 360.0;

    if ( specialKeys[GLUT_KEY_F1]){ r += 0.5; }
    if ( specialKeys[GLUT_KEY_F2]){ r -= 0.5; }
    
    if ( specialKeys[GLUT_KEY_UP] && !isColliding)  { 
		yrefCam += moveSpeed; 
		animationU = true;
	}
    if ( specialKeys[GLUT_KEY_DOWN] && !isColliding){ 
		yrefCam -= moveSpeed; 
		animationD = true;
	}

	if (!specialKeys[GLUT_KEY_UP])  { 
		animationU = false;
	}
	if (!specialKeys[GLUT_KEY_DOWN]){ 
		animationD = false;
	}
    
    drawScene();
}

void killResources()
{
    glDeleteBuffers(1, &VertexVBOID);
    glDeleteBuffers(1, &IndexVBOID);
    killFontMap();
}

void Mouse(int button, int state, int x, int y) //
{
    if(state == GLUT_DOWN){
        oldmx =x ;
	oldmy = y;
    }
    if(state == GLUT_UP && button == GLUT_WHEEL_UP)
    {
	r += 0.2;
	glutPostRedisplay();
    }
    if(state == GLUT_UP && button == GLUT_WHEEL_UP)
    {
	if( r > 2 ){
	    r -= 0.2;
	    glutPostRedisplay();
	}
    }
}

void onMouseMove(int x,int y) // process the movement 
{
    phi   += 0.5f*(x - oldmx); 
    theta += 0.5f*(y - oldmy); 
    oldmx = x;
    oldmy = y; 
}



int main(int argc, char** argv)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Terrain Simulation");

    // now get all the other program arguments 
	/*
    int opt;
    while ((opt = getopt(argc, argv, "t:p:")) != -1) {
	switch (opt) {
	case 't':
	    terrainTexIndex = atoi(optarg);
	    break;
	case 'p':
	    planeTexIndex = atoi(optarg);
	    break;
	}
    }
	*/

	int i = 0;
	string str_argv, str_terrainTexIndex, str_planeTexIndex, str_spiderAmount, str_spiderSpeed, 
		str_randomSize, str_spiderSize, str_mineAmount, str_showPlane, str_notFlatMap;
	str_terrainTexIndex.assign ("-t");
	str_planeTexIndex.assign ("-p");
	str_spiderAmount.assign("-n");
	str_spiderSpeed.assign("-s");
	str_randomSize.assign("-rs");
	str_spiderSize.assign("-aves");
	str_mineAmount.assign("-m");
	str_showPlane.assign("-sp");
	str_notFlatMap.assign("-nf");
	//default value:
	terrainTexIndex=3;
	planeTexIndex=-1;
	spiderAmount = 20;
	SPIDERSPEED = 0.05;
	mineAmount = 10;
	//end of default value
   for(i = 0; i < argc; i++){
		str_argv.assign (argv[i]);
		if (!str_argv.compare (str_terrainTexIndex)){
			if ( i +1 < argc){
				terrainTexIndex = atoi(argv[i+1]);
				continue;
			}
			else {
				throw "invalid terrainTexIndex";
			}
		}
		if (!str_argv.compare (str_planeTexIndex)){
			if ( i +1 < argc){
				planeTexIndex = atoi(argv[i+1]);
				continue;
			}
			else {
				throw "invalid planeTexIndex";
			}
		}
		if (!str_argv.compare (str_spiderAmount)){
			if ( i + 1 < argc && atoi(argv[i+1])>=0){
				spiderAmount = atoi(argv[i+1]);
				continue;
			}
			else {
				throw "invalid spiderAmount";
			}
		}
		if (!str_argv.compare (str_spiderSize)){
			if ( i + 1 < argc && atoi(argv[i+1])>=0){
				aveSpiderSize = (float)atoi(argv[i+1]);
				if (aveSpiderSize<3){
					aveSpiderSize = 3;
				}
				if (aveSpiderSize>10){
					aveSpiderSize = 10;
				}
				continue;
			}
			else {
				throw "invalid spiderAveSize";
			}
		}	
		if (!str_argv.compare (str_spiderSpeed)){
			if ( i + 1 < argc && atoi(argv[i+1])>0){
				SPIDERSPEED = (float)atoi(argv[i+1]) / 1000;
				continue;
			}
			else {
				throw "invalid spiderSpeed";
			}
		}
		if (!str_argv.compare (str_randomSize)){
			RANDOMSIZEFLAG = true;
			continue;
		}
		if (!str_argv.compare (str_notFlatMap)){
			FLATFLAG = false;
			continue;
		}
		if (!str_argv.compare (str_showPlane)){
			SHOWPLANEFLAG = true;
			continue;
		}
		if (!str_argv.compare (str_planeTexIndex)){
			if ( i +1 < argc){
				mineAmount = atoi(argv[i+1]);
				continue;
			}
			else {
				throw "invalid mine amount";
			}
		}
   }
	
    if ((planeTexIndex < -1) || (planeTexIndex >= (int)(sizeof(filenames)/sizeof(char*))) ||
	(terrainTexIndex < 0) || (terrainTexIndex >= (int)sizeof(filenames)/sizeof(char*)))
    {
	printf("wrong texture selection %d %d %d\n", planeTexIndex, terrainTexIndex, 
	       sizeof(filenames)/sizeof(char*));
	std::exit(0);
    }
    printf("%d %d\n",planeTexIndex, terrainTexIndex);
    terrainTexFile = filenames[terrainTexIndex];
    planeTexFile   = filenames[(planeTexIndex == -1) ? 0 : planeTexIndex];
	explodeFile = filenames[7];
    
    InitGL();
	if (planeTexIndex == -1){
		InitNicePlane();
		initFireBall();
	}
	else {
		InitPlane();
	}

	InitCockPit();
	initSpiders();
	initMines();


    glutDisplayFunc(drawScene);
    glutReshapeFunc(resizeWindow);
    glutIdleFunc(&idle);
    glutMouseFunc(Mouse);
    glutMotionFunc(onMouseMove);
    glutSpecialFunc(processSpecialKeys);
    glutSpecialUpFunc(processSpecialUpKeys);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    
    glutMainLoop();
    
    killResources();
    return 0; 
}
