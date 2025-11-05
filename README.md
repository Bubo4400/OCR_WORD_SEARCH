# NeurOctopus – OCR Word Search Solver

![Project Banner](images/banner.png)

## Overview
**NeurOctopus OCR Word Search Solver** is a C project developed by the **NeurOctopus** team.  
It automatically solves word search puzzles using:

- **Image Processing** for grid extraction  
- **Neural Network OCR** to recognize letters in images  
- **Word Search Solver Algorithm** to find all the words  
- **SDL User Interface (Coming Soon!)** to visualize the solution  

The goal is to turn a word search image into a solved puzzle with minimal user input.

---

## Features

- **Image Preprocessing**: Grayscale conversion, thresholding, and letter segmentation.
- **Neural Network OCR**: Recognizes letters from puzzle images using a trained neural network.
- **Word Search Solver**: Finds words in all directions (horizontal, vertical, diagonal).
- **SDL GUI**: Coming soon — visually displays the solved puzzle.

---

## Requirements

- **C Compiler** 
- **SDL2**
- **Make**

---

## Setup & Build

1. **Clone the repository:**

```bash
git clone https://github.com/yourusername/OCR-WordSearch.git
cd OCR-WordSearch
```

2. **Install SDL2 (On Linux):**

```bash
sudo apt-get install libsdl2-dev
```

3. **Compile the project:**

```bash
make
```

4. **Usage**

```bash
For the image detection part, go to the folder with this name and run : make <name of image to test> <name of final file>.
```
