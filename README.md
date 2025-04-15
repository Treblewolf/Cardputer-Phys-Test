# 🕹️ M5Cardputer 2D Physics Engine

A lightweight and modular 2D physics engine with sprite rendering for the **M5Stack Cardputer**. This project features real-time gravity simulation, AABB (Axis-Aligned Bounding Box) collision detection, and basic keyboard-controlled movement using the Cardputer’s built-in keyboard.

## 🚀 Features

- Gravity simulation for all objects  
- AABB collision detection & resolution  
- Sprite rendering with `M5Canvas`  
- Lightweight
- Easily extendable for simple 2D games or physics demos

## 📷 Preview

![example](https://github.com/user-attachments/assets/71ad773b-3dfb-4e88-b2ef-ec4ca93eda6e)

*Demo of two dynamic sprites colliding with gravity and keyboard movement*

## 🧰 Hardware Requirements

- [M5Stack Cardputer](https://shop.m5stack.com/products/cardputer-kit?ref=chatgpt)  
- USB-C Cable  
- Arduino IDE with M5Stack libraries installed  

## 📦 Dependencies

Install these libraries via the Arduino Library Manager:

- `M5Unified`
- `M5GFX`
- `M5Cardputer`

## How It Works

The engine uses a basic update-render loop running at ~70FPS. Objects are represented as simple structures containing position, velocity, and dimensions.

Collision is resolved with axis separation logic (AABB), and gravity is applied as a constant force each frame.
