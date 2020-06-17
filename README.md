# Hide And Seek With AI

A test assignment for a simple hide and seek game with AI in pure C++ and Unreal Engine 4

### How to checkout

Since [the game's Content directory hosted on GitLab as a separate submodule repository](https://gitlab.com/NuLL3rr0r/HideAndSeekWithAI_Content), for checking out the complete source code and assets, use the following instructions.

For GitLab checkouts:

```
$ git clone --recursive --jobs 16 https://gitlab.com/NuLL3rr0r/HideAndSeekWithAI.git
```

For GitHub checkouts:

```
$ git clone --recursive --jobs 16 https://github.com/NuLL3rr0r/HideAndSeekWithAI.git
```

### Introduction

Create a new Basic Code project, with starter content, named "Hide and seek with AI" The best approach is to use C++ for building base gameplay systems and performance-critical code with Blueprints used to customize behavior or create composite behaviors from C++ building blocks. Your goal is to create a simple game prototype with AI Bots and a character controller.

### Environment

1. Location - the big rectangular room 120m x 30m.
2. Start spot - position where the player spawned at the beginning of the game. Located at the end of the room;
3. Win spot - a small green area, located at the opposite end of the room 1m x 1m;
4. Small cubes - represent an abstract item player can pick up and throw forward. In the number of twenty-four pieces randomly spawned in the room 0.2m x 0.2m x 0.2m each;
5. Big static cube - represents an obstacle. AI bots can't see the player through the obstacles. In the number of twenty-four pieces randomly spawned in the room 3m x 3m x 3m each.

### Player

Abstract 1m height cylinder:

1. First-Person camera view;
2. WASD movement;
3. LMB - pick up / throw the items;
4. Throwing power increases by holding the LMB down from min to max in 2 sec. The minimum throwing distance is 1m, and the maximum is 15m;
5. No inventory system, the player can carry only one item at a time;
6. The player's goal is to get from one end of the room to the other. Whenever the player triggers the win spot - game is completed.

### Bots

Abstract AI-driven 1 meter height cylinders in the number of six randomly spawned in the room. AI specifications below:

1. AI's goal is to chase and catch the player. If the bot gets the player game is over and should be restarted automatically;
2. The AI's movement speed is twice faster than the player's speed, which makes the player pretty easy to catch.
3. Bots chase after the player only when seeing him. The field of view angle is 70 degrees; raycast distance is 20 meters. The player should see the debug splines of FOV;
4. Whenever bots can't see the player, they slowly move to their spawn spot with 20% of chasing speed;
5. Standing on the spawn spot bot constantly looks around with random intervals each time set in the range from 3 to 12 sec;
6. The player can distract bot by throwing items (small cubes) but only on the condition that bot can't see the player. The bot can hear how the item hits the floor, walls, or obstacles if the distance between him and the item's collision spot is less than 20 meters. Every time hearing item collision bot should go there with 50% of the chasing speed. On collision spot, the bot should look around, and if he can't see the player, he has to bring the item to its spawn spot and then go back to own spawn spot with 20% of chasing speed. Whenever bot sees the player, he should stop the current activity (holding the respawn spot, moving to collision spot or carrying item) and start chasing after the player;
7. If bot lost sight of the player, he has to reach the point where he last saw player and look around there. If the player is disappeared, the bot should go back to his spawn position with 20% of chasing speed.

### Expected gameplay

Player placed on start spot. Bots randomly spawned on the map in such a way that provides a safe start position for the player. If the start position isn't safe - restart the entire level automatically.

The player beats the game if he passes through the level without getting caught by bots. Player should throw the items to distract bots and sneak behind the obstacles to get to the win spot.

### Requirements

1. The test project must be completed using the latest version of the Unreal Engine;
2. The project’s archive should contain UE project and compiled desktop application for Windows;
3. Keep your code clean and readable, don't forget to leave the comments with sufficient information;
4. Consider this project as the early prototype for a Game Designer where all important parameters should be accessible and customizable on the fly;
5. The recommended completion time is 16 hours (two working days);
6. Please attach a short summary of your work to the email. Describe in a few words how you assessed, structured, and completed the test project. What parts where you spend most of your time and explain why. You can bring up there the useful solutions you’ve implemented which have facilitated the Game Designer's work.

Thank you for your time and good luck with the test assignment!

## License

(The MIT License)

Copyright (c) 2020 Mamadou Babaei

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the ‘Software’), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED ‘AS IS’, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
