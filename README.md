# Tetris

<h1>Tetris (C & SDL2)</h1>

<p align="center">
  <img src="./ressource/Capture d'écran 2026-07-06 111135.png" width="800">
</p>

**Author:** Lionel DOFONHAKOU

## Overview

Tetris is a classic tile-matching puzzle game developed in **C** using the **SDL2** library and its extensions (**SDL_image**, **SDL_ttf**, and **SDL_mixer**).

The player controls falling puzzle pieces (tetrominoes) to create horizontal lines without gaps. When a line is completed, it disappears, and the player earns points. The game features real-time block falling, collision detection, a score system, a "next piece" preview, and background music.

The main purpose of this project was to deepen knowledge in C programming, specifically handling 2D arrays (matrices) for grid management, collision logic, and advanced 2D graphics rendering with SDL2.

## Educational Objectives

This project was designed to practice and reinforce the following concepts:

* Modular programming in C
* Dynamic memory allocation (2D grid manipulation)
* Matrix traversal and collision detection logic
* Event-driven programming and real-time game loops
* 2D graphics rendering with SDL2
* Text rendering with SDL_ttf and HUD (Head-Up Display) creation
* Audio playback and background music management with SDL_mixer
* Project organization using multiple source files and headers
* Compilation automation with Makefiles
* Memory debugging to ensure no leaks occur

## Game Description

The game takes place on a 25x25 grid. A random geometric shape falls from the top of the screen.

The game features 5 distinct forms:
* **L** : L-shape
* **C** : Square block
* **T** : T-shape
* **V** : Vertical line
* **H** : Horizontal line

**Gameplay Rules:**
* The player can move the falling piece left, right, or accelerate its descent.
* A "Next" panel in the top-right corner displays the upcoming piece so the player can strategize.
* When a full horizontal line is filled with blocks, it is cleared from the board, and all blocks above it fall down.
* **Score:** Each cleared line grants **10 points**. The objective is to reach the maximum score (`YOU WIN` screen at 1000 points).
* **Game Over:** The player loses if the stacked blocks reach the top of the grid, preventing new blocks from spawning.

## Installation

### Required Dependencies

On Ubuntu / WSL:

```bash
sudo apt update

sudo apt install build-essential
sudo apt install libsdl2-dev
sudo apt install libsdl2-image-dev
sudo apt install libsdl2-ttf-dev
sudo apt install libsdl2-mixer-dev
sudo apt install valgrind