<div align="center">
    <h1>R-Type</h1>
    <h3>The remake by Epitech</h3>
</div>

---

## 📋 Project Description

R-Type is a remake of the legendary side-scrolling shooter arcade game, developed as part of an Epitech project.
The goal is to faithfully recreate the classic gameplay experience while building a modular and scalable game engine from scratch.

This project focuses on:

* Implementing an Entity-Component-System (ECS) architecture for flexibility and reusability

* Separating engine logic from game-specific logic to allow future game development beyond R-Type

* Supporting multiplayer gameplay through a dedicated server-client architecture

* Leveraging Raylib for rendering and cross-platform compatibility

## 🛠️ Technologies Used

### Frontend

* **C++** - Development language
* **Raylib** - Graphics library

### Compilation & Build

CMake – Cross-platform build system and project configuration

### Testing

Criterion – Unit testing framework

## 📁 Project Structure

```
R-Type/
├── Client/               # Graphical client
│   ├── Input/            # Input handling
│   ├── Network_client/   # Communication with the server
│
├── Engine/               # Generic engine (independent from R-Type)
│   ├── Core/             # ECS, Event Bus, Time Management
│   ├── Physics/          # Collisions, hitboxes
│   └── Rendering/        # Rendering (Raylib wrapper)
│
├── Game/                 # R-Type game (built on top of the engine)
│   ├── Assets/           # Game assets (sprites, sounds)
│   │   ├── sounds/       # Audio files
│   │   └── sprites/      # Image files
│   ├── Config_assets/    # Config files (players, settings…)
│   ├── Entities/         # Prefabs (Player, Enemy, Bullet…)
│   ├── Scene/            # Levels, menus
│   ├── Systeme/          # Game systems (logic, mechanics)
│
├── Server/               # Authoritative server
│   ├── Game_logic/       # Game rules (spawn, score…)
│   ├── Network_handler/  # Client management
│
├── Shared/               # Shared code (protocol, common structures)
│
├── Unit_test/            # Unit tests
│   └── Engine/           # Engine-related tests
│
├── CMakeLists.txt        # Root build configuration
├── vcpkg/                # Dependencies manager
└── README.md

```

## 📚 Documentation

- 🧠 **[Lucispark Diagrams](https://lucid.app/lucidchart/4633408f-cba8-48c3-a0b8-c60bfb79cc14/edit?viewport_loc=-582%2C-133%2C3079%2C1520%2C0_0&invitationId=inv_87f07e72-7bc2-49f5-ad85-d82292c0f6f7)**  
  Visual representations of system architecture.

## 🔧 Development

### Available Scripts

```bash
# IN DEVELLOPEMENT
```

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