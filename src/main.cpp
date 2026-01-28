#include <graphics.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

struct Point {
    int x, y;
};

struct Seed {
    double x, y;
    double angle;
    double velocityY;
    bool active;
};

class AnimatedTreeDrawer {
   private:
    int screenWidth, screenHeight;
    int groundLevel;
    int seedX, seedY;
    std::vector<Point> grassPositions;

    // Animation state variables
    double treeGrowthScale;
    double flowerScale;
    bool showFlowers;
    std::vector<Seed> fallingSeeds;
    int animationPhase;  // 0: seed germination, 1: tree growth, 2: flowering, 3: seed fall, 4: reset
    int phaseTimer;
    bool grassInitialized;

    double zoomScale;
    int cameraOffsetX, cameraOffsetY;

    // Colors
    const int BROWN = COLOR(139, 69, 19);
    const int DARK_BROWN = COLOR(101, 67, 33);
    const int LEAF_GREEN = COLOR(34, 139, 34);
    const int LIGHT_GREEN = COLOR(50, 205, 50);
    const int SKY_BLUE = COLOR(135, 206, 235);
    const int SOIL_BROWN = COLOR(90, 50, 20);

    // Draw a seed with rotation
    void drawSeed(int x, int y, double angle, double scale = 1.0) {
        // Save current settings
        int oldColor = getcolor();

        // Seed is an oval shape
        setcolor(COLOR(160, 82, 45));
        setfillstyle(SOLID_FILL, COLOR(160, 82, 45));

        // Calculate rotated points for seed outline
        int size = static_cast<int>(8 * scale);
        std::vector<Point> seedPoints;

        for (int i = 0; i < 8; i++) {
            double t = i * 3.14159 / 4;
            int localX = static_cast<int>(size * cos(t));
            int localY = static_cast<int>(size * 0.6 * sin(t));

            // Apply rotation
            int rotatedX = static_cast<int>(localX * cos(angle) - localY * sin(angle));
            int rotatedY = static_cast<int>(localX * sin(angle) + localY * cos(angle));

            seedPoints.push_back({x + rotatedX, y + rotatedY});
        }

        // Draw seed as filled polygon
        fillellipse(x, y, static_cast<int>(size), static_cast<int>(size * 0.6));

        // Add a small sprout line if germinating
        if (scale > 1.0) {
            setcolor(LIGHT_GREEN);
            int sproutLength = static_cast<int>((scale - 1.0) * 20);
            line(x, y, x, y - sproutLength);
        }

        setcolor(oldColor);
    }

    // Draw soil layers
    void drawSoil() {
        // Ground surface
        setcolor(DARK_BROWN);
        setfillstyle(SOLID_FILL, DARK_BROWN);
        bar(0, groundLevel, screenWidth, groundLevel + 50);

        // Underground soil (darker)
        setcolor(SOIL_BROWN);
        setfillstyle(SOLID_FILL, SOIL_BROWN);
        bar(0, groundLevel + 50, screenWidth, screenHeight);

        // Generate grass positions only once
        if (!grassInitialized) {
            grassPositions.clear();
            for (int i = 0; i < screenWidth; i += 10) {
                Point grass;
                grass.x = i + (rand() % 10 - 5);
                grass.y = groundLevel - (rand() % 15 + 5);
                grassPositions.push_back(grass);
            }
            grassInitialized = true;
        }

        // Draw grass using stored positions
        setcolor(GREEN);
        for (const auto& grass : grassPositions) {
            line(grass.x, groundLevel, grass.x, grass.y);
        }
    }

    // Draw a branch recursively with scaling
    void drawBranch(int x1, int y1, double length, double angle, int depth, double scale, double growthProgress = 1.0) {
        if (depth <= 0 || scale <= 0.1) return;

        // Progressive branch growth - branches appear based on depth and progress
        double branchProgress = std::min(1.0, std::max(0.0, (growthProgress * 10) - (8 - depth)));
        if (branchProgress <= 0) return;

        double scaledLength = length * scale * branchProgress;

        int x2 = x1 + static_cast<int>(scaledLength * cos(angle));
        int y2 = y1 - static_cast<int>(scaledLength * sin(angle));

        if (depth > 4) {
            setcolor(BROWN);
            setlinestyle(SOLID_LINE, 0, static_cast<int>(depth * scale) + 1);
        } else {
            setcolor(LEAF_GREEN);
            setlinestyle(SOLID_LINE, 0, std::max(1, static_cast<int>(depth * scale)));
        }

        line(x1, y1, x2, y2);

        // Draw leaves throughout the tree, not just at top
        if (depth <= 5 && scale > 0.5 && branchProgress > 0.8) {
            setcolor(LIGHT_GREEN);
            setfillstyle(SOLID_FILL, LIGHT_GREEN);

            int numLeaves = (depth <= 3) ? 3 : 2;
            for (int i = 0; i < numLeaves; i++) {
                int leafX = x2 + (rand() % 10 - 5);
                int leafY = y2 + (rand() % 10 - 5);
                int leafSize = static_cast<int>(4 * scale);
                fillellipse(leafX, leafY, leafSize, leafSize);
            }
        }

        // Draw flowers - smaller and pink
        if (depth <= 2 && showFlowers && scale > 0.8 && branchProgress > 0.9) {
            drawFlower(x2, y2, flowerScale);
        }

        double newLength = length * 0.7;
        drawBranch(x2, y2, newLength, angle - 0.3, depth - 1, scale, growthProgress);
        drawBranch(x2, y2, newLength, angle + 0.3, depth - 1, scale, growthProgress);
        drawBranch(x2, y2, newLength * 0.8, angle, depth - 1, scale, growthProgress);
    }

    // Draw a flower with scaling
    void drawFlower(int x, int y, double scale) {
        if (scale <= 0) return;

        int petalSize = static_cast<int>(4 * scale);  // Changed from 5 to 4

        // Pink color instead of magenta
        setcolor(COLOR(255, 192, 203));
        setfillstyle(SOLID_FILL, COLOR(255, 192, 203));

        for (int i = 0; i < 5; i++) {
            double angle = i * 2 * 3.14159 / 5;
            int petalX = x + static_cast<int>(petalSize * cos(angle));
            int petalY = y + static_cast<int>(petalSize * sin(angle));
            fillellipse(petalX, petalY, petalSize, petalSize);
        }

        setcolor(YELLOW);
        setfillstyle(SOLID_FILL, YELLOW);
        fillellipse(x, y, petalSize - 1, petalSize - 1);
    }

    // Draw the sun
    void drawSun() {
        setcolor(YELLOW);
        setfillstyle(SOLID_FILL, YELLOW);
        fillellipse(screenWidth - 100, 100, 30, 30);

        // Draw sun rays
        for (int i = 0; i < 12; i++) {
            double angle = i * 30 * 3.14159 / 180.0;
            int x1 = screenWidth - 100 + static_cast<int>(35 * cos(angle));
            int y1 = 100 + static_cast<int>(35 * sin(angle));
            int x2 = screenWidth - 100 + static_cast<int>(50 * cos(angle));
            int y2 = 100 + static_cast<int>(50 * sin(angle));
            line(x1, y1, x2, y2);
        }
    }

    // Draw clouds
    void drawClouds() {
        setcolor(WHITE);
        setfillstyle(SOLID_FILL, WHITE);

        for (int cloud = 0; cloud < 3; cloud++) {
            int cloudX = 100 + cloud * 200;
            int cloudY = 80 + (cloud * 17) % 50;

            for (int i = 0; i < 5; i++) {
                int circleX = cloudX + i * 25;
                int circleY = cloudY + ((i * 13) % 20 - 10);
                int radius = 20 + (i * 7) % 10;
                fillellipse(circleX, circleY, radius, radius);
            }
        }
    }

    // Update falling seeds with physics
    void updateFallingSeeds() {
        for (auto& seed : fallingSeeds) {
            if (!seed.active) continue;

            // Apply gravity
            seed.velocityY += 0.5;
            seed.y += seed.velocityY;

            // Rotation during fall
            seed.angle += 0.1;

            // Check if seed hit the ground
            if (seed.y >= groundLevel + 80) {
                seed.active = false;
                // This seed will germinate in next cycle
                seedX = static_cast<int>(seed.x);
                seedY = groundLevel + 80;
            }
        }
    }

    // Display phase information
    void displayPhaseInfo() {
        setcolor(WHITE);
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);

        char title[100];
        switch (animationPhase) {
            case 0:
                sprintf(title, "Phase 1: Seed Germination");
                break;
            case 1:
                sprintf(title, "Phase 2: Seedling (Leaves)");
                break;
            case 2:
                sprintf(title, "Phase 3: Tree Growth");
                break;
            case 3:
                sprintf(title, "Phase 4: Flowering");
                break;
            case 4:
                sprintf(title, "Phase 5: Seed Dispersal");
                break;
            case 5:
                sprintf(title, "Phase 6: Cycle Reset");
                break;
        }
        outtextxy(10, 10, title);

        settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
        char msg[] = "Press ESC to exit, SPACE to restart";
        outtextxy(10, screenHeight - 20, msg);
    }

    void drawSeedlingLeaves(int x, int y, double progress) {
        // progress goes from 0 to 1
        int stemHeight = static_cast<int>(40 * progress);  // Increased from 30

        // Draw stem (this becomes the trunk)
        setcolor(LEAF_GREEN);
        setlinestyle(SOLID_LINE, 0, std::max(2, static_cast<int>(progress * 4)));
        line(x, y, x, y - stemHeight);

        // Leaves appear and grow
        if (progress > 0.2) {  // Changed from 0.3 to appear earlier
            double leafProgress = (progress - 0.2) / 0.8;
            int leafSize = static_cast<int>(20 * leafProgress);    // Increased from 15
            int leafYOffset = static_cast<int>(stemHeight * 0.5);  // Position leaves partway up stem

            setcolor(LIGHT_GREEN);
            setfillstyle(SOLID_FILL, LIGHT_GREEN);

            // Left leaf - angled outward
            fillellipse(x - leafSize, y - leafYOffset, leafSize, static_cast<int>(leafSize * 0.6));

            // Right leaf - angled outward
            fillellipse(x + leafSize, y - leafYOffset, leafSize, static_cast<int>(leafSize * 0.6));
        }
    }

   public:
    AnimatedTreeDrawer()
        : screenWidth(800),
          screenHeight(600),
          groundLevel(480),  // Changed from 450 to 480
          seedX(400),
          seedY(560),  // Changed from 530 to 560
          treeGrowthScale(0.0),
          flowerScale(0.0),
          showFlowers(false),
          animationPhase(0),
          phaseTimer(0),
          grassInitialized(false),
          zoomScale(1.0),
          cameraOffsetX(0),
          cameraOffsetY(0) {}

    void initialize() {
        initwindow(screenWidth, screenHeight, "Animated Tree Life Cycle");
        setbkcolor(SKY_BLUE);
        cleardevice();
        setactivepage(0);
        setvisualpage(0);
    }

    void resetAnimation() {
        treeGrowthScale = 0.0;
        flowerScale = 0.0;
        showFlowers = false;
        animationPhase = 0;
        phaseTimer = 0;
        fallingSeeds.clear();
        seedX = screenWidth / 2;
        seedY = 560;  // Match new seedY value
        zoomScale = 1.0;
        cameraOffsetX = 0;
        cameraOffsetY = 0;
    }

    void update() {
        phaseTimer++;

        switch (animationPhase) {
            case 0:  // Seed germination (0-40 frames)
                if (phaseTimer < 40) {
                    treeGrowthScale = 0.0;
                } else {
                    animationPhase = 1;
                    phaseTimer = 0;
                }
                break;

            case 1: {  // Leaf phase (0-60 frames) - grows smoothly
                if (phaseTimer < 60) {
                    // Don't set to 0, let it transition smoothly
                    treeGrowthScale = std::min(0.15, phaseTimer / 400.0);  // Very gradual start
                } else {
                    animationPhase = 2;
                    phaseTimer = 0;
                }
                break;
            }

            case 2: {  // Tree growth (0-100 frames)
                if (phaseTimer < 100) {
                    treeGrowthScale = 0.15 + (phaseTimer / 100.0) * 0.85;  // Continue from 0.15
                } else {
                    animationPhase = 3;
                    phaseTimer = 0;
                    showFlowers = true;
                }
                break;
            }

            case 3: {                   // Flowering (0-25 frames) - VERY FAST
                if (phaseTimer < 25) {  // Changed from 50 to 25
                    flowerScale = phaseTimer / 25.0;
                } else {
                    animationPhase = 4;
                    phaseTimer = 0;

                    Seed newSeed;
                    newSeed.x = seedX + 50;
                    newSeed.y = groundLevel - 150;
                    newSeed.angle = 0;
                    newSeed.velocityY = 0;
                    newSeed.active = true;
                    fallingSeeds.clear();
                    fallingSeeds.push_back(newSeed);
                }
                break;
            }

            case 4: {  // Seed dispersal with extreme zoom (0-150 frames)
                updateFallingSeeds();

                // Get the falling seed position
                if (!fallingSeeds.empty() && fallingSeeds[0].active) {
                    Seed& seed = fallingSeeds[0];

                    // Zoom dramatically into seed as it falls
                    if (phaseTimer < 100) {
                        // Extreme zoom from 1x to 15x
                        zoomScale = 1.0 + (phaseTimer / 100.0) * 14.0;

                        // Center camera on the falling seed
                        cameraOffsetX = screenWidth / 2 - static_cast<int>(seed.x);
                        cameraOffsetY = screenHeight / 2 - static_cast<int>(seed.y) + 100;
                    }
                }

                bool allFallen = true;
                for (const auto& seed : fallingSeeds) {
                    if (seed.active) allFallen = false;
                }

                if (allFallen && phaseTimer > 30) {
                    animationPhase = 5;
                    phaseTimer = 0;
                }
                break;
            }

            case 5: {  // Zoom out and reset (0-40 frames)
                if (phaseTimer < 40) {
                    // Fade tree
                    treeGrowthScale = 1.0 - (phaseTimer / 40.0);
                    flowerScale = 1.0 - (phaseTimer / 40.0);

                    // Zoom back out from 15x to 1x
                    zoomScale = 15.0 - (phaseTimer / 40.0) * 14.0;

                    // Return camera to center
                    cameraOffsetX = cameraOffsetX * (1.0 - phaseTimer / 40.0);
                    cameraOffsetY = cameraOffsetY * (1.0 - phaseTimer / 40.0);
                } else {
                    resetAnimation();
                }
                break;
            }
        }
    }

    void render() {
        setactivepage(1 - getactivepage());
        cleardevice();

        drawSun();
        drawClouds();

        int drawOffsetX = cameraOffsetX;
        int drawOffsetY = cameraOffsetY;

        int transformedGroundLevel = groundLevel + drawOffsetY;
        int originalGroundLevel = groundLevel;
        groundLevel = transformedGroundLevel;
        drawSoil();
        groundLevel = originalGroundLevel;

        // Draw seed underground
        if (animationPhase <= 1) {
            double seedScale = 1.0 + (animationPhase == 0 ? phaseTimer / 20.0 : 2.0);
            int seedDrawX = static_cast<int>((seedX + drawOffsetX) * zoomScale - (zoomScale - 1.0) * screenWidth / 2);
            int seedDrawY = static_cast<int>((seedY + drawOffsetY) * zoomScale - (zoomScale - 1.0) * screenHeight / 2);
            drawSeed(seedDrawX, seedDrawY, 0, seedScale * zoomScale);
        }

        // MORPHING STAGES - all drawn together for smooth transition

        // Stage 1: Seedling stem and leaves (phases 1)
        if (animationPhase == 1) {
            double leafProgress = phaseTimer / 60.0;
            int leafDrawX = static_cast<int>((seedX + drawOffsetX) * zoomScale - (zoomScale - 1.0) * screenWidth / 2);
            int leafDrawY = static_cast<int>((groundLevel + drawOffsetY) * zoomScale - (zoomScale - 1.0) * screenHeight / 2);
            drawSeedlingLeaves(leafDrawX, leafDrawY, leafProgress);
        }

        // Stage 2-3: Tree (overlaps with leaves at start of phase 2 for smooth transition)
        if (animationPhase >= 2 || (animationPhase == 1 && phaseTimer > 50)) {
            // Calculate blend factor for smooth transition
            double blendFactor = 1.0;
            if (animationPhase == 1) {
                blendFactor = (phaseTimer - 50) / 10.0;  // Fade in tree during last 10 frames of leaf stage
            }

            int startX = static_cast<int>((seedX + drawOffsetX) * zoomScale - (zoomScale - 1.0) * screenWidth / 2);
            int startY = static_cast<int>((groundLevel + drawOffsetY) * zoomScale - (zoomScale - 1.0) * screenHeight / 2);
            int trunkLength = static_cast<int>(150 * zoomScale);
            double initialAngle = 3.14159 / 2;

            // Use actual treeGrowthScale which transitions smoothly
            if (treeGrowthScale > 0.01) {
                drawBranch(startX, startY, trunkLength, initialAngle, 8, treeGrowthScale * blendFactor, treeGrowthScale * blendFactor);
            }
        }

        // Draw falling seed - zoomed and centered
        for (const auto& seed : fallingSeeds) {
            if (seed.active) {
                int seedDrawX = static_cast<int>((seed.x + drawOffsetX) * zoomScale - (zoomScale - 1.0) * screenWidth / 2);
                int seedDrawY = static_cast<int>((seed.y + drawOffsetY) * zoomScale - (zoomScale - 1.0) * screenHeight / 2);

                drawSeed(seedDrawX, seedDrawY, seed.angle, zoomScale * 1.5);
            }
        }

        displayPhaseInfo();
        setvisualpage(getactivepage());
    }

    void run() {
        initialize();

        while (true) {
            // Check for keyboard input
            if (kbhit()) {
                char key = getch();
                if (key == 27) break;              // ESC to exit
                if (key == ' ') resetAnimation();  // SPACE to restart
            }

            // Update animation state
            update();

            // Render frame
            render();

            // Control frame rate (~30 FPS)
            // Sleep(33);  // Windows Sleep uses milliseconds
            delay(33);
        }

        closegraph();
    }
};

int main() {
    AnimatedTreeDrawer drawer;
    drawer.run();

    return 0;
}