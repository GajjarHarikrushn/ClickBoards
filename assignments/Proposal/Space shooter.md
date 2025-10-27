# Space Shooter

## Overview
I plan to create a Space Shooter game that demonstrates multitasking and event-driven behavior on the embedded system.  
The player will control a spaceship using a joystick, shoot projectiles, and avoid or destroy enemy ships.  
The game will feature multiple independently running tasks that together create smooth gameplay.

## Tasks
The game will include seven independent tasks:

1. Player Movement
    - Handles input from the joystick to move the player’s spaceship on the screen.  
    - Runs on interrupts, updating the player’s position based on user input.

2. Player Projectiles
    - Fires when the user presses the fire button.  
    - Once fired, projectiles move independently of the spaceship until they leave the screen or collide with an enemy.

3. Enemy Spawning
    - Spawns new enemy ships after a certain number of enemies have been destroyed.  
    - This will be event based rather than time or interrupt based.

4. Collision Detection
    - Checks for collisions between projectiles and enemy ships.  
    - Triggered whenever projectile or enemy positions overlap (event based task).

5. Background Scrolling
    - Moves the background continuously at a fixed pace to create the illusion of movement through space.  
    - Runs independently of other tasks.

6. Enemy Projectiles
    - Handles enemy firing logic.  
    - Enemy bullets move independently of both the enemy ships and the player.

7. Screen Refresh
    - Updates the display at a fixed rate to ensure smooth animation and consistent gameplay visuals.

These seven concurrent tasks will together create the Space Shooter game. It will showcase multitasking, event handling, and user interactions.

## Peripherals

1. OLED Screen
    - Displays all game elements, including the player, enemies, projectiles, and background.  
    - Essential for visual output and verifying that all concurrent tasks are synchronized properly.

2. Joystick
    - Provides analog control for player movement in multiple directions.  
    - Offers intuitive and responsive gameplay interaction.

3. Main Board Button (SW0)
    - Used for firing projectiles.  
    - A simple, reliable input method that complements the joystick for dual control.

## Relation to the Course
This project is related to course because it:
- Demonstrates multitasking with several concurrent and event-driven tasks.  
- Utilizes multiple hardware peripherals (display, joystick, button).  
- Is significantly more complex than simple games like Pong or Tetris.  
- Provides a clear platform to explore synchronization, timing, and event handling in an embedded environment.

