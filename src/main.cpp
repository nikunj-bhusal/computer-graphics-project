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
    void drawBranch(int x1, int y1, double length, double angle, int depth, double scale) {
        if (depth <= 0 || scale <= 0.1) return;

        double scaledLength = length * scale;

        // Calculate end point of the branch
        int x2 = x1 + static_cast<int>(scaledLength * cos(angle));
        int y2 = y1 - static_cast<int>(scaledLength * sin(angle));

        // Set color and thickness based on depth
        if (depth > 4) {
            setcolor(BROWN);
            setlinestyle(SOLID_LINE, 0, static_cast<int>(depth * scale) + 1);
        } else {
            setcolor(LEAF_GREEN);
            setlinestyle(SOLID_LINE, 0, std::max(1, static_cast<int>(depth * scale)));
        }

        // Draw the branch
        line(x1, y1, x2, y2);

        // Draw leaves on smaller branches if tree is grown enough
        if (depth <= 3 && scale > 0.5) {
            setcolor(LIGHT_GREEN);
            setfillstyle(SOLID_FILL, LIGHT_GREEN);

            for (int i = 0; i < 3; i++) {
                int leafX = x2 + (rand() % 10 - 5);
                int leafY = y2 + (rand() % 10 - 5);
                int leafSize = static_cast<int>(3 * scale);
                fillellipse(leafX, leafY, leafSize, leafSize);
            }
        }

        // Draw flowers if in flowering phase
        if (depth <= 2 && showFlowers && scale > 0.8) {
            drawFlower(x2, y2, flowerScale);
        }

        // Recursively draw sub-branches
        double newLength = length * 0.7;
        drawBranch(x2, y2, newLength, angle - 0.3, depth - 1, scale);
        drawBranch(x2, y2, newLength, angle + 0.3, depth - 1, scale);
        drawBranch(x2, y2, newLength * 0.8, angle, depth - 1, scale);
    }

    // Draw a flower with scaling
    void drawFlower(int x, int y, double scale) {
        if (scale <= 0) return;

        int petalSize = static_cast<int>(5 * scale);

        // Draw petals (pink/magenta)
        setcolor(MAGENTA);
        setfillstyle(SOLID_FILL, MAGENTA);

        for (int i = 0; i < 5; i++) {
            double angle = i * 2 * 3.14159 / 5;
            int petalX = x + static_cast<int>(petalSize * cos(angle));
            int petalY = y + static_cast<int>(petalSize * sin(angle));
            fillellipse(petalX, petalY, petalSize, petalSize);
        }

        // Draw center (yellow)
        setcolor(YELLOW);
        setfillstyle(SOLID_FILL, YELLOW);
        fillellipse(x, y, petalSize, petalSize);
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

   public:
    AnimatedTreeDrawer()
        : screenWidth(800),
          screenHeight(600),
          groundLevel(450),
          seedX(400),
          seedY(530),
          treeGrowthScale(0.0),
          flowerScale(0.0),
          showFlowers(false),
          animationPhase(0),
          phaseTimer(0),
          grassInitialized(false) {}

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
        seedY = groundLevel + 80;
    }

    void update() {
        phaseTimer++;

        switch (animationPhase) {
            case 0:  // Seed germination (0-60 frames)
                if (phaseTimer < 60) {
                    // Just show growing seed with sprout
                    treeGrowthScale = 0.0;  // No tree yet
                } else {
                    animationPhase = 1;
                    phaseTimer = 0;
                }
                break;

            case 1: {  // Leaf phase (0-80 frames) - two small leaves
                if (phaseTimer < 80) {
                    treeGrowthScale = 0.15;  // Keep small, just show initial leaves
                } else {
                    animationPhase = 2;
                    phaseTimer = 0;
                }
                break;
            }

            case 2: {  // Tree growth (0-150 frames)
                if (phaseTimer < 150) {
                    // Scaling transformation - tree grows from small to full size
                    treeGrowthScale = 0.15 + (phaseTimer / 150.0) * 0.85;  // 0.15 to 1.0
                } else {
                    animationPhase = 3;
                    phaseTimer = 0;
                    showFlowers = true;
                }
                break;
            }

            case 3: {  // Flowering (0-80 frames)
                if (phaseTimer < 80) {
                    flowerScale = phaseTimer / 80.0;
                } else {
                    animationPhase = 4;
                    phaseTimer = 0;

                    // Create ONE falling seed from a flower position
                    Seed newSeed;
                    newSeed.x = seedX + 50;
                    newSeed.y = groundLevel - 150;
                    newSeed.angle = 0;
                    newSeed.velocityY = 0;
                    newSeed.active = true;
                    fallingSeeds.clear();  // Clear any old seeds
                    fallingSeeds.push_back(newSeed);
                }
                break;
            }

            case 4: {  // Seed dispersal
                updateFallingSeeds();

                bool allFallen = true;
                for (const auto& seed : fallingSeeds) {
                    if (seed.active) allFallen = false;
                }

                if (allFallen && phaseTimer > 50) {
                    animationPhase = 5;
                    phaseTimer = 0;
                }
                break;
            }

            case 5: {  // Fade out and reset
                if (phaseTimer < 50) {
                    treeGrowthScale = 1.0 - (phaseTimer / 50.0);
                    flowerScale = 1.0 - (phaseTimer / 50.0);
                } else {
                    resetAnimation();
                }
                break;
            }
        }
    }

    void render() {
        setactivepage(1 - getactivepage());
        // Clear screen
        cleardevice();

        // Draw background elements
        drawSun();
        drawClouds();
        drawSoil();

        // Draw seed underground (always visible if not sprouted)
        if (animationPhase <= 1) {
            double seedScale = 1.0 + (animationPhase == 0 ? phaseTimer / 50.0 : 2.0);
            drawSeed(seedX, seedY, 0, seedScale);
        }

        // Draw tree with scaling transformation
        if (treeGrowthScale > 0.01) {
            int startX = seedX;
            int startY = seedY - 20;
            int trunkLength = 150;
            double initialAngle = 3.14159 / 2;

            drawBranch(startX, startY, trunkLength, initialAngle, 8, treeGrowthScale);

            // Draw tree base
            if (treeGrowthScale > 0.2) {
                setcolor(DARK_BROWN);
                setfillstyle(SOLID_FILL, DARK_BROWN);
                int baseWidth = static_cast<int>(30 * treeGrowthScale);
                bar(startX - baseWidth, startY - 10, startX + baseWidth, startY + 20);
            }
        }

        // Draw falling seeds with rotation and translation
        for (const auto& seed : fallingSeeds) {
            if (seed.active) {
                drawSeed(static_cast<int>(seed.x), static_cast<int>(seed.y), seed.angle, 1.0);
            }
        }

        // Display information
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