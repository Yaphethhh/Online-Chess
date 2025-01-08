# README: Online Chess Project

## Overview

This project implements a fully functional online chess system, combining a C-based backend for game logic with a Python-driven web server for real-time interaction. The system includes a web-based user interface for playing chess, reviewing games, and managing game states. It is built using a mix of technologies, including C, Python, SQLite, SWIG, JavaScript, and HTML/CSS.

## Key Features

1. **Backend Logic (C)**

   - Chess rules and game logic are implemented in C.
   - The backend ensures accurate move validation and game state management.

2. **Python Web Server**

   - A Python-based web server facilitates real-time communication between players.
   - Utilizes SWIG (Simplified Wrapper and Interface Generator) to load the C backend as a shared module.

3. **Web Interface**

   - Created with HTML, CSS, and JavaScript (using chessboard.js) to provide a responsive user interface.
   - Supports real-time gameplay for both white and black players, including game-over detection and time-based move tracking.

4. **Game History and Review**

   - Game states, moves, and results are stored in an SQLite database.
   - Users can review previous matches via a dedicated web page.

## File Structure

- `hclib.c`: Contains the core game logic and move validation for chess.
- `hclib.i`: SWIG interface file to generate Python bindings for the C backend.
- `server.py`: Python script implementing the web server and handling player interactions.
- `index.html`: Main web page for the chessboard and gameplay interface.
- `chessboard.css`: CSS file for styling the web interface.
- `chessboard.js`: JavaScript library for rendering the chessboard and managing piece movement.
- `hostage_chess.db`: SQLite database to store game data.
- `history.html`: Web page for reviewing past games.

## How It Works

1. **Backend Logic**

   - The C backend processes each move to ensure it follows the rules of chess.
   - It manages the game state, including turn tracking, piece positions, and game-over conditions.

2. **Python Web Server**

   - Python acts as the bridge between the C backend and the web interface.
   - SWIG is used to generate bindings, allowing Python to call C functions directly.
   - Handles HTTP requests and serves the web interface to players.

3. **Web Interface**

   - The chessboard is rendered using chessboard.js.
   - Players interact with the board to make moves, which are sent to the Python server for validation via the C backend.

4. **Database Integration**

   - Each move is recorded in the SQLite database for persistence.
   - Players can review past games by querying the database through the review\.html page.
