#ifndef ANIMATOR
#define ANIMATOR

#include <graphics.h>

#include <chrono>
#include <vector>

struct Point {
    int x, y;
};

struct Seed {
    double x, y;
    double angle;
    double velocityY;
    double velocityX;
    bool active;
};

class AnimatedTreeDrawer {
   private:
    bool isPaused;
    int screenWidth, screenHeight;
    int groundLevel;
    int seedX, seedY;
    std::chrono::steady_clock::time_point lastTime;
    double sunAngle;

    double treeGrowthScale;
    double flowerScale;
    bool showFlowers;
    int flowerPosX, flowerPosY;
    std::vector<Point> flowerPositions;
    std::vector<Seed> fallingSeeds;
    int animationPhase;
    int phaseTimer;
    int rightmostBranchX, rightmostBranchY;
    double zoomScale;
    double cameraOffsetX, cameraOffsetY;

    const double PI = 3.141592654;
    const int BROWN = COLOR(139, 69, 19);
    const int DARK_BROWN = COLOR(101, 67, 33);
    const int LEAF_GREEN = COLOR(34, 139, 34);
    const int LIGHT_GREEN = COLOR(50, 205, 50);
    const int SKY_BLUE = COLOR(135, 206, 235);
    const int SOIL_BROWN = COLOR(90, 50, 20);

    void drawSeed(int x, int y, double angle, double scale = 1.0);
    void drawSoil();
    void drawBranch(int x1, int y1, double length, double angle, int depth, double scale, double growthProgress = 1.0);
    void drawFlower(int x, int y, double scale);
    void drawSun();
    void drawClouds();
    void displayPhaseInfo();
    void drawPauseMenu();
    void drawLeafShape(int x, int y, double angle, double scale, int color);
    void drawSeedlingLeaves(int x, int y, double progress, int stemColor = -1);
    void drawRoot(int x1, int y1, double length, double angle, int depth, double growthProgress, int surfaceLimitY);
    void updateFallingSeeds();

   public:
    AnimatedTreeDrawer();
    void initialize();
    void resetAnimation();
    void update();
    void render();
    void run();
};

#endif