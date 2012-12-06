#include "Spiders.h"


Spiders::Spiders(void)
{
}


Spiders::~Spiders(void)
{
}

bool Spiders::isColliding(int spiderNum1, int spiderNum2){
	if (spiderNum1 == spiderNum2){
		return false;
	}
	if (spiderNum1<0 || spiderNum2<0 || spiderNum1>=this->spiderList.size() ||spiderNum2>=this->spiderList.size()){
		return false;
	}
	MyVector pos1 = this->spiderList.at(spiderNum1).getPosition();
	MyVector pos2 = this->spiderList.at(spiderNum2).getPosition();
	if (pos1.distance(pos2) * 0.8<= this->spiderList.at(spiderNum1).getSize() +this->spiderList.at(spiderNum2).getSize()){
		return true;
	}
	else{
		return false;
	}
}

void Spiders::drawAll(GLuint explodeTexture){
	int tmpAlive = 0;
	int tmpKilledByMine = 0;
	int tmpKilledByMissiles = 0;
	for (int i = 0; i< spiderList.size(); i++){
		spiderList.at(i).draw(explodeTexture);
		if (spiderList.at(i).isAlive()){
			tmpAlive++;
		}
		else{
			if (spiderList.at(i).isKilledByMine()){
				tmpKilledByMine++;
			}
			else{
				tmpKilledByMissiles++;
			}
		}
	}
	this->alive = tmpAlive;
	this->killedByMines = tmpKilledByMine;
	this->killedByMissiles = tmpKilledByMissiles;
}

void Spiders::collisionAvoidWanderAll(float xBoundry, float zBoundry){
	MyVector tmp = MyVectorBuilder::createMyVectorBuilder().build();
	bool tmpCollideFlag = false;
	for (int i=0; i< (int)this->spiderList.size(); i++){
		//stay if it's dead
		if(!this->spiderList.at(i).isAlive()){
			continue;
		}
		if (!this->spiderList.at(i).isAlive()){
			//this->spiderList.at(i).explode(explodeTexture);
			continue;
		}
		//check if spider is outside the map
		if (abs(this->spiderList.at(i).getPosition().getX()) > xBoundry - this->spiderList.at(i).getSize()
			|| abs(this->spiderList.at(i).getPosition().getZ()) > zBoundry - this->spiderList.at(i).getSize()){
			if (this->spiderList.at(i).getPosition().getX() > xBoundry - this->spiderList.at(i).getSize()){
				tmp = MyVectorBuilder::createMyVectorBuilder().withZ(0).withX(abs(this->spiderList.at(i).getVelocity().getX())*-1).build();
			}
			if (this->spiderList.at(i).getPosition().getX() < -1* xBoundry + this->spiderList.at(i).getSize()){
				tmp = MyVectorBuilder::createMyVectorBuilder().withZ(0).withX(abs(this->spiderList.at(i).getVelocity().getX())*1).build();
			}
			if (this->spiderList.at(i).getPosition().getZ() > zBoundry - this->spiderList.at(i).getSize()){
				tmp = MyVectorBuilder::createMyVectorBuilder().withX(0).withZ(abs(this->spiderList.at(i).getVelocity().getZ())*-1).build();
			}
			if (this->spiderList.at(i).getPosition().getZ() < -1* zBoundry + this->spiderList.at(i).getSize()){
				tmp = MyVectorBuilder::createMyVectorBuilder().withX(0).withZ(abs(this->spiderList.at(i).getVelocity().getZ())*1).build();
			}
			if (this->spiderList.at(i).getIsRedirectingFlag()){			//is redirecting
				this->spiderList.at(i).setNextVelocity(tmp);
				this->spiderList.at(i).redirect(tmp);
			}
			else{														//is moving
				if (this->spiderList.at(i).getNextVelocity() == tmp){	//moving after rotate
					this->spiderList.at(i).walk();
				}
				else{													//moving before rotate, need to redirect
					this->spiderList.at(i).redirect(tmp);
				}
			}
		}
		tmpCollideFlag = false;
		for (int j = 0; j <(int)this->spiderList.size(); j++){
			if (i == j){
				continue;
			}
			if (this->isColliding(i,j)){
				tmpCollideFlag = true;
				if (!this->spiderList.at(i).getCollideFlag()){
					this->spiderList.at(i).setCollideFlag(true);					
				}
				tmp = (this->spiderList.at(i).getPosition() + this->spiderList.at(j).getPosition() * -1) * this->spiderList.at(i).getMoveSpeedPara();					
				break;
			}
		}
		if (tmpCollideFlag){											//collision
			if (this->spiderList.at(i).getIsRedirectingFlag()){			//is redirecting
				this->spiderList.at(i).setNextVelocity(tmp);
				this->spiderList.at(i).redirect(tmp);
			}
			else{														//is moving
				if (this->spiderList.at(i).getNextVelocity() == tmp){	//moving after rotate
					this->spiderList.at(i).walk();
				}
				else{													//moving before rotate, need to redirect
					this->spiderList.at(i).redirect(tmp);
				}
			}
		}
		else{															//non-collision, just walking and random redirection
			if (this->spiderList.at(i).checkNeedToRedirect()){
				MyVector tmpVector = this->spiderList.at(i).ComputeNewDirection();
				this->spiderList.at(i).redirect(tmpVector);
			}
			else{
				this->spiderList.at(i).walk();
			}
		}
	}
}
