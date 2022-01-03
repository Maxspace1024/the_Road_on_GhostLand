﻿#pragma once
#include<Windows.h>
#include<GL/glew.h>
#include<GL/freeglut.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include"basicFunc.h"
#include <vector>
#include <set>
using namespace std;

enum Status { START, GAME, DEAD, TIMEUP, END, MAIN_MENU, DEBUG };

class Player{
public:
	GLfloat pos[3];	//Position
	GLfloat LR_MOVE = 1; //玩家每次移動左右的距離
	GLfloat v = 0.01; //玩家移動速度
	GLfloat maxV = 1; //玩家移動速度
	GLfloat minV = 0.01; //玩家移動速度
	GLfloat shift = 0; //玩家與相機的位移
	Status status = MAIN_MENU; //遊戲目前的狀態

	Player(){
		memset(this->pos, 0, 3);
	}
	~Player(){}

	//鏡頭
	void lookAt() {
		gluLookAt(
			pos[0], pos[1] + 5, pos[2] + 10,
			pos[0], pos[1], pos[2] - 17,
			0, 1, 0);
	}

	//畫出player
	void drawPlayer(){
		glPushMatrix();
		{
			glTranslatef(pos[0], pos[1], pos[2] - this->shift);
			glScalef(1,2,1);
			glutSolidCube(1);
		}
		glPopMatrix();
	}

	//player鍵盤功能
	void kb(unsigned char key, int x, int y) {
		if (key == 'w') {
			//速度
			if (v + 0.002 < maxV) v += 0.002;
			else v = maxV;
			//相機位移
			if (this->shift + 0.3 < 3) this->shift += 0.3;
			else this->shift = 3;
		}
		else if (key == 's') {
			if (v - 0.07 > minV) v -= 0.07;
			else v = minV;

			if (v > 0) {
				if (this->shift - 0.1 > 0) this->shift -= 0.1;
				else this->shift = 0;
			}
			
		}
		else if (key == 'a') {
			pos[0] -= LR_MOVE;
		}
		else if (key == 'd') {
			pos[0] += LR_MOVE;
		}
	}

	//自動前進
	void Progress() {
		pos[2] -= v;
	}

	//停止
	void playerStop(int finalPos) {
		this->pos[2] = finalPos;
	}
};

class ProgressBar {
public:
	int pathLen; //終點位置

	ProgressBar(int pathLen) {
		this->pathLen = -pathLen;
	}
	void draw(Player *p) {
		float x = 0, y = 0; //進度條起點
		float width = 8, height = 0.2;
		
		//如果到達終點
		if (p->pos[2] <= pathLen) p->status = END;
		
		glDisable(GL_LIGHTING);
		{
			glPushMatrix();
			{
				//藍條
				glTranslatef(p->pos[0]-4, p->pos[1]+7.8, p->pos[2]);
				glColor3f(0, 1, 1);
				glBegin(GL_QUADS);
				glVertex2f(x, y);
				glVertex2f(x + (p->pos[2])/ pathLen * width, y);
				glVertex2f(x + (p->pos[2]) / pathLen * width, y + height);
				glVertex2f(x, y + height);
				glEnd();

				//白條
				glColor3f(1, 1, 1);
				glBegin(GL_QUADS);
				glVertex2f(x, y);
				glVertex2f(x + width, y);
				glVertex2f(x + width, y + height);
				glVertex2f(x, y + height);
				glEnd();
			}
			glPopMatrix();
			
		}
		glEnable(GL_LIGHTING);

		
	}
};

class Timer {
public:
	int nowTime;
	int time;
	Timer(int t) {
		this->time = t;
		this->nowTime = t;
	}

	void drawTimer(Player *p) {
		glDisable(GL_LIGHTING);
		{
			glPushMatrix();
			{
				glTranslatef(p->pos[0], p->pos[1], p->pos[2]);
				glColor3f(1, 1, 1);
				char timer[15];
				sprintf(timer, "TIME: %3d", this->nowTime);
				drawstr(-4, 7.5, timer);
			}
			glPopMatrix();
		}
		glEnable(GL_LIGHTING);
	}
};

class Building{
public:
	GLfloat scl[3];
	GLfloat pos[3]={0,0,0};
	GLfloat mat_amb[4]={0.2,0.2,0.2,1};
	GLfloat mat_dif[4]={0.8,0.7,0.7,1};
	GLfloat mat_dif_w[4]={1,1,1,1};
	GLfloat mat_nul[4]={0,0,0,0};

	Building(GLfloat x,GLfloat y,GLfloat z){
		scl[0]=x;scl[1]=y;scl[2]=z;
	}
	~Building(){}

	void setPos(GLfloat x,GLfloat y,GLfloat z){
		pos[0]=x;pos[1]=y;pos[2]=z;
	}

	void drawBuilding(){
		glPushMatrix();
		{
			//glTranslatef(0,0,-70);
			glTranslatef(pos[0],pos[1],pos[2]);
			glScalef(scl[0],scl[1],scl[2]);
			//glMaterialfv(GL_FRONT,GL_AMBIENT,mat_amb);
			
			glutSolidCube(1);
			//glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_dif_w);
		}
		glPopMatrix();
	}
};

class CollisionBall {
public:
	GLfloat myPos[3];

	CollisionBall(GLfloat x, GLfloat y, GLfloat z) {
		myPos[0] = x;
		myPos[1] = y;
		myPos[2] = z;
	}
	~CollisionBall() {}

	GLfloat dist(GLfloat pos[3]) {
		return sqrt(
			pow(pos[0] - myPos[0], 2) +
			pow(pos[1] - myPos[1], 2) +
			pow(pos[2] - myPos[2], 2)
		);
	}
	bool isColision(GLfloat r, GLfloat pos[3]) {
		return dist(pos) < r;
	}
};

class Obstacles{
public:
	//draw a test object in scene
	Obstacles(){}
	~Obstacles(){}

	static void drawObstacle(GLfloat x,GLfloat y,GLfloat z, GLfloat r, Player *p, GLuint displayId){
		GLfloat mat_dif_yellow[4] = { 0.8,0.7,0,1 };
		GLfloat mat_dif_white[4] = { 1,1,1,1 };
		GLfloat mat_dif_red[4] = { 1,0,0,1 };
		
		CollisionBall clision(x, y, z);
		glPushMatrix();
		{
			//glMaterialfv(GL_FRONT,GL_DIFFUSE, mat_dif_yellow);
			if (clision.isColision(r, p->pos)) {
				glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif_red);
			}
			glTranslatef(x, y, z);
			glEnable(GL_TEXTURE_2D); glEnable(GL_BLEND);
			{
				glCallList(displayId);
			}
			glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif_white);
		}
		glPopMatrix();
	}
};

typedef enum { ELDER_R, ELDER_L, CAR, OTHER } TYPE;

struct Pos {
	float x, y, z;
	TYPE type;
	bool moveR = false;
	Pos(float a, float b, float c, TYPE type) {
		x = a, y = b, z = c;
		this->type = type;
	}
};

class RandomGenObStacles {
public:
	int minX = -6, maxX = 6;/* 指定X範圍 */
	int intialPosZ = -15; //障礙物的初始z座標
	int nowZ = -15;
	int genNum = 4; //每次生成幾個障礙物
	int posZ_Shift = 10; //每次生成完後Z位移的範圍
	int endIdx = 0; //畫到endIdx
	bool gen = true;
	vector<Pos> ObStaclesPos;

	void init() {
		endIdx = 0;
		nowZ = intialPosZ;
		ObStaclesPos.clear();
		gen = true;
	}

	RandomGenObStacles(int minX, int maxX,
				int genNum,
				int intialPosZ,
				int posZ_Shift)
	{
		this->minX = minX;
		this->maxX = maxX;
		this->genNum = genNum;
		this->intialPosZ = intialPosZ;
		this->nowZ = intialPosZ;
		this->posZ_Shift = posZ_Shift;
	}

	
	void genObstaclePos() {
		nowZ -= (rand() % posZ_Shift) + 6;

		set<int> X;
		int num = rand() % genNum;
		int older = rand() % 4; //控制老奶奶出現的機率
		int car = rand() % 10; //控制逆向車出現的機率

		if (num == 1 && older==0) { //老奶奶過馬路
			Pos pos((float)maxX, 1, (float)nowZ, ELDER_R);
			pos.moveR = true;
			ObStaclesPos.push_back(pos);
		}
		else {
			for (int i = 0; i < num; ++i) {
				/* 產生亂數 */
				int x = rand() % (maxX - minX + 1) + minX;
				x = (int)x / 2 * 2;
				while (X.find(x) != X.end()) {
					x = rand() % (maxX - minX + 1) + minX;
					x = (int)x / 2 * 2; //出現的間隔
				}
				//printf("x=%d\n", x);
				X.insert(x);
				if (i == 0 && car==0) { //逆向車
					Pos pos((float)x, 1, (float)nowZ, CAR);
					ObStaclesPos.push_back(pos);
				}
				else {
					Pos pos((float)x, 1, (float)nowZ, OTHER);
					ObStaclesPos.push_back(pos);
				}
				
			}
		}
		
	}

	void drawObstacle(Player *p, int pathLen) {
		if(gen) genObstaclePos();
		/*printf("size = %d\n", ObStaclesPos.size());*/
		for (int i = ObStaclesPos.size()-1; i >= 0; --i) {
		//for (int i = ObStaclesPos.size()-1; i >= endIdx; --i) {
			
			if (ObStaclesPos[i].type == ELDER_R && i-endIdx <= 10) { //老奶奶左右移動
				if (ObStaclesPos[i].x >= maxX) ObStaclesPos[i].moveR = false;
				if (ObStaclesPos[i].x <= minX) ObStaclesPos[i].moveR = true;

				if (ObStaclesPos[i].moveR) {
					ObStaclesPos[i].x += 0.01;
				}
				else {
					ObStaclesPos[i].x -= 0.01;
				}
			}

			if (ObStaclesPos[i].type == CAR && i - endIdx <= 50) { //逆向車
				ObStaclesPos[i].z += 0.05;
			}

			Obstacles::drawObstacle(ObStaclesPos[i].x, 1, ObStaclesPos[i].z, 1.4, p, 10);
			//if (ObStaclesPos[i].z - p->pos[2] > 5) endIdx = i;
			//printf("endIdx=%d\n", endIdx);
			if (ObStaclesPos[i].z < pathLen) gen = false;
		}
	}
};