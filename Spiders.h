#pragma once

#include "Spider.h"
#include <vector>
#	include <gl\glew.h>
#	include <gl\glut.h>	

class Spiders
{
	std::vector<Spider> spiderList;
	int alive;
	int killedByMines;
	int killedByMissiles;
public:
	Spiders(void);
	void pushBackSpider(Spider spider){spiderList.push_back(spider);};
	void drawAll(GLuint explodeTexture);
	bool isColliding(int spiderNum1, int spiderNum2);
	void collisionAvoidWanderAll(float xMax, float zMax);
	~Spiders(void);
	std::vector<Spider> getSpiderList(){return this->spiderList;};
	void die(int num){if (num>=0 && num<this->spiderList.size()) this->spiderList.at(num).die();};
	void die(int num, int killedType){if (num>=0 && num<this->spiderList.size()) this->spiderList.at(num).die(killedType);};
	int getAliveAmount(){return this->alive;};
	int getKilledByMines(){return this->killedByMines;};
	int getKilledByMissiles(){return this->killedByMissiles;};
};

