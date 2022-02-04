#pragma once

class Sprite
{
public:
	double x = 0;
	double y = 0;

	int dirY = 1; //"Down"
	int dirX = 1; //"Right"

	double rotation = 0.0f;
	double scale = 1.0f;

	int width = 0;
	int height = 0;

	int spriteSheetId;
	int frameIndex;

	double rotationSpeed = 0;
	double speed = 0;
};