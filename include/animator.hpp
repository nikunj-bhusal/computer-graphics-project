#ifndef ANIMATOR
#define ANIMATOR

#include <graphics.h>

#include <chrono>
#include <vector>

struct Seed {
    double x, y;
    double angle;
    double velocityX, velocityY;
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
    int animationPhase;
    int frameCount;
    double zoomScale;
    double cameraOffsetX, cameraOffsetY;
    Seed fallingSeed;
    bool hasFallingSeed;

    const double PI = 3.141592654;

    const int BROWN = COLOR(139, 69, 19);
    const int DARK_BROWN = COLOR(101, 67, 33);

    const int SEED_COLOR = COLOR(160, 82, 45);
    const int SEED_SLIT_COLOR = COLOR(100, 50, 20);

    const int LEAF_GREEN = COLOR(34, 139, 34);
    const int LIGHT_GREEN = COLOR(50, 205, 50);

    const int SKY_BLUE = COLOR(135, 206, 235);
    const int SOIL_BROWN = COLOR(110, 70, 40);

    const int PETAL_COLOR = COLOR(255, 245, 238);
    const int CENTER_DARK = COLOR(139, 0, 0);

    const int ROOT_DEEP = COLOR(140, 100, 60);
    const int ROOT_SHALLOW = COLOR(190, 150, 100);

    const int PHASE_INFO_TEXT = COLOR(20, 20, 60);
    const int PAUSE_MENU_BORDER = COLOR(0, 109, 252);

    void drawSeed(int x, int y, double angle, double scale = 1.0);
    void drawSoil();
    void drawBranch(int x1, int y1, double length, double angle, int depth, double scale, double growthProgress = 1.0);
    void drawFlower(int x, int y, double scale);
    void drawSun();
    void drawClouds();
    void displayPhaseInfo();
    void drawPauseMenu();
    void drawLeafShape(int x, int y, double angle, double scale, int color);
    void drawRoot(int x1, int y1, double length, double angle, int depth, double growthProgress, int surfaceLimitY);
    void updateFallingSeed();

   public:
    AnimatedTreeDrawer();
    void initialize();
    void resetAnimation();
    void update();
    void render();
    void run();
};

#endif