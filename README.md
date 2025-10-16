<div align="center">
    <h1>R-Type</h1>
    <h3>The remake by Epitech</h3>
</div>

---

## 📋 Project Description

This project is a **multiplayer recreation of the classic R-Type game** in C++, built with a **client-server architecture** and a **custom Entity-Component-System (ECS) engine**.  
The goal is to reproduce a **retro shoot’em up** with smooth gameplay, precise collision handling, and robust network management.  

The project is divided into several modules:  
- **Client**: Handles rendering, player input, and synchronization with the server.  
- **Server**: Centralized game engine, responsible for world state, collisions, entities, and overall logic.  
- **ECS Engine**: Custom implementation of the Entity-Component-System model, ensuring a clear separation between data (components) and logic (systems).  

## Key Features  

- 🎮 **Multiplayer**: Supports multiple players connected via UDP.  
- 👾 **Classic gameplay**: Players, enemies, obstacles, and projectiles with full collision detection.  
- 🧩 **ECS system**: Modular entities, easily extendable (new enemy types, power-ups, etc.).  
- 🎨 **Rendering**: Sprites + debug rectangles (optional visible hitboxes).  
- 🛠️ **Authoritative server**: All collisions and game logic are validated server-side to prevent cheating.  
- 🔊 **Audio & effects**: Integration of sound effects and music (optional depending on configuration).  

## 🛠️ Technologies Used

### Language & Libs

* **C++** - Development language
* **Raylib** - Graphical library
* **Asio** - Network library

### Compilation & Build

* **CMake** – Cross-platform build system and project configuration
* **VCPKG** - Cross-platform package manager for C++

### Testing

* **GoogleTest** – Unit testing framework

## 📁 Project Structure

```
r-type/
├── engine/               # Generic engine (independent from R-Type)
│   |── assets/           # Sprites, sounds
│   ├── core/             # Core (ECS, Event Bus, Time Management)
|   |     └─entities/     # Prefabs (Player, Enemy, Bullet...)
│   ├── rendering/        # Rendering (Raylib wrapper)
│   ├── physics/          # Collisions, hitboxes
│   └── scenes/           # Levels, menus
│
├── server/               # Authoritative server
│   ├── game_logic/       # Game rules (spawn, score...)
│   └── network_handler/  # Client management
│
├── client/               # Graphical client
│   ├── Hander/            # Input handling
│   └── network_client/   # Communication with the server
│
├── shared/               # Shared code (protocol, common structures)
│
└── Unit_test/                # Unit tests (catch2, doctest)

```

## 🔧 Development

### Available Scripts

#### 1. Install dependencies
vcpkg:

```bash
sudo dnf install ninja-build

sudo dnf install libXinerama-devel libXcursor-devel libXrandr-devel libXi-devel mesa-libGL-devel pkg-config

git submodule update --init --recursive
```

#### 2. Build the project
```bash
mkdir build
cd build
cmake ..
make
```

### 2.1. Build Unit Tests of the project
``` bash
make tu
```

#### 3. Launch the project
3.1 server:

```bash
./rtype_server [PORT]
```

3.2 client:

```bash
./rtype_client [IP ADRESS] [PORT] [NAME]
```
📝 Example:
```bash
./rtype_server 4242
./rtype_client 127.0.0.1 4242 Paco
```

## 🧰 Troubleshooting

### Common issues
#### 1. Missing OpenGL or X11 dependencies

Error: cannot find -lGL or X11/Xlib.h: No such file or directory
➡️ Install missing dependencies:<br>
```sudo apt install libx11-dev libgl1-mesa-dev```

#### 2. Raylib linking errors

Ensure Raylib is properly fetched via vcpkg or submodules.
You can also install it manually:<br>
```sudo apt install libraylib-dev```

#### 3. Undefined references during linking

Clean your build folder and recompile:<br>
```rm -rf build && mkdir build && cd build && cmake .. && make```

#### 4. Client window doesn’t open

Check that you’re not running in a headless environment (e.g., WSL without GUI).
You can use:<br>
```xeyes or glxinfo | grep OpenGL ```<br>
to verify X11/OpenGL works.

#### 5. I can't install the dependencies

if you're running on ubuntu try:

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build pkg-config git \
    libxinerama-dev libxcursor-dev libxrandr-dev libxi-dev mesa-common-dev libgl1-mesa-dev
git submodule update --init --recursive
```

## 📚 Documentation

- 🧠 **[Lucispark Diagrams](https://lucid.app/lucidchart/4633408f-cba8-48c3-a0b8-c60bfb79cc14/edit?viewport_loc=-582%2C-133%2C3079%2C1520%2C0_0&invitationId=inv_87f07e72-7bc2-49f5-ad85-d82292c0f6f7)**<br>
Visual representations of system architecture.

- 📕 **[Wiki](https://github.com/Anthoche/R-type/wiki)**<br>
Simple wiki to explain some technical details of the project.

- 🖥️ **[Mirror Repository](https://github.com/Anthoche/R-type)**<br>
Repository to run CI.

## 👥 Team

- [Clément-Alexis Fournier](https://github.com/Clement-Alexis)
- [Bastien Leroux](https://github.com/bast0u)
- [Anthony El-Achkar](https://github.com/Anthoche)
- [Mariia Semenchenko](https://github.com/mariiasemenchenko)
- [Corto Morrow](https://github.com/NuggetReckt)

<br>
*Last update: October 2025*

---

📄 License

This project is licensed under the GNU General Public License v3.0 (GPLv3) 

📜 License Summary

GNU GENERAL PUBLIC LICENSE  
Version 3, 29 June 2007

Copyright (C) 2025 The R-Type Team

This program is free software: you can redistribute it and/or modify  it under the terms of the GNU General Public License as published by  
the Free Software Foundation, either version 3 of the License, or  (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of  
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.


<div align="center"> <sub>{Epitech.} - 2025</sub> </div>
