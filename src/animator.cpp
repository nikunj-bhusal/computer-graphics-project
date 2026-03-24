#include "../include/animator.hpp"

#include <graphics.h>

#include <chrono>
#include <cmath>

void AnimatedTreeDrawer::drawSeed(int x, int y, double angle, double scale) {
    // drawSeed() katai call hunu agadi j setcolor() gareko thiyo, tyo na bigrios bhanera.
    // getcolor() returns an integer representing the current drawing color (the one set by the last setcolor() call

    int oldColor = getcolor();

    setcolor(SEED_COLOR);  // border ko lagi
    setfillstyle(SOLID_FILL, SEED_COLOR);

    int size = static_cast<int>(8 * scale);

    // seed is drawn by connecting 12 points with lines
    const int numPoints = 12;

    // times 2 for storing X and Y positions of 12 points
    int points[numPoints * 2];

    for (int i = 0; i < numPoints; i++) {
        // results in 0deg, 30deg, 60deg, ..., 330deg
        //    which are the angular positions of the 12 points
        double theta = i * 2 * PI / numPoints;

        // y = a*cos(theta), x = (a/2)*sin(theta)
        //    divided by 2 bcz seed is an ellipse, not a circle
        double localX = size * cos(theta);
        double localY = size * 0.5 * sin(theta);

        //  Anti-clockwise rotation formula
        // x' = x * cos(angle) - y * sin (angle)
        // y' = x * sin(angle) + y * cos (angle)
        int rotatedX = static_cast<int>(localX * cos(angle) - localY * sin(angle));
        int rotatedY = static_cast<int>(localX * sin(angle) + localY * cos(angle));

        // adding x and y so that points are calculated with center of seed as origin
        points[i * 2] = x + rotatedX;
        points[i * 2 + 1] = y + rotatedY;
    }

    // reads the 24 values of "points" array then treats them as 12 coordinates, joins them, then fills them
    fillpoly(numPoints, points);

    int slitLength = static_cast<int>(size * 0.8);  // make it slightly shorter than the radius */
    //  these four are coordinates of the end points of the slit
    // multiplying by cos and sin ensures that the line points to the direction of the seed
    // adding x and y makes sure the points are calculated with center of the seed as origin
    int lineX1 = x + static_cast<int>(slitLength * cos(angle));
    int lineY1 = y + static_cast<int>(slitLength * sin(angle));
    int lineX2 = x - static_cast<int>(slitLength * cos(angle));
    int lineY2 = y - static_cast<int>(slitLength * sin(angle));

    setcolor(SEED_SLIT_COLOR);
    setlinestyle(SOLID_LINE, 0, std::max(1, static_cast<int>(scale / 3)));
    line(lineX1, lineY1, lineX2, lineY2);

    setcolor(oldColor);  // reverting back to the setcolor() that was before calling this function
}

void AnimatedTreeDrawer::drawSoil() {
    setfillstyle(SOLID_FILL, SOIL_BROWN);
    // bar(left, top, right, bottom)
    // -> This fills a rectangle that has sides that are passed as the argument, in the style as passed in setfillstyle().
    bar(0, groundLevel, screenWidth, screenHeight);
}

void AnimatedTreeDrawer::drawBranch(int x1, int y1, double length, double angle, int depth, double scale, double growthProgress) {  // NIRDESH
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
                setcolor(LEAF_GREEN);
                setfillstyle(SOLID_FILL, LEAF_GREEN);
            }

            double branchPos = (i % 2 == 0) ? 1.0 : 0.85;
            int bx = x1 + (x2 - x1) * branchPos;
            int by = y1 + (y2 - y1) * branchPos;

            double leafAngle = i * (6.28 / numLeaves);
            int radius = (i % 3 == 0) ? 12 : 6;
            radius *= scale;

            int offsetX = cos(leafAngle) * radius;
            int offsetY = (sin(leafAngle) * radius) + (i % 4 == 0 ? -8 : 4);

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

    int oldColor = getcolor();  // saving the current color to restore it later
    // even if the zoom is small, the size of the flowers is at least 2 pixels big
    int baseSize = std::max(2, static_cast<int>(3 * scale));

    // all of below code has the same logic except for their radius

    // setcolor(SHADOW_PETAL);
    // setfillstyle(SOLID_FILL, SHADOW_PETAL);

    setcolor(PETAL_COLOR);
    setfillstyle(SOLID_FILL, PETAL_COLOR);
    fillellipse(x - baseSize, y, baseSize + 2, baseSize + 1);  // left
    fillellipse(x, y + baseSize, baseSize + 1, baseSize + 2);  // top
    fillellipse(x + baseSize, y, baseSize + 2, baseSize + 1);  // right
    fillellipse(x, y - baseSize, baseSize + 1, baseSize + 2);  // bottom

    setcolor(CENTER_DARK);
    setfillstyle(SOLID_FILL, CENTER_DARK);
    int centerRadius = std::max(1, (baseSize / 2) + 1);
    fillellipse(x, y, centerRadius, centerRadius);

    setcolor(oldColor);  // restore old setcolor() value back
}

void AnimatedTreeDrawer::drawSun() {  // NIRDESH
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
    setcolor(WHITE);  // so that the border of the circles are set to WHITE
    setfillstyle(SOLID_FILL, WHITE);
    //
    // cloud = 0 to 4 ==> 4 ota cloud
    for (int cloud = 0; cloud < 4; cloud++) {
        int cloudX = 50 + cloud * 200;

        // position mathi tala hos bhanera odd even
        int cloudY = 80 + ((cloud % 2 == 0) ? 25 : -25);
        for (int i = 0; i < 6; i++) {
            int circleX = cloudX + i * 25;
            int circleY = cloudY + ((i * 13) % 20);
            int radius = 20 + (i * 7) % 10;

            // fillellipse(x_position, y_position, x_radius, y_radius)
            fillellipse(circleX, circleY, radius, radius);
        }
    }
}

void AnimatedTreeDrawer::updateFallingSeed() {
    if (!hasFallingSeed) return;

    if (fallingSeed.y < groundLevel) {           // if fallingSeed is above the ground
        fallingSeed.velocityY += 0.02;           // increase vertical falling seed
        fallingSeed.y += fallingSeed.velocityY;  // position ground tira lagne
        fallingSeed.x += fallingSeed.velocityX;  // position update garne RIGHT tira

        // calculate distance between seed ko y level from ground
        double distanceToGround = groundLevel - fallingSeed.y;

        // calculate rotation angle based on that distance so that seed always lands horizontally
        // distanceToGround / 36.0 is for the speed of rotation
        // changing 36.0 to a higher value decreases the speed of rotation
        // adding 4*PI ensures about 2 complete rotations
        fallingSeed.angle = (distanceToGround / 36.0) + (PI * 4);
    } else {                        // otherwise, (if seed touches the ground)
        fallingSeed.angle = 0;      // make it stay cmopletely horizontal
        fallingSeed.velocityY = 0;  // make it stop moving vertically
        fallingSeed.velocityX = 0;  // make it stop moving horizontally

        if (fallingSeed.y < groundLevel + 25) {  // i.e., when seed is inside the ground but above the final resting position,
            fallingSeed.y += 0.4;                // stop moving horizontally but keep moving down slowly
        } else {
            // otherwise, fix the seed in its resting position (which is also the initial position (GND level + 25))
            fallingSeed.y = groundLevel + 25;
        }
    }
}

void AnimatedTreeDrawer::displayPhaseInfo() {
    setcolor(PHASE_INFO_TEXT);
    settextstyle(SIMPLEX_FONT, HORIZ_DIR, 1);

    // sprintf() saves a string into a character array.In the code below,
    // it saves the string that 's passed in the second argument in the character array that' s passed in the first argument.
    char phaseInfo[100];
    switch (animationPhase) {
        case 0:
            sprintf(phaseInfo, "Phase 1: Seed Germination");
            break;
        case 1:
            sprintf(phaseInfo, "Phase 2: Seedling Growth");
            break;
        case 2:
            sprintf(phaseInfo, "Phase 3: Tree Maturation");
            break;
        case 3:
            sprintf(phaseInfo, "Phase 4: Flowering");
            break;
        case 4:
            sprintf(phaseInfo, "Phase 5: Seed Dispersal");
            break;
        case 5:
            sprintf(phaseInfo, "Phase 6: Cycle Reset");
            break;
    }
    outtextxy(20, 20, phaseInfo);
}

void AnimatedTreeDrawer::drawPauseMenu() {
    int boxW = 345, boxH = 135;  // (width, height) of pause menu box = (345, 135)
    int midX = screenWidth / 2;  // (X_position, Y_position) = (midX, midY)
    int midY = screenHeight / 2;

    setcolor(PAUSE_MENU_BORDER);

    // setlinestyle() ==> second argument passed is 0 bcz we are using a pre-defined style (SOLID_LINE), third parameter is thickness of line (3
    // pixels thick)
    setlinestyle(SOLID_LINE, 0, 3);

    // rectangle(left, top, right, bottom)
    rectangle(midX - boxW / 2, midY - boxH / 2, midX + boxW / 2, midY + boxH / 2);

    // starting from midpoint of pause menu, PAUSE_MENU_BORDER na pugunjel fill garne. fill color is defined by setfillstyle()
    setfillstyle(SOLID_FILL, WHITE);
    floodfill(midX, midY, PAUSE_MENU_BORDER);

    // text ko background color = WHITE, text ko color = BLACK
    setbkcolor(WHITE);
    setcolor(BLACK);

    // minus gareko values are all manually adjusted, no logic
    settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);
    outtextxy(midX - 55, midY - 55, (char*)"PAUSED");

    settextstyle(SIMPLEX_FONT, HORIZ_DIR, 1);
    outtextxy(midX - 160, midY - 10, (char*)"[Space] - Play / Pause");
    outtextxy(midX - 160, midY + 15, (char*)"[R]        - Reset Animation");
    outtextxy(midX - 160, midY + 40, (char*)"[Q]        - Quit Animation");
}

void AnimatedTreeDrawer::drawLeafShape(int x, int y, double angle, double scale, int color) {
    int oldColor = getcolor();  // same logic as before
    setcolor(color);
    setfillstyle(SOLID_FILL, color);

    int size = static_cast<int>(10 * scale);
    const int numPoints = 12;   // same logic as before
    int points[numPoints * 2];  // same logic as before

    for (int i = 0; i < numPoints; i++) {       // same logic as before
        double theta = i * 2 * PI / numPoints;  // same logic as before

        double localX = size * cos(theta) + size;  // +size garena bhane 4 ota aauchha for some reason
        double localY = size * 0.5 * sin(theta);   // same logic as before

        // same logic as before
        int rotatedX = static_cast<int>(localX * cos(angle) - localY * sin(angle));
        int rotatedY = static_cast<int>(localX * sin(angle) + localY * cos(angle));

        // same logic as before
        points[i * 2] = x + rotatedX;
        points[i * 2 + 1] = y + rotatedY;
    }

    // same logic as before
    fillpoly(numPoints, points);
    setcolor(oldColor);
}

void AnimatedTreeDrawer::drawRoot(int x1, int y1, double length, double angle, int depth, double growthProgress, int surfaceLimitY) {  // NABARAJ
    if (depth <= 0 || growthProgress <= 0) return;

    // if depth is different, change color
    int rootColor = (depth > 3) ? ROOT_DEEP : ROOT_SHALLOW;
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

AnimatedTreeDrawer::AnimatedTreeDrawer()  // constructor
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
      frameCount(0),
      zoomScale(1.0),
      cameraOffsetX(0),
      cameraOffsetY(0),
      hasFallingSeed(false) {}

void AnimatedTreeDrawer::resetAnimation() {
    // values at the start of the animation
    treeGrowthScale = 0.0;
    flowerScale = 0.0;
    showFlowers = false;
    animationPhase = 0;
    frameCount = 0;
    hasFallingSeed = false;
    seedX = screenWidth / 2;
    seedY = groundLevel + 25;
    zoomScale = 1.0;
    cameraOffsetX = 0;
    cameraOffsetY = 0;
}

void AnimatedTreeDrawer::initialize() {
    // lastTime is a time point representing the current point in time
    lastTime = std::chrono::steady_clock::now();
    initwindow(screenWidth, screenHeight, "Animated Tree Life Cycle");
    setbkcolor(SKY_BLUE);
    cleardevice();

    // setactivepage(int page) directs all drawing commands to a specific memory buffer,
    //     while setvisualpage(int page) determines which buffer is currently displayed on the screen

    //     Active Page : The "hidden" page where you draw(setactivepage)
    //         Visual Page : The page currently visible to the user(setvisualpage)
    //         Purpose : By drawing on a hidden page and then switching it to
    //                    visual(often setting them to 0 and 1),
    //     you prevent screen flickering during complex animations.

    setactivepage(0);
    setvisualpage(0);
}

void AnimatedTreeDrawer::update() {
    // increments each time this function is called
    // 30 times per second bcz delay(33)
    frameCount += 1;

    switch (animationPhase) {
        case 0:  // Seed germination (0-40 frames)
            if (frameCount < 40) {
                // if seed dekhaako 40 frame pani bhako chhaina bhane tree nadekhaune
                treeGrowthScale = 0.0;
            } else {  // after 40 frames, move to next phase and reset timer for new phase
                animationPhase = 1;
                frameCount = 0;
            }
            break;

        case 1: {  // Leaf phase
            if (frameCount < 60) {
                // during this phase, the tree will have grown from 0 to 15% over 60 frames
                treeGrowthScale = 0.15 * (frameCount / 60.0);
            } else {  // same thing as case 0
                animationPhase = 2;
                frameCount = 0;
            }
            break;
        }

        case 2: {  // Tree growth
            if (frameCount < 100) {
                // case 2 bhanda agadi 15% tree growth saki sake ko hunchha (so 0.15 + some_other_terms)
                // some_other_terms basically means:
                // "over 100 frames, tree should grow the rest 85% (100% - 15% = 85%)"
                treeGrowthScale = 0.15 + (frameCount / 100.0) * 0.85;
            } else {
                // 100 frame pachhi tree will have reached 100% growth
                // so the treeGrowthScale = 1.0 in else case (after 100 frames)
                // tree growth complete bhae pachhi showFlower = true garne, others are same as above
                treeGrowthScale = 1.0;
                showFlowers = true;
                animationPhase = 3;
                frameCount = 0;
            }
            break;
        }

        case 3: {  // Flowering
            if (frameCount < 100) {
                // same logic as above, flower doesn't grow gradually as frame changes so no need to multiply by anytihng
                flowerScale = frameCount / 100.0;
            } else {
                // after 100 frames, change animation phase and reset the counter
                animationPhase = 4;
                frameCount = 0;

                // initialize the fallingSeed structure in this phase (new seed that falls)
                fallingSeed.x = seedX + 150;  // manually set values. no logic behind +150 and -360
                fallingSeed.y = seedY - 360;
                fallingSeed.angle = 60;  // manually set initial angle. looked nice so 60deg
                fallingSeed.velocityX = 0.8;
                fallingSeed.velocityY = 0.1;
                hasFallingSeed = true;
            }
            break;
        }

        case 4: {  // Falling phase
            // each time update() is called and animation is in phase 4, update the falling seed coordinates
            updateFallingSeed();

            if (hasFallingSeed) {  // is a seed is falling in the screen

                // gradually update the zoomScale from 1.0 (initial value) to maxm 8.0
                // increasing by 0.05 in each FRAME
                if (zoomScale < 8.0) zoomScale += 0.05;

                // start to pan the camera from the center of the screen
                double startFocusX = screenWidth / 2.0;
                double startFocusY = screenHeight / 2.0;

                // target of panning the camera is the falling seed
                double targetFocusX = fallingSeed.x;
                double targetFocusY = fallingSeed.y;

                // this is to make sure that the camera panning is complete (0.0 to 1.0) ofer 80 frames to ensure smooth camera movement
                double panProgress = std::min(1.0, frameCount / 80.0);

                // cubic ease out function so that the panning starts out slow then speeds up instead of just going linear
                double ease = 1.0 - pow(1.0 - panProgress, 3);

                // find distance between the TARGET and the STARTING (targetFocusX - startFocusX)
                // then *ease for speeding up from slow to fast camera movement
                // add to STARTing point to get a point in between the START and TARGET point
                double currentFocusX = startFocusX + (targetFocusX - startFocusX) * ease;
                double currentFocusY = startFocusY + (targetFocusY - startFocusY) * ease;

                // take zoomScale = 1.0 and screenWidth / 2.0 = midX
                // then  cameraOffsetX = midX - currentFocusX (x' = x - h)
                // i.e., new origin of the screen is currentFocusX (h)
                // new coordinate is cameraOffsetX (x') which corresponds to
                // old coordinate midX (x)
                cameraOffsetX = (screenWidth / 2.0 / zoomScale) - currentFocusX;
                cameraOffsetY = (screenHeight / 2.0 / zoomScale) - currentFocusY;

                // when seed is in contact with the ground,
                if (fallingSeed.y >= groundLevel + 25) {
                    // but only if 100 frames have been drawn,
                    if (frameCount > 100) {  // change the animation phase and reset frameCount
                        animationPhase = 5;
                        frameCount = 0;
                    }
                }
            }
            break;
        }

        case 5: {                    // Zoom out phase
            if (frameCount < 150) {  // this phase lasts for 150 frames
                // progress should go from 0% to 100% over 150 frames
                double progress = frameCount / 150.0;

                // zoomScale decreases linearly from maxm 8.0 to 1.0 (thus the *7.0)
                // when progress = 0%, zoomScale = 8.0
                // when progress = 50%, zoomScale = 4.5
                // when progress = 100%, zoomScale = 1.0
                zoomScale = 8.0 - (7.0 * progress);

                if (hasFallingSeed) {
                    // same logic as above but now, the horizontal center should be the seed
                    // that landed in the ground
                    cameraOffsetX = (screenWidth / 2.0 / zoomScale) - fallingSeed.x;

                    double startScreenY = screenHeight / 2.0;
                    double endScreenY = (groundLevel + 25.0);

                    // screen height ko center dekhi seed ko final position samma point haru find out garne
                    double currentScreenY = startScreenY + (endScreenY - startScreenY) * progress;

                    cameraOffsetY = (currentScreenY / zoomScale) - fallingSeed.y;
                }

                // the fully grown tree should disappear as the zoom out progresses
                treeGrowthScale = 1.0 - progress;
            } else {                // finally
                zoomScale = 1.0;    // reset the zoomScale
                cameraOffsetX = 0;  // reset camera
                cameraOffsetY = 0;
                if (hasFallingSeed) seedX = fallingSeed.x; // set initial seed position to final seed position
                resetAnimation(); // then reset animation
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
    if (hasFallingSeed) {
        if (fallingSeed.y >= groundLevel + 25) {
            newSeedHasLanded = true;
        }
    }

    if (hasFallingSeed) {
        int sx = static_cast<int>((fallingSeed.x + cameraOffsetX) * zoomScale);
        int sy = static_cast<int>((fallingSeed.y + cameraOffsetY) * zoomScale);
        drawSeed(sx, sy, fallingSeed.angle, 1.2 * zoomScale);
    }

    if (!newSeedHasLanded) {
        if (animationPhase >= 1 && animationPhase <= 4) {
            setcolor(ROOT_DEEP);
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
            if (frameCount > 15) {
                double sproutProgress = std::min(1.0, (frameCount - 15) / 25.0);

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
                double currentScale = (animationPhase == 0 ? 1.0 + frameCount / 40.0 : 2.0) * morphFactor * zoomScale;
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
    // this function is what causes a frame to be drawn in an interval set in delay function */
    initialize();
    isPaused = true;

    while (true) {
        if (kbhit()) {
            char key = getch();
            if (key == 'q' || key == 'Q') break;
            if (key == 'r' || key == 'R') {
                resetAnimation();  // resetting all progress causes animation to start from beginning
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
        // Formula:
        // Delay[millisecond] = 1000 / Target FPS
        // if Target FPS = 30fps, delay ~33ms (33.333ms)
        // if Target FPS = 60fps, delay ~17ms (16.667ms)
        delay(17);
    }
    closegraph();
}
