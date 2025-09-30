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

### Frontend

* **C++** - Development language
* **Raylib** - Graphical librairie

### Backend

* **Asio** - network librairie


## 📁 Project Structure

```
r-type/
├── engine/               # Generic engine (independent from R-Type)
│   ├── core/             # Core (ECS, Event Bus, Time Management)
│   ├── rendering/        # Rendering (Raylib wrapper)
│   ├── physics/          # Collisions, hitboxes
│   └── utils/            # Math, logging, serialization
│
├── game/                 # R-Type game (built on top of the engine)
│   ├── entities/         # Prefabs (Player, Enemy, Bullet...)
│   ├── systems/          # Game logic (MovementSystem, CombatSystem...)
│   ├── scenes/           # Levels, menus
│   └── assets/           # Sprites, sounds
│
├── server/               # Authoritative server
│   ├── game_logic/       # Game rules (spawn, score...)
│   └── network_handler/  # Client management
│
├── client/               # Graphical client
│   ├── input/            # Input handling
│   └── network_client/   # Communication with the server
│
├── shared/               # Shared code (protocol, common structures)
│
└── tests/                # Unit tests (catch2, doctest)

```

## 🔧 Development

### Available Scripts

#### 1.Install dependencies
vcpkg:

```bash
sudo dnf install ninja-build

sudo dnf install libXinerama-devel libXcursor-devel libXrandr-devel libXi-devel mesa-libGL-devel pkg-config

git submodule update --init --recursive
```

#### 2.Build the project
```bash
mkdir build
cd build
cmake ..
make
```

#### 3.Launch the project
3.1 server:

```bash
./rtype_server
```

3.2 client:

```bash
./rtype_client [IP ADRESS] [NAME]
```
📝 Example:
```bash
./rtype_client 127.0.0.1 Paco
```

## 📚 Documentation

- 🧠 **[Lucispark Diagrams](https://lucid.app/lucidchart/4633408f-cba8-48c3-a0b8-c60bfb79cc14/edit?viewport_loc=-582%2C-133%2C3079%2C1520%2C0_0&invitationId=inv_87f07e72-7bc2-49f5-ad85-d82292c0f6f7)**  
Visual representations of system architecture.

- 🖥️ **[Mirror Repository](https://github.com/Anthoche/R-type)**  
  Repository to run CI.

## 👥 Team

- [Clément-Alexis Fournier](https://github.com/Clement-Alexis)
- [Bastien Leroux](https://github.com/bast0u)
- [Anthony El-Achkar](https://github.com/Anthoche)
- [Mariia Semenchenko](https://github.com/mariiasemenchenko)
- [Corto Morrow](https://github.com/NuggetReckt)


*Last update: September 2025*

---

<div align="center">
    <sub>{Epitech.} - 2025</sub>
</div>