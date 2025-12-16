# ESP32 LovyanGFX Flocking Simulation

A real-time **boids / flocking simulation** running on an **800x480 [8048S070 RGB panel](https://esp3d.io/ESP3D-TFT/Version_1.X/hardware/esp32-s3/sunton-70-8048/)** with a touch UI, rendered using **LovyanGFX**.  
The simulation is interactive: you can add boids by touching the screen and tune the flocking parameters live via sliders.

![screenshot](https://github.com/user-attachments/assets/23f05aa0-85c7-4808-9390-8074222d4059)

This project is inspired by and conceptually based on the **Processing Flocking example**:

https://github.com/processing/processing-examples/tree/main/Topics/Simulate/Flocking

## What is Flocking?

Flocking is an emergent behavior model [introduced by Craig Reynolds in 1987](https://en.wikipedia.org/wiki/Flocking).  
It simulates how birds, fish, or other agents move collectively without central control.

Each agent (“boid”) follows three simple local rules:

1. **Separation** – avoid crowding nearby neighbors  
2. **Alignment** – steer toward the average heading of neighbors  
3. **Cohesion** – steer toward the average position of neighbors  

Despite the simplicity of these rules, the group exhibits complex, lifelike motion.

---

## Features

- Real-time flocking simulation
- Touch-based interaction
  - Tap simulation area to add boids
  - Sliders to tune separation, alignment, and cohesion weights
  - Buttons to clear or reset the simulation

---

## Hardware

Tested on:
- **ESP32-S3** on a 800×480 [8048S070 RGB panel](https://esp3d.io/ESP3D-TFT/Version_1.X/hardware/esp32-s3/sunton-70-8048/)
- Capacitive touch

---

## Software Stack

- PlatformIO
- Arduino framework (ESP32)
- LovyanGFX

---

## Controls

| Action | Description |
|------|-------------|
| Touch simulation area | Add a boid |
| Separation slider | Adjust personal space force |
| Alignment slider | Adjust heading matching |
| Cohesion slider | Adjust group attraction |
| Clear Boids | Remove all boids |
| Reset Simulation | Reset to initial state |

---

## Attribution

This project is inspired by the Processing example:

> *Flocking*  
> https://github.com/processing/processing-examples/tree/main/Topics/Simulate/Flocking  

The original idea and behavior model come from:
- Craig Reynolds — *“Flocks, Herds, and Schools: A Distributed Behavioral Model”*

---

## License

[MIT](LICENSE)
