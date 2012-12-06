#pragma once
#include <math.h>
#include <time.h>
#include <vector>
#	include <gl\glew.h>
#	include <gl\glut.h>	

enum Color{black, red, green, blue, Color_count};
const float	CONVERTER = 3.1415926/180.0f;	// Transform from angle to radian

class MyVector{
private:
	float x;
	float y;
	float z;
public:
	float getNorm(){return pow(x*x+y*y+z*z, (float)0.5);};
	friend class MyVectorBuilder;
	~MyVector(void){};
	float getX(){return this->x;};
	float getY(){return this->y;};
	float getZ(){return this->z;};
	MyVector operator+ (MyVector added){this->x+=added.getX();this->y+=added.getY();this->z+=added.getZ(); return *this;};
	MyVector operator* (MyVector added){this->x*=added.getX();this->y*=added.getY();this->z*=added.getZ(); return *this;};
	MyVector operator/ (MyVector added){this->x/=added.getX();this->y/=added.getY();this->z/=added.getZ(); return *this;};
	MyVector operator* (float added){this->x*=added;this->y*=added;this->z*=added; return *this;};
	MyVector operator/ (float added){this->x/=added;this->y/=added;this->z/=added; return *this;};
	bool operator== (MyVector val){bool result = true; if (this->x != val.getX()) return false; if (this->y != val.getY()) return false; if (this->z != val.getZ()) return false; return true;};
	void rotateAroundY(float radians);
	float distance(MyVector theOtherVector);
};

class MyVectorBuilder{
	MyVector myVector;
	bool done;
	MyVectorBuilder(){myVector.x = 0; myVector.y = 0; myVector.z = 0; done = false;};
public:
	static MyVectorBuilder createMyVectorBuilder(){
		return MyVectorBuilder();
	}
	MyVectorBuilder withX(float x){myVector.x = x; return *this;};
	MyVectorBuilder withY(float y){myVector.y = y; return *this;};
	MyVectorBuilder withZ(float z){myVector.z = z; return *this;};
	MyVector build(void){ if (!done) {this->done = true; return myVector;} else{ throw "please use a new builder";} };
};

class Leg{
	float angle[2];
	float length[2];
	float initAngle[2];
public:
	Leg (float angle1, float angle2, float length1, float length2) { this->initAngle[0] = angle1; this->angle[0] = angle1; this->initAngle[1] = angle2; this->angle[1] = angle2; this->length[0] = length1; this->length[1] = length2;};
	void setAngle(int num, float a){angle[num] = a;};
	void setInitAngle(int num, float a) {initAngle[num] = a;};
	float getInitAngle(int num){return initAngle[num];};
	float getAngle(int num){return angle[num];};
	float getLength(int num){return length[num];};
};

class Spider
{
	Spider(void);
	MyVector position;
	MyVector velocity;
	MyVector acceleration;
	float size;
	float distanceBeforeRedirect;
	int color;
	float moveSpeedPara;
	bool isRedirecting;
	bool redirectFinish;
	int redirectFrame;
	MyVector velocityCache;
	MyVector nextVelocity;									//next velocity for redirect
	bool collideFlag;
	std::vector<Leg> legs;
	void legAnimation(int totalFrames);
	int frame;												//leg, animation frame
	bool aliveFlag;
	int explodeFrame;
	bool explodeFinishFlag;
	bool killedByMine;
	bool killedByMissile;
public:
	friend class SpiderBuilder;
	bool checkNeedToRedirect();								//check if the spider need to redirect
	~Spider(void);
	void setCollideFlag(bool newValue){this->collideFlag = newValue;};
	float getMoveSpeedPara(){return this->moveSpeedPara;};
	bool getCollideFlag(){return this->collideFlag;};
	void draw(GLuint explodeTexture);
	void stop(){};
	void redirect();										//randomly
	void redirect(MyVector newVelocity);					//with a given direction, default distanceBeforeRedirect == 10
	MyVector computeOpposite();
	MyVector getPosition(){return this->position;};
	MyVector getVelocity(){return this->velocity;};
	MyVector getAcceleration(){return this->acceleration;};
	float getSize(){return this->size;};
	MyVector ComputeNewDirection();							//random new direction
	bool getIsRedirectingFlag(){return this->isRedirecting;};
	void setNextVelocity(MyVector next){this->nextVelocity = next;};
	void setIsRedirectingFlag(bool val){this->isRedirecting = val;};
	void walk();
	MyVector getNextVelocity(){return this->nextVelocity;};
	bool isAlive(){return this->aliveFlag;};
	void die(){this->aliveFlag = false;}; 
	void die(int killedType){if (killedType == 0) this->killedByMine = true; else this->killedByMissile = true; die();};	//type 0 == mine
	void explode(GLuint explodeTexture);
	int getExplodeFrame(){return explodeFrame;};
	bool isKilledByMine(){return killedByMine;};
	bool isKilledByMissile(){return killedByMissile;};
};

class SpiderBuilder{
private:
	Spider spider;
	bool done;
	SpiderBuilder(void);
	float moveSpeedPara;
public:
	static SpiderBuilder createSpiderBuilder(){return SpiderBuilder();};
	SpiderBuilder withPosition(float x, float y, float z);
	virtual void withRandomPositionIn(float x1, float y1, float z1, float x2, float y2, float z2){};
	SpiderBuilder withVelocity(float x, float y, float z);
	SpiderBuilder withRandomVelocity(int rangeX, int rangeZ);								//y always be 0
	SpiderBuilder withAcceleration(float x, float y, float z);
	SpiderBuilder withSize(float size);
	SpiderBuilder withDistanceBeforeRedirect(float distanceBeforeRedirect);
	SpiderBuilder withColor(int color);
	SpiderBuilder withMoveSpeedPara(float moveSpeedPara);
	Spider build(void);
};

