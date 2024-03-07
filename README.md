# projet_interfilliaire
Projet interfilliaire voiture autonome

This project aims to develop an autonomous vehicle system capable of detecting and following blue lines on the road while recognizing and interpreting traffic signs. The vehicle will operate autonomously and allow manual takeover via a controller. It will also assess whether it has enough energy for a round trip.

## Table of Contents

- Project Objective
- Key System Features
- Installation
- Usage
- Contributing
- License

## Project Objective

The primary objective is to design and develop an autonomous vehicle system that can:

1. Detect and track blue lines on the road using a camera system.
2. Recognize and interpret traffic signs using a database for decision-making.
3. Switch between autonomous and manual control using a controller.
4. Evaluate the vehicle's battery capacity for a round trip.

## Key System Features

1. **Blue Line Detection:**
   - Real-time image capture using a camera.
   - Algorithm to detect blue lines on the road.
   - Analysis of detected lines for vehicle trajectory.

2. **Traffic Sign Recognition:**
   - Database-driven recognition and interpretation of traffic signs.
   - Accurate detection for decision-making.

3. **Autonomous and Manual Modes:**
   - Integration of a switching system between autonomous and manual modes via a controller.
   - User capability to take control at any time.

4. **Battery Autonomy Assessment:**
   - Calculation of distance traveled and estimation of energy consumption for a return trip.

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/HotPantz/projet_interfilliaire.git

2. Compile the code:

   g++ -Wall sources/TrainAndTest.cpp -o recog `pkg-config --cflags --libs opencv4`

3. Run the code:

   ./recog

   
