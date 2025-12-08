# Microcontroller-Based Space Shooter Game – Final Project Report

## 1. Introduction
This project implements a 96×96 pixel space shooter game on a SAMD51 microcontroller using C. The game gives smooth gameplay through player controlled spaceship movement on an analog joystick, single and multiple projectile shooting for the user, enemy spawning, collision detection, and an animated scrolling background full of stars. All tasks runs under the Rate Monotonic Scheduling (RMS) system with a 1 ms SysTick timer as the time base.

## 2. Project Objectives
- Provide responsive spaceship control with variable speed going from 1-3 pixels/frame using an analog joystick.
- Implement both single shot and an automatic (multiple projectiles) firing modes.
- Generate enemy ships with each one having their own projectile logic.
- Create a continuously scrolling space background for visual looks.
- Design a fixed-priority schedule that meets all deadlines while keeping the flow of the screen smooth looking.
- Optimize rendering to eliminate visible lag .

## 3. Implementation Details
### 3.1 Input Handling
#### 3.1.1 Joystick
The joystick outputs 12-bit ADC values (0–4096). Initially, the readings from the joystick were too jumpy. They were idling at ~4090 and jumping to ~4096 or 0 depending on the movement of the joystick. It became hard to control it so I tried to find a value in it that was consitent. after analysis, I realized that the first few bits are always too jumpy while the last 3 bits are comsistent with the movement of the joystick. So, I choose to keep the last 3-bits which gave me values from 0-6. Each values received was taken into count as a distance from the idle values which gave me the number of pixel I could move the user spaceship.  The values were as shown:
- 3 = centered (idle)
- 0, 6 = highest difference from 3 (3 pixels/frame)
- 1, 5 = medium difference from 3 (2 pixels/frame)
- 2, 4 = lowest difference from 3 (1 pixels/frame)

This simple approach produced smooth and predictable movement without needing too much calculation or any floating-point math.

#### 3.1.2 Shoot Button
The shoot button is polled as a regular task (not interrupt-driven). This allowed me to do something that would have been difficult using interrupts. This allowed me to give it a specific rate at which it ran in RM scheduling. Which creates this illustion of a single shot when pressed once while multiple shots fired when held down. Due to its 100 ms period, the user can easily click once without having multiple shots fired while also not having to sample the input for some number of time to allow for multiple shoot.

#### 3.1.3 Display
During the initialization, the orientation of the screen was given preference. The screen is not wrongly positioned, it is in the correct orientation. The game was designed to play in the portrait mode. The display initialization was a frustrating task since it required things that were too confusing. However, through datasheets and experimentation, correct pins and cofiguration were found to work with the display.

### 3.2 Graphics and Rendering Strategy
All sprites (player ship, enemies, projectiles) are stored as bitmap arrays and drawn with the display driver’s `drawArray(...arguments...)` function.

At first, I was updating the entire 96×96 framebuffer in a single task. This caused all the tasks other than the updating display to run in under ~3 ms together while update display took up ~82 ms alone. This meant I could have no more than 10 FPS. Very slow and feels delayed. To improve this, I decided to spread my display update task to individual tasks where each task updated it portion on the display without affecting the others portions. This improved the frames by a lot. 

Due to each task running at a certain period, different items had different frames yet they all felt smooth.
- Task running at 20 ms got 50 FPS
- Tasks running at 50 ms got 20 FPS
- Tasks running at 100 ms got 10 FPS.

Although the 10 FPS is bad, tasks running at 100 ms are not display updating tasks. The only task that updates display at that speed is the background scrolling but it looks smooth as well due to everything else around it being smooth.

The human eye percieves these updates into smooth and fluid movements.

### 3.3 Task Schedule (Rate Monotonic)
| Task                                | Period (ms) | Priority | Execution Time              | Purpose                              |
|------------------------------------|-------------|----------|----------------------------|-------------------------------------|
| Projectile Update                  | 20           | Highest  | 5 ms                        | Fast, smooth bullet movement         |
| Update player location             | 50           | Medium   | 2 ms                        | Responsive feeling                   |
| Add enemy projectile               | 50           | Medium   | 1 ms                        | Responsive controls & AI             |
| Updating Enemy Projectile location | 50           | Medium   | 5 ms                        | Natural fire rate                    |
| Moving background on the screen    | 100          | Lowest   | 25 ms                       | slow yet smooth background movement |
| Player shooting                    | 100          | Lowest   | 1 ms                        | Timely and responsive shooting       |
| Enemies spawning                   | 100          | Lowest   | 10 ms                       | Timely and responsive updates        |

All tasks meet their deadlines with their frame restarting at every 100 ms. Their scheduling cycle repeats every 100 ms as shown below.

| Time (ms) | Task                 |
|----------|-----------------------|
| 0-5      | updateProjectile      |
| 5–7      | updateSpacePosition   |
| 7-8      | addEnemyProjectile    |
| 8-13     | updateEnemyProjectile |
| 13-28    | moveBackground        |
| 28-33    | updateProjectile      |
| 33-34    | shoot                 |
| 34-44    | spawnenemies          |
| 44-49    | updateProjectile      |
| 49-50    | slack time            |
| 50-52    | updateSpacePosition   |
| 52-53    | addEnemyProjectile    |
| 53-58    | updateEnemyProjectile |
| 58-60    | slack time            |
| 60-65    | updateProjectile      |
| 65-80    | slack time            |
| 80-85    | updateProjectile      |
| 85-100   | Frame ends            |

100 ms is the frame cycle period. Not only the last frame ends but a new one starts here. The timeline shown above repeats again and again every 100 ms.

### 3.4 Game Logic and Collision Detection
Implemented in `displayDrawer.c` using the function `intersect(...arguments...)`, collision checks if a given corner is inside a given box. On hit (upon finding out that the given corner is inside the box):
- Player -> Game Over
- Player bullet -> Enemy -> enemy removal
- Enemy bullet -> Player -> Game Over
- Player -> Enemy -> Game Over

The projectiles cannot hit each other as they are lazers and I decided to not have lazers touch each other by default.

### 3.5 Reset Mechanism
The joystick center button triggers a hardware interrupt that immediately reinitializes all game state variables and returns to the title screen — no need to reprogram the board or turn it on and off. This task is interrupt driven and is implemented for the sake of easing the resetting of the game. It is not part of the schedule hence it can only be run when no task is being run. It will be turned off while tasks are being run since it is not part of the schedule at all.

## 4. Experiments and Optimizations

### 4.1 Display Update Strategy
Experiment 1: Full-screen refresh every frame  
- Total non-display tasks executed in <3 ms, but display task took 82 ms which gave visible stutter. Although not unpleasent, it didn't look good.

Solution: Individual object rendering  
- Distributed drawing calls increased perceived frames more than they actully were.

### 4.2 Joystick Calibration
Problem: Raw 12-bit values jumped unpredictably between 0 and ~4090.  
Discovery: Only the lowest 3 bits changed meaningfully with movement.  
Fix: Mask with `value & 0x07` giving stable and repeatable 0–6 range values.

### 4.3 Task Period Tuning
- 20 ms projectile period was essential for bullets to feel faster than the ship without appearing jumpy.
- 50 ms proved the sweet spot for player and enemy updates — ≥20 FPS with full deadline compliance.
- 100 ms background and spawn periods gave acceptable visuals while freeing CPU cycles for gameplay-critical tasks.
