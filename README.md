# Puara

This addon provides a collection of powerful, real-time gesture and biodata processing nodes for your [ossia score](https://ossia.io) projects.

Easily analyze data from motion sensors (like accelerometers and gyroscopes), biometric sensors (like heart rate and GSR), and simple buttons directly within your score environment.

# Available Nodes

This addon adds the following processes in your library, under categories such as Gestures, Biodata, and Analysis:

- BioData Heart: Processes a raw heart signal to extract BPM, beat events, and other metrics.
- BioData Skin Conductance: Analyzes a skin conductance signal to get its tonic (SCL) and phasic (SCR) components.
- Button Processor: Detects single taps, double taps, triple taps, and holds from a simple button input.
- Gesture Recognizer: A comprehensive node for analyzing IMU and biodata to get jab, shake, tilt, roll, heart rate, and GSR values simultaneously.
- Jab (1D, 2D, 3D): Detects sharp, sudden "jab" motions using accelerometer data on one, two, or three axes.
- Leaky Integrator: A simple utility node for smoothing signals over time.
- Peak Detection: A versatile node to detect peaks in any continuous data stream.
- Power Band: Calculates the amount of energy within a specific frequency band from a Power Spectral Density (PSD) input.
- Roll: Calculates the roll orientation angle from full IMU (9-DOF) sensor data.
- Shake: Measures the intensity of a shaking gesture using accelerometer data.
- Tilt: Calculates the tilt orientation angle from full IMU sensor data.
