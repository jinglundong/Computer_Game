#include "Spider.h"
#include "Spiders.h"
#include <stdlib.h>
#include <gl\glew.h>
#include <gl\glut.h>	
#include <cmath>

Spider::Spider(void)
{
	position = MyVectorBuilder::createMyVectorBuilder().withX(0).withY(0).withZ(0).build();
	velocity = MyVectorBuilder::createMyVectorBuilder().withX(0).withY(0).withZ(0).build();
	acceleration = MyVectorBuilder::createMyVectorBuilder().withX(0).withY(0).withZ(0).build();
	size = 2.0;
	distanceBeforeRedirect = 10.0;
	color = black;
}

Spider::~Spider(void)
{
}

SpiderBuilder SpiderBuilder::withPosition(float x, float y, float z){
	this->spider.position = MyVectorBuilder::createMyVectorBuilder().withX(x).withY(y).withZ(z).build();
	return *this;
}

SpiderBuilder SpiderBuilder::withVelocity(float x, float y, float z){
	this->spider.velocity = MyVectorBuilder::createMyVectorBuilder().withX(x).withY(y).withZ(z).build();
	return *this;
}

SpiderBuilder SpiderBuilder::withAcceleration(float x, float y, float z){
	this->spider.acceleration = MyVectorBuilder::createMyVectorBuilder().withX(x).withY(y).withZ(z).build();
	return *this;
}

SpiderBuilder SpiderBuilder::withSize(float size){
	if (size >0){
		this->spider.size = size;
		return *this;
	}
	else{
		throw "size of spider must be a positive number";
	}
}

SpiderBuilder SpiderBuilder::withDistanceBeforeRedirect(float distanceBeforeRedirect){
	this->spider.distanceBeforeRedirect = distanceBeforeRedirect;
	return *this;
}

SpiderBuilder SpiderBuilder::withColor(int newColor){
	int enumSize = Color_count;
	if (newColor < enumSize && newColor >= 0){
		this->spider.color = newColor;
		return *this;
	}
	else{
		throw "invalid color";
	}
}

SpiderBuilder SpiderBuilder::withMoveSpeedPara(float moveSpeedPara){
	if (moveSpeedPara > 0){
		this->moveSpeedPara = moveSpeedPara;
		return *this;
	}
	else{
		throw "negative speed";
	}
}

SpiderBuilder SpiderBuilder::withRandomVelocity(int rangeX, int rangeZ){
	int tempX = rand()%rangeX-0.5*rangeX;
	int tempZ;
	while(tempX == rand()%rangeX-0.5*rangeX){
		tempZ = rand()%rangeZ-0.5*rangeZ;
	}
	this->spider.velocity = MyVectorBuilder::createMyVectorBuilder()
		.withX(tempX)
		.withZ(tempZ)
		.build();
	this->spider.collideFlag = false;
	return *this;
}

void Spider::explode(GLuint explodeTexture){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, explodeTexture);

	glPushMatrix();
	float r = 1 + this->explodeFrame * 0.05;
	//float r = pow((float)1.02,(this->explodeFrame+1));
	glScalef(r, r, r);

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
		this->explodeFinishFlag = true;
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	//To fix the bug with font
}

Spider SpiderBuilder::build(void){ 
	if (!done) {
		this->spider.velocity = this->spider.velocity * this->moveSpeedPara;
		this->spider.acceleration = this->spider.acceleration * this->moveSpeedPara;
		this->spider.moveSpeedPara = this->moveSpeedPara;
		this->spider.explodeFrame = 0;
		this->spider.explodeFinishFlag = false;
		this->spider.killedByMine = false;
		this->spider.killedByMissile = false;
		//generate a different velocity
		int tempX = rand()%11-5;
		int tempZ= rand()%11-5;

		if (tempX == 0 || this->spider.velocity.getX() == 0){					//avoid divide by 0
			while(abs(tempZ - this->spider.velocity.getZ()) < 0.01){
				tempZ = rand()%11-5;
			}
		}
		else{
			while(abs(tempZ/tempX - this->spider.velocity.getZ()/this->spider.velocity.getX()) < 0.01){
				tempZ = rand()%11-5;
			}
		}
		this->spider.nextVelocity = MyVectorBuilder::createMyVectorBuilder()
								.withX(tempX*this->moveSpeedPara)
								.withZ(tempZ*this->moveSpeedPara)
								.build();
		this->done = true; 
		return spider;
	} 
	else{
		throw "please use a new builder";
	} 
}

SpiderBuilder::SpiderBuilder(){
	Spider spider();
	this->spider.frame = 0;
	Leg l = Leg(-10, 50, 0.55, 0.5);
	this->spider.legs.push_back(l);
	l = Leg(5, 40, 0.52, 0.4);
	this->spider.legs.push_back(l);
	l = Leg(-10, 50, 0.52, 0.4);
	this->spider.legs.push_back(l);
	l = Leg(10, 45, 0.6, 0.5);
	this->spider.legs.push_back(l);
	this->spider.isRedirecting = false;
	this->spider.redirectFinish = false;
	this->spider.aliveFlag = true;
	this->done = false;
}

void Spider::legAnimation(int totalFrames){
	if (totalFrames <50){
		totalFrames = 50;
	}
	float tmpAngle = 0;
	for (int i = 0; i < 4; i++){
		tmpAngle = this->legs.at(i).getInitAngle(0) +frame%totalFrames;
		if (tmpAngle >= totalFrames){
			tmpAngle -= totalFrames;
		}
		if (tmpAngle < 0){
			tmpAngle += totalFrames;
		}
		tmpAngle = abs(tmpAngle - 0.5*totalFrames);
		tmpAngle = tmpAngle / (0.5*totalFrames) * 20 - 10;
		this->legs.at(i).setAngle(0, tmpAngle);
	}
	this->frame++;
	if (frame >= totalFrames){
		frame -= totalFrames;
	}
}

void Spider::draw(GLuint explodeTexture){
	if (!this->isAlive() || explodeFinishFlag){
		if (!this->isAlive() && !explodeFinishFlag){
			glPushMatrix();
			glTranslatef(this->position.getX(),this->position.getY(),this->position.getZ());
			glColor3f(1, 1, 0);
			this->explode(explodeTexture);
			glPopMatrix();
		}
	}
	else{
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, explodeTexture);
		glTranslatef(this->getPosition().getX(), this->getPosition().getY() + 0.5 * this->getSize(), this->getPosition().getZ());
		float tmpNorm = this->getVelocity().getNorm();
		if (tmpNorm < 0.00001){
			tmpNorm = 1.0;
		}
		//avoid divide by zero
		if (abs (this->getVelocity().getZ()) < 0.0000001){
			this->velocity = MyVectorBuilder::createMyVectorBuilder()
				.withX(this->velocity.getX())
				.withZ(0.0000001)
				.build();
		}
		if (this->getVelocity().getZ() > 0){
			glRotatef(atan(this->getVelocity().getX()/this->getVelocity().getZ())/CONVERTER, 0, 1, 0);
		}
		else{
			glRotatef(atan(this->getVelocity().getX()/this->getVelocity().getZ())/CONVERTER + 180, 0, 1, 0);
		}
		switch (this->color){
		case black:
			glColor3f(0.0, 0.0, 0.0);
			break;
		case red:
			glColor3f(1.0, 0.0, 0.0);
			break;
		case green:
			glColor3f(0.0, 1.0, 0.0);
			break;
		case blue:
			glColor3f(0.0, 0.0, 1.0);
			break;
		default:
			glColor3f(0.0, 0.0, 0.0);
		}
		//glutWireCube(this->getSize());

		float tmpSize = this->getSize();
		GLUquadricObj *sphere=NULL;
		sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluQuadricTexture(sphere, GL_TRUE);
		gluQuadricNormals(sphere, GLU_SMOOTH);

		//body
		glPushMatrix();
		glTranslatef(0, 0, tmpSize*0.05);
		glScalef(tmpSize/4.2, tmpSize/4.2, tmpSize/4.2);
		gluSphere(sphere, 1.0, 20, 20);
		glPopMatrix();

		//head
		glPushMatrix();
		glTranslatef(0, 0, tmpSize*0.32);
		glScalef(tmpSize/7, tmpSize/7, tmpSize/7);
		gluSphere(sphere, 1.0, 20, 20);
		glPopMatrix();
	
		//tail?
		glPushMatrix();
		glTranslatef(0, tmpSize*0.17, tmpSize*(-0.32));
		glScalef(tmpSize/3.2, tmpSize/3.2, tmpSize/3.2);
		gluSphere(sphere, 1.0, 20, 20);
		glPopMatrix();

		//legs
		glPushMatrix();
		glTranslatef(0, 0, tmpSize*0.2);
		for (int i = 0; i< 4; i++){
			//left
			glPushMatrix();
			glRotatef(70+20*i, 0, 1, 0);
			glRotatef(legs.at(i).getAngle(0), 1, 0, 0);
			gluCylinder(sphere, tmpSize*0.02, tmpSize*0.05, tmpSize*legs.at(i).getLength(0), 16, 16);
			glTranslatef(0, 0, tmpSize*legs.at(i).getLength(0));
			glRotatef(legs.at(i).getAngle(1), 1, 0, 0);
			glutSolidCone(tmpSize*0.05, tmpSize*0.4, 16, 16);
			glPopMatrix();
			//right
			glPushMatrix();
			glRotatef(-70-20*i, 0, 1, 0);
			glRotatef(legs.at(i).getAngle(0), 1, 0, 0);
			gluCylinder(sphere, tmpSize*0.02, tmpSize*0.05, tmpSize*legs.at(i).getLength(0), 16, 16);
			glTranslatef(0, 0, tmpSize*legs.at(i).getLength(0));
			glRotatef(legs.at(i).getAngle(1), 1, 0, 0);
			glutSolidCone(tmpSize*0.05, tmpSize*0.4, 16, 16);
			glPopMatrix();
		}
		glPopMatrix();
		//end of legs

		gluDeleteQuadric(sphere);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
}

void Spider::walk(){
	this->velocity = this->velocity + this->acceleration;
	this->position = this->position + this->velocity;
	this->distanceBeforeRedirect -= this->velocity.getNorm();
	this->legAnimation(1/moveSpeedPara + 50 - this->velocity.getNorm() * 100);
}

void Spider::redirect(MyVector newVelocity){
	this->legAnimation(1/moveSpeedPara + 30);
	if (!this->isRedirecting){											//start to redirect			
			this->distanceBeforeRedirect = rand()%20+10;	
			this->nextVelocity = newVelocity;
			this->velocity = this->velocity / this->velocity.getNorm();
			this->isRedirecting = true;	
	}
	else{																//keep going until achieve the aim velocity
		//avoid divide by zero
		bool needRotateFlag = false;
		if (this->getVelocity().getX() == 0 || this->nextVelocity.getX() == 0){
			if(abs(asin(this->getVelocity().getX()/this->getVelocity().getNorm()) - 
				asin(this->nextVelocity.getX()/this->nextVelocity.getNorm())) > 5.0){
					needRotateFlag = true;
			}
		}		
		else{
			if (abs(atan(this->velocity.getZ()/this->velocity.getX())/CONVERTER -			//different angle or different sign of x or z
				atan(this->nextVelocity.getZ()/this->nextVelocity.getX())/CONVERTER)>5.0 ||				//need to rotate.
				this->velocity.getZ() * this->nextVelocity.getZ() < 0 ||
				this->velocity.getX() * this->nextVelocity.getX() < 0 ){
					needRotateFlag = true;
			}
		}
		if (needRotateFlag){
				//check clockwise or anti-clockwise
			float cur_degree = 0;						//range: -90 ~ 270
			float new_degree = 0;						//range: -90 ~ 270
			if (this->velocity.getX() < 0){
				cur_degree = atan(this->velocity.getZ()/this->velocity.getX())/CONVERTER + 180;
			}
			else{
				cur_degree = atan(this->velocity.getZ()/this->velocity.getX())/CONVERTER;
			}
			if (this->nextVelocity.getX() < 0){
				new_degree = atan(this->nextVelocity.getZ()/this->nextVelocity.getX())/CONVERTER + 180;
			}
			else{
				new_degree = atan(this->nextVelocity.getZ()/this->nextVelocity.getX())/CONVERTER;
			}						
			if (cur_degree < 0) cur_degree += 360;
			if (new_degree < 0) new_degree += 360;
			//check if should rotate clockwise or anti-clockwise
			if ((new_degree<cur_degree && new_degree>cur_degree-180) || new_degree > cur_degree+180){
				this->velocity.rotateAroundY(CONVERTER*-10*this->moveSpeedPara);			//anti-clockwise	
			}
			else{
				this->velocity.rotateAroundY(CONVERTER*10*this->moveSpeedPara);				//clockwise	
			}
		}
		else{																				//rotate finsh, now accelery to given speed
			this->velocity = this->nextVelocity;
			this->isRedirecting = false;
		}
	}
}

//subroutine of redirect
MyVector Spider::ComputeNewDirection(){
	MyVector result = MyVectorBuilder::createMyVectorBuilder().build();
	int tempX = rand()%11-5;
	while(tempX == 0){
		tempX = rand()%11-5;
	}
	int tempZ;
	tempZ = rand()%11-5;
	while(tempZ == 0){
		tempZ = rand()%11-5;
	}
	if (this->velocity.getX() == 0){					//avoid divide by 0
		MyVector tmpVector;
		tmpVector =  MyVectorBuilder::createMyVectorBuilder()
			.withX (0.01)
			.withZ(this->velocity.getZ())
			.build();
		this->velocity = tmpVector;
	}
	while(abs(tempZ/tempX - this->velocity.getZ()/this->velocity.getX()) < 0.01){
		tempZ = rand()%11-5;
		while(tempZ == 0){
			tempZ = rand()%11-5;
		}
	}
	result = MyVectorBuilder::createMyVectorBuilder()
				.withX(tempX*this->moveSpeedPara)
				.withZ(tempZ*this->moveSpeedPara)
				.build();
	return result;
}

bool Spider::checkNeedToRedirect(){
	//collision detection is done by class Spiders not Spider
	if (this->distanceBeforeRedirect < 0.1 || this->isRedirecting == true){
		return true;
	}
	else{
		return false;
	}
}

void MyVector::rotateAroundY(float radians){
	float resultX = cos(radians)*this->x - sin(radians)*this->z; 
	float resultZ = sin(radians)*this->x + cos(radians)*this->z; 
	this->x = resultX; 
	this->z = resultZ;
}

float MyVector::distance(MyVector theOtherVector){
	return pow(
		pow(this->getX()-theOtherVector.getX(),int(2))+
		pow(this->getY()-theOtherVector.getY(),int(2))+
		pow(this->getZ()-theOtherVector.getZ(),int(2))
		, (float) 0.5);
}

MyVector Spider::computeOpposite(){
	MyVector result = MyVectorBuilder::createMyVectorBuilder().build();
	result = MyVectorBuilder::createMyVectorBuilder()
		.withX(this->getVelocity().getX()*(-0.9))
		.withZ(this->getVelocity().getZ()*(-0.9))
		.build();
	return result;
}