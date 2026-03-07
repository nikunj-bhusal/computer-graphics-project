#include "../include/animator.hpp"

#include <graphics.h>

#include <chrono>
#include <cmath>
#include <vector>

void AnimatedTreeDrawer::drawSeed(int x, int y, double angle, double scale) {
    int oldColor = getcolor();

    setcolor(COLOR(160, 82, 45));
    setfillstyle(SOLID_FILL, COLOR(160, 82, 45));

    int size = static_cast<int>(8 * scale);

    const int numPoints = 12;
    int points[numPoints * 2];

    for (int i = 0; i < numPoints; i++) {
        double t = i * 2 * PI / numPoints;
        double localX = size * cos(t);
        double localY = size * 0.5 * sin(t);

        int rotatedX = static_cast<int>(localX * cos(angle) - localY * sin(angle));
        int rotatedY = static_cast<int>(localX * sin(angle) + localY * cos(angle));

        points[i * 2] = x + rotatedX;
        points[i * 2 + 1] = y + rotatedY;
    }

    fillpoly(numPoints, points);

    int lineLength = static_cast<int>(size * 0.8);
    int lineX1 = x + static_cast<int>(lineLength * cos(angle));
    int lineY1 = y + static_cast<int>(lineLength * sin(angle));
    int lineX2 = x - static_cast<int>(lineLength * cos(angle));
    int lineY2 = y - static_cast<int>(lineLength * sin(angle));

    setcolor(COLOR(100, 50, 20));
    setlinestyle(SOLID_LINE, 0, std::max(1, static_cast<int>(scale / 3)));
    line(lineX1, lineY1, lineX2, lineY2);

    setcolor(oldColor);
}

void AnimatedTreeDrawer::drawSoil() {
    int soilColor = COLOR(110, 70, 40);
    setfillstyle(SOLID_FILL, soilColor);
    setcolor(soilColor);
    bar(0, groundLevel, screenWidth, screenHeight);
}

void AnimatedTreeDrawer::drawBranch(int x1, int y1, double length, double angle, int depth, double scale, double growthProgress) {
    double currentVisualLength = length * scale;
    if (currentVisualLength < 2.0 || depth <= 0) return;

    if (depth >= 5) {
        setcolor(DARK_BROWN);
    } else {
        setcolor(BROWN);
    }

    double branchProgress = std::min(1.0, std::max(0.0, (growthProgress * 10) - (8 - depth)));
    if (branchProgress <= 0) return;

    double scaledLength = currentVisualLength * branchProgress;
    int x2 = x1 + static_cast<int>(scaledLength * cos(angle));
    int y2 = y1 - static_cast<int>(scaledLength * sin(angle));

    setlinestyle(SOLID_LINE, 0, std::max(1, static_cast<int>(depth * scale)));
    line(x1, y1, x2, y2);

    if (depth <= 5 && branchProgress > 0.6) {
        int leafSize = std::max(2, static_cast<int>(5 * scale));
        int numLeaves = 10;

        for (int i = 0; i < numLeaves; i++) {
            if (i % 3 == 0) {
                setcolor(LIGHT_GREEN);
                setfillstyle(SOLID_FILL, LIGHT_GREEN);
            } else if (i % 3 == 1) {
                setcolor(LEAF_GREEN);
                setfillstyle(SOLID_FILL, LEAF_GREEN);
            } else {
                setcolor(COLOR(34, 139, 34));
                setfillstyle(SOLID_FILL, COLOR(34, 139, 34));
            }

            double branchPos = (i % 2 == 0) ? 1.0 : 0.85;
            int bx = x1 + (x2 - x1) * branchPos;
            int by = y1 + (y2 - y1) * branchPos;

            double angle = i * (6.28 / numLeaves);
            int radius = (i % 3 == 0) ? 12 : 6;
            radius *= scale;

            int offsetX = cos(angle) * radius;
            int offsetY = (sin(angle) * radius) + (i % 4 == 0 ? -8 : 4);

            fillellipse(bx + offsetX, by + offsetY, leafSize, leafSize + 1);
        }
        setcolor(BROWN);
    }

    if (showFlowers && branchProgress > 0.9) {
        if (depth == 3 && flowerScale > 0.3) {
            double localScale = (flowerScale - 0.3) / 0.7;
            drawFlower(x2, y2, localScale);
        }
        if (depth == 2 && flowerScale > 0.6) {
            double localScale = (flowerScale - 0.6) / 0.4;
            drawFlower(x2, y2, localScale);
        }
        if (depth == 1 && flowerScale > 0.8) {
            double localScale = (flowerScale - 0.8) / 0.2;
            drawFlower(x2, y2, localScale);
        }
    }

    double newLength = length * 0.7;
    drawBranch(x2, y2, newLength, angle - 0.45, depth - 1, scale, growthProgress);
    drawBranch(x2, y2, newLength, angle + 0.45, depth - 1, scale, growthProgress);
    drawBranch(x2, y2, newLength * 0.8, angle, depth - 1, scale, growthProgress);
}

void AnimatedTreeDrawer::drawFlower(int x, int y, double scale) {
    if (scale <= 0) return;

    int oldColor = getcolor();
    int baseSize = std::max(2, static_cast<int>(3 * scale));

    int shadowPetalColor = COLOR(240, 220, 210);
    int frontPetalColor = COLOR(255, 245, 238);
    int centerDark = COLOR(139, 0, 0);
    int centerLight = YELLOW;

    setcolor(shadowPetalColor);
    setfillstyle(SOLID_FILL, shadowPetalColor);
    fillellipse(x, y - baseSize, baseSize + 1, baseSize + 2);
    fillellipse(x, y + baseSize, baseSize + 1, baseSize + 2);

    setcolor(frontPetalColor);
    setfillstyle(SOLID_FILL, frontPetalColor);
    fillellipse(x - baseSize, y, baseSize + 2, baseSize + 1);
    fillellipse(x + baseSize, y, baseSize + 2, baseSize + 1);

    setcolor(centerDark);
    setfillstyle(SOLID_FILL, centerDark);
    int centerRadius = std::max(1, (baseSize / 2) + 1);
    fillellipse(x, y, centerRadius, centerRadius);

    setcolor(centerLight);
    setfillstyle(SOLID_FILL, centerLight);
    int dotRadius = std::max(1, baseSize / 3);
    fillellipse(x, y, dotRadius, dotRadius);

    setcolor(oldColor);
}

void AnimatedTreeDrawer::drawSun() {
    int radius = 30;
    int skyHeight = 150;
    int sunX = static_cast<int>(screenWidth * sunAngle / PI);
    int sunY = static_cast<int>(skyHeight * sin(sunAngle)) + 50;

    setcolor(YELLOW);
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(sunX, sunY, radius, radius);

    for (int i = 0; i < 12; i++) {
        double angle = i * 30 * PI / 180.0;
        int x1 = sunX + static_cast<int>((radius + 5) * cos(angle));
        int y1 = sunY + static_cast<int>((radius + 5) * sin(angle));
        int x2 = sunX + static_cast<int>((radius + 20) * cos(angle));
        int y2 = sunY + static_cast<int>((radius + 20) * sin(angle));
        line(x1, y1, x2, y2);
    }
}

void AnimatedTreeDrawer::drawClouds() {
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

void AnimatedTreeDrawer::updateFallingSeeds() {
    for (auto& seed : fallingSeeds) {
        if (!seed.active) continue;

        if (seed.y < groundLevel) {
            seed.velocityY += 0.02;
            seed.y += seed.velocityY;
            seed.x += seed.velocityX;

            double distanceToGround = groundLevel - seed.y;
            seed.angle = (distanceToGround / 35.0) + (PI * 4);
        } else {
            seed.angle = 0;
            seed.velocityY = 0;
            seed.velocityX = 0;

            if (seed.y < groundLevel + 25) {
                seed.y += 0.5;
            } else {
                seed.y = groundLevel + 25;
            }
        }
    }
}

void AnimatedTreeDrawer::displayPhaseInfo() {
    setcolor(COLOR(20, 20, 60));
    settextstyle(SIMPLEX_FONT, HORIZ_DIR, 1);

    char title[100];
    switch (animationPhase) {
        case 0:
            sprintf(title, "Phase 1: Seed Germination");
            break;
        case 1:
            sprintf(title, "Phase 2: Seedling Growth");
            break;
        case 2:
            sprintf(title, "Phase 3: Tree Maturation");
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
    outtextxy(20, 20, title);
}

void AnimatedTreeDrawer::drawPauseMenu() {
    int boxW = 345, boxH = 135;
    int midX = screenWidth / 2;
    int midY = screenHeight / 2;
    int borderColor = COLOR(80, 50, 30);

    setcolor(borderColor);
    setlinestyle(SOLID_LINE, 0, 3);
    rectangle(midX - boxW / 2, midY - boxH / 2, midX + boxW / 2, midY + boxH / 2);

    setfillstyle(SOLID_FILL, WHITE);
    floodfill(midX, midY, borderColor);

    setbkcolor(WHITE);
    setcolor(BLACK);

    settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);
    outtextxy(midX - 55, midY - 55, (char*)"PAUSED");

    settextstyle(SIMPLEX_FONT, HORIZ_DIR, 1);
    outtextxy(midX - 160, midY - 10, (char*)"[Space] - Play / Pause");
    outtextxy(midX - 160, midY + 15, (char*)"[R]        - Reset Animation");
    outtextxy(midX - 160, midY + 40, (char*)"[Q]        - Quit Application");
}

void AnimatedTreeDrawer::drawLeafShape(int x, int y, double angle, double scale, int color) {
    int oldColor = getcolor();
    setcolor(color);
    setfillstyle(SOLID_FILL, color);

    int size = static_cast<int>(10 * scale);
    const int numPoints = 12;
    int points[numPoints * 2];

    for (int i = 0; i < numPoints; i++) {
        double t = i * 2 * PI / numPoints;

        double localX = size * cos(t) + size;
        double localY = size * 0.35 * sin(t);

        int rotatedX = static_cast<int>(localX * cos(angle) - localY * sin(angle));
        int rotatedY = static_cast<int>(localX * sin(angle) + localY * cos(angle));

        points[i * 2] = x + rotatedX;
        points[i * 2 + 1] = y + rotatedY;
    }
    fillpoly(numPoints, points);
    setcolor(oldColor);
}

void AnimatedTreeDrawer::drawSeedlingLeaves(int x, int y, double progress, int stemColor) {
    if (progress <= 0) return;

    int usedColor = (stemColor == -1) ? LIGHT_GREEN : stemColor;

    int stemHeight = static_cast<int>(60 * progress * zoomScale);

    setcolor(usedColor);
    setlinestyle(SOLID_LINE, 0, std::max(2, static_cast<int>(progress * 4 * zoomScale)));
    line(x, y, x, y - stemHeight);

    if (progress > 0.1) {
        int tipX = x;
        int tipY = y - stemHeight;
        double leafScale = progress * zoomScale;

        drawLeafShape(tipX, tipY, -3 * PI / 4.0, leafScale, usedColor);
        drawLeafShape(tipX, tipY, -1 * PI / 4.0, leafScale, usedColor);
    }
}

void AnimatedTreeDrawer::drawRoot(int x1, int y1, double length, double angle, int depth, double growthProgress, int surfaceLimitY) {
    if (depth <= 0 || growthProgress <= 0) return;

    int rootColor = (depth > 3) ? COLOR(140, 100, 60) : COLOR(190, 150, 100);
    setcolor(rootColor);

    int thickness = std::max(1, static_cast<int>(depth * depth * 0.35 * zoomScale * growthProgress));
    setlinestyle(SOLID_LINE, 0, thickness);

    double currentLength = length * growthProgress;

    int x2 = x1 + static_cast<int>(currentLength * cos(angle) * 1.3);
    int y2 = y1 + static_cast<int>(currentLength * sin(angle) * 0.5);

    int drawY1 = std::max(y1, surfaceLimitY);
    int drawY2 = std::max(y2, surfaceLimitY);

    if (drawY1 >= surfaceLimitY) {
        line(x1, drawY1, x2, drawY2);
    }

    if (depth > 1) {
        drawRoot(x2, y2, length * 0.65, angle + 0.3, depth - 1, growthProgress, surfaceLimitY);
        drawRoot(x2, y2, length * 0.65, angle - 0.3, depth - 1, growthProgress, surfaceLimitY);
        drawRoot(x2, y2, length * 0.8, angle + 1.3, depth - 1, growthProgress, surfaceLimitY);
        drawRoot(x2, y2, length * 0.8, angle - 1.3, depth - 1, growthProgress, surfaceLimitY);

        if (depth == 4) {
            drawRoot(x1, y1, length * 1.2, 0.2, depth - 2, growthProgress, surfaceLimitY);
            drawRoot(x1, y1, length * 1.2, PI - 0.2, depth - 2, growthProgress, surfaceLimitY);
        }
    }
}

AnimatedTreeDrawer::AnimatedTreeDrawer()
    : isPaused(true),
      screenWidth(800),
      screenHeight(600),
      groundLevel(480),
      seedX(400),
      seedY(groundLevel + 25),
      sunAngle(0.0),
      treeGrowthScale(0.0),
      flowerScale(0.0),
      showFlowers(false),
      animationPhase(0),
      phaseTimer(0),
      rightmostBranchX(0),
      rightmostBranchY(0),
      zoomScale(1.0),
      cameraOffsetX(0),
      cameraOffsetY(0) {}

void AnimatedTreeDrawer::initialize() {
    lastTime = std::chrono::steady_clock::now();
    initwindow(screenWidth, screenHeight, "Animated Tree Life Cycle");
    setbkcolor(SKY_BLUE);
    cleardevice();
    setactivepage(0);
    setvisualpage(0);
}

void AnimatedTreeDrawer::resetAnimation() {
    treeGrowthScale = 0.0;
    flowerScale = 0.0;
    showFlowers = false;
    animationPhase = 0;
    phaseTimer = 0;
    fallingSeeds.clear();
    seedX = screenWidth / 2;
    seedY = groundLevel + 25;
    zoomScale = 1.0;
    cameraOffsetX = 0;
    cameraOffsetY = 0;
    flowerPositions.clear();
    rightmostBranchX = 0;
    rightmostBranchY = 0;
    fallingSeeds.clear();
}

void AnimatedTreeDrawer::update() {
    phaseTimer += 1;

    switch (animationPhase) {
        case 0:  // Seed germination (0-40 frames)
            if (phaseTimer < 40) {
                treeGrowthScale = 0.0;
            } else {
                animationPhase = 1;
                phaseTimer = 0;
            }
            break;

        case 1: {  // Leaf phase
            if (phaseTimer < 60) {
                treeGrowthScale = std::min(0.15, phaseTimer / 400.0);
            } else {
                animationPhase = 2;
                phaseTimer = 0;
            }
            break;
        }

        case 2: {  // Tree growth
            if (phaseTimer < 100) {
                treeGrowthScale = 0.15 + (phaseTimer / 100.0) * 0.85;
            } else {
                treeGrowthScale = 1.0;
                animationPhase = 3;
                phaseTimer = 0;
                showFlowers = true;
            }
            break;
        }

        case 3: {  // Flowering
            if (phaseTimer < 100) {
                flowerScale = phaseTimer / 100.0;
            } else {
                animationPhase = 4;
                phaseTimer = 0;

                Seed newSeed;
                newSeed.x = seedX + 150;
                newSeed.y = seedY - 360;
                newSeed.angle = 60;
                newSeed.velocityY = 0;
                newSeed.velocityX = 0.8;
                newSeed.active = true;
                fallingSeeds.clear();
                fallingSeeds.push_back(newSeed);
            }
            break;
        }

        case 4: {  // Falling phase
            updateFallingSeeds();

            if (!fallingSeeds.empty()) {
                Seed& s = fallingSeeds[0];

                if (zoomScale < 8.0) zoomScale += 0.05;

                double startFocusX = screenWidth / 2.0;
                double startFocusY = screenHeight / 2.0;
                double targetFocusX = s.x;
                double targetFocusY = s.y;

                double panProgress = std::min(1.0, phaseTimer / 80.0);
                double ease = 1.0 - pow(1.0 - panProgress, 3);

                double currentFocusX = startFocusX + (targetFocusX - startFocusX) * ease;
                double currentFocusY = startFocusY + (targetFocusY - startFocusY) * ease;

                cameraOffsetX = (screenWidth / 2.0 / zoomScale) - currentFocusX;
                cameraOffsetY = (screenHeight / 2.0 / zoomScale) - currentFocusY;

                if (s.y >= groundLevel + 25) {
                    if (phaseTimer > 100) {
                        animationPhase = 5;
                        phaseTimer = 0;
                    }
                }
            }
            break;
        }

        case 5: {  // Zoom out phase
            if (phaseTimer < 150) {
                double progress = phaseTimer / 150.0;
                zoomScale = 8.0 - (7.0 * progress);

                if (!fallingSeeds.empty()) {
                    Seed& s = fallingSeeds[0];

                    cameraOffsetX = (screenWidth / 2.0 / zoomScale) - s.x;

                    double startScreenY = screenHeight / 2.0;
                    double endScreenY = (groundLevel + 25.0);
                    double currentScreenY = startScreenY + (endScreenY - startScreenY) * progress;

                    cameraOffsetY = (currentScreenY / zoomScale) - s.y;
                }

                treeGrowthScale = 1.0 - progress;
            } else {
                zoomScale = 1.0;
                cameraOffsetX = 0;
                cameraOffsetY = 0;
                if (!fallingSeeds.empty()) seedX = fallingSeeds[0].x;
                resetAnimation();
            }
            break;
        }
    }

    auto currentTime = std::chrono::steady_clock::now();
    double elapsedSeconds = std::chrono::duration<double>(currentTime - lastTime).count();
    lastTime = currentTime;

    sunAngle += 0.25 * elapsedSeconds;
    if (sunAngle > 2 * PI) sunAngle -= 2 * PI;
}

void AnimatedTreeDrawer::render() {
    setactivepage(1 - getactivepage());

    int r = 100 - static_cast<int>(50 * -sin(sunAngle));
    int g = 170 - static_cast<int>(100 * -sin(sunAngle));
    int b = 200 - static_cast<int>(80 * -sin(sunAngle));
    r = std::max(0, std::min(255, r));
    g = std::max(0, std::min(255, g));
    b = std::max(0, std::min(255, b));
    setbkcolor(COLOR(r, g, b));
    cleardevice();

    drawSun();
    drawClouds();

    int visualGroundY = static_cast<int>((groundLevel + cameraOffsetY) * zoomScale);
    int anchorX = static_cast<int>((seedX + cameraOffsetX) * zoomScale);
    int anchorY = static_cast<int>((seedY + cameraOffsetY) * zoomScale);

    int tempGround = groundLevel;
    groundLevel = visualGroundY;
    drawSoil();
    groundLevel = tempGround;

    bool newSeedHasLanded = false;
    if (!fallingSeeds.empty()) {
        for (const auto& s : fallingSeeds) {
            if (!s.active && s.y >= groundLevel - 1) {
                newSeedHasLanded = true;
                break;
            }
        }
    }

    for (const auto& s : fallingSeeds) {
        int sx = static_cast<int>((s.x + cameraOffsetX) * zoomScale);
        int sy = static_cast<int>((s.y + cameraOffsetY) * zoomScale);
        drawSeed(sx, sy, s.angle, 1.2 * zoomScale);
    }

    if (!newSeedHasLanded) {
        if (animationPhase >= 1 && animationPhase <= 4) {
            setcolor(COLOR(140, 100, 60));
            int connectionWidth = std::max(1, static_cast<int>(5 * zoomScale * treeGrowthScale));
            setlinestyle(SOLID_LINE, 0, connectionWidth);

            int bridgeTopY = anchorY - static_cast<int>((anchorY - visualGroundY) * treeGrowthScale);
            int finalTopY = std::max(bridgeTopY, visualGroundY);

            line(anchorX, anchorY, anchorX, finalTopY);
            drawRoot(anchorX, anchorY, 80.0 * zoomScale, PI / 2.0, 4, treeGrowthScale, visualGroundY);
        }

        int seedTop = anchorY - static_cast<int>(8 * zoomScale);
        int aboveGroundHeight = static_cast<int>(15 * zoomScale);
        int finalSproutTipY = visualGroundY - aboveGroundHeight;
        int totalSproutDist = seedTop - finalSproutTipY;

        if (animationPhase == 0) {
            if (phaseTimer > 15) {
                double sproutProgress = std::min(1.0, (phaseTimer - 15) / 25.0);

                double eased = sin(sproutProgress * PI / 2.0);
                int currentTipY = seedTop - static_cast<int>(eased * totalSproutDist);

                setcolor(LIGHT_GREEN);
                setlinestyle(SOLID_LINE, 0, 3);
                line(anchorX, seedTop, anchorX, currentTipY);

                double groundThreshold = (double)(seedTop - visualGroundY) / totalSproutDist;
                if (eased > groundThreshold) {
                    double leafGrowth = (eased - groundThreshold) / (1.0 - groundThreshold);
                    double leafScale = sin(leafGrowth * PI / 2.0) * zoomScale;

                    drawLeafShape(anchorX, currentTipY, -3 * PI / 4.0, leafScale, LIGHT_GREEN);
                    drawLeafShape(anchorX, currentTipY, -1 * PI / 4.0, leafScale, LIGHT_GREEN);
                }
            }
        } else if (animationPhase >= 1) {
            double growthProgress = treeGrowthScale;

            bool drawSeedling = (animationPhase < 4) || (animationPhase == 4 && growthProgress > 0.3);

            if (drawSeedling) {
                double colorBlend = std::min(1.0, growthProgress * 3.0);
                int rR = static_cast<int>(50 * (1 - colorBlend) + 101 * colorBlend);
                int rG = static_cast<int>(205 * (1 - colorBlend) + 67 * colorBlend);
                int rB = static_cast<int>(50 * (1 - colorBlend) + 33 * colorBlend);
                setcolor(COLOR(rR, rG, rB));
                setlinestyle(SOLID_LINE, 0, std::max(3, static_cast<int>(5 * zoomScale)));
                line(anchorX, seedTop, anchorX, finalSproutTipY);
            }

            drawBranch(anchorX, finalSproutTipY, 150 * zoomScale, PI / 2.0, 5, 1.0, growthProgress);

            double branchProgress = std::max(0.0, (growthProgress * 10.0) - 3.0);
            branchProgress = std::min(1.0, branchProgress);

            int trunkExtension = static_cast<int>(150 * zoomScale * 1.0 * branchProgress);
            int leafPositionY = finalSproutTipY - trunkExtension;

            if (drawSeedling) {
                drawLeafShape(anchorX, leafPositionY, -3 * PI / 4.0, 1.0 * zoomScale, LIGHT_GREEN);
                drawLeafShape(anchorX, leafPositionY, -1 * PI / 4.0, 1.0 * zoomScale, LIGHT_GREEN);
            }
        }

        if (animationPhase <= 2 && anchorY < screenHeight && anchorY > -100) {
            double morphFactor = std::max(0.0, 1.0 - (treeGrowthScale * 5.0));
            if (morphFactor > 0.01) {
                double currentScale = (animationPhase == 0 ? 1.0 + phaseTimer / 40.0 : 2.0) * morphFactor * zoomScale;
                drawSeed(anchorX, anchorY, 0, currentScale);
            }
        }
    }

    displayPhaseInfo();
    if (isPaused) {
        drawPauseMenu();
    }
    setvisualpage(getactivepage());
}

void AnimatedTreeDrawer::run() {
    initialize();
    isPaused = true;

    while (true) {
        if (kbhit()) {
            char key = getch();
            if (key == 'q' || key == 'Q') break;
            if (key == 'r' || key == 'R') {
                resetAnimation();
                isPaused = false;
            }
            if (key == ' ') isPaused = !isPaused;
        }

        if (!isPaused) {
            update();
        } else {
            lastTime = std::chrono::steady_clock::now();
        }

        render();
        delay(33);
    }
    closegraph();
}
