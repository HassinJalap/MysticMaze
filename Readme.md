# 2D-world Game

## Description
This is a game in a 2-dimensional, grid-based world, in which the protagonist's goal is to kill all the enemies in the world before his health and energy runs out. There are health packs along the way to help with the diminishing health after killing the enemies. Each move that is done takes a certain amount of energy, and there are no energy boosters so the player has to think about the path they want to take in order to ensure that they'll be able to win the game.

## Features

- **Protagonist:** The game includes a protagonist character, represented by a Mario image, and can be moved with the WASD keys. 
- **Enemies:** There are four main types of enemies in the game:
    - **Generic Enemies:** The most basic ones simply stay at their position and consume a part of the protagonist's health when he tries to kill them.
    - **P(oison) Enemies:** A more advanced type of enemy is the PEnemy, which not only takes a part of the protagonist's health upon being attacked, but also poisons some of the area around it. Therefore, the protagonist's health could be further compromised if he stays present in the effected area.
    - **Teleporting Enemies:** Teleporting enemies are similar to the generic enemies except that they teleport from one place in the world to another after a certain period of time. 
    - **Growing Enemies:** Growing enemies stay put at one position but their strength continues to grow the longer they stay alive, so it would serve the protagonist to kill these types of enemies as soon as possible.
- **Health Packs:** Health packs are scattered throughout the game to restore the protagonist's health.
- **Views:** There are three different types of views the user can switch between, at any given time:
    - **Color-based Rendering:** Utilizes the grayscale value of the world image in order to provide a color-based display. The health of the protagonist is also reflected by the opacity of the image representing the protagonist. It is high in opacity when the health is high, and vice versa.
    - **Tile-based ASCII Art Rendering:** Utilizes ASCII art to represent the game environment. The protagonist is represented by `P`, the generic enemies by `E`, and the other two types of enemies by `X`. The protagonist's health is reflected by the color of the character it is represented by. For instance, the `P` is green when the health is high, but it's red when the health is low.
    - **Overlay Rendering:** Utilizes images on top of the color-based graphics view in order to give different visualisations for tiles with different values (e.g. tiles with infinite energy are represented with brick images to indicate that they cannot be traversed).
- **Autoplay:** Once clicked, the autoplay button provided in the UI will disable any further user input, and will try to win the game. The implemented algorithm first searches for the nearest enemy and checks if it can defeat it. If it can, it proceeds to do so but if not, it searches for the nearest healthpack(s) until it can defeat the (nearest) enemy. The movement from one position to another is done efficiently, based on the moves provided by the A* pathfinding algorithm. In this mode, all the tiles which are being traversed are marked in red, to visualize the path that is being taken.
- **Text-based commands:** There is also a text box provided if the user wants to play the game with text-based commands instead of the keyboard. For the movement, again, the pathfinding algorithm is used to be efficient in the energy. Some of the supported commands are:
    - **Attack nearest enemy:** Based on the euclidian distance, this will attack the nearest enemy.
    - **Take nearest healthpack:** Based on the euclidian distance, this will take the nearest healthpack.
    - **Go to tile x y:** Goes to the tile specified with the x and y coordinates, using the A* pathfinding algorithm. The tiles traversed during this are also marked in red to visualize the path.
- **Heuristic weight slider:** A slider is provided in the GUI, which determines the heuristic weight provided as an input to the pathfinding algorithm. A weight of 0 implies that the algorithm is Dijkstra, and a higher weight takes it closer to being best-first.
- **Zoom in/out:** Buttons are provided in the GUI for the player to easily be able to zoom in and out of the game whenever needed.
- **Protagonist-focused UI:** The user does not have to worry about scrolling to see the world when playing the game zoomed in; it automatically moves.

## How to Play

1. **Installation:**
    - Clone the repository: `git clone git@github.com:HassinJalap/MysticMaze.git`
    - Navigate to the Game folder in the project directory: `cd Game`

2. **Qt Creator:**
    - Make sure you have all the required dependencies installed with Qt Creator.
    - Open the project in QT creator by opening the `Game.pro` file.

3. **Run the Game:**
    - Build the project and run the game.


## Acknowledgments
Thanks for our lab instructor Dimitri Vargemidis for helping us with challenges throughout the semester, and to professor Stef Desmet for providing us with the knowledge in the lectures to be able to complete this project
