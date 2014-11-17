#pragma once

#include <cmath>

class Camera
{
    public:
        Camera();
        virtual ~Camera();

        void init(int windowWidth, int windowHeight);
        void applyMatrix();
        void update(int x, int y);
	void translateCamera();
	void moveView(int x, int y);
        void mousePressed(int x, int y);
        void drawAim();

    private:
        int width;
        int height;
	
        float rotX;
        float rotY;

        float aim_dist;

        // Previous mouse position
        int lastX;
        int lastY;

	float posX;
	float posY;
	
	float speed;
	float speedView;
	float angleHorizontal;
};
