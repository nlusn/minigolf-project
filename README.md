# Mini Golf with a Windmill

A 3D mini-golf game written in C++ with OpenGL 3.3 core profile. There are 3 holes. Two of them have a brick windmill whose blades turn in front of a tunnel and block the ball, so you have to time your shot.

## Build and run

You need a C++17 compiler, CMake 3.16+, GLFW 3 and GLM. If GLFW or GLM are not installed, CMake downloads them. GLAD is already in `external/glad`.

Linux:
```bash
sudo apt install build-essential cmake libglfw3-dev libglm-dev
cmake -S . -B build
cmake --build build
./build/minigolf
```

Windows (Visual Studio):
```bash
cmake -S . -B build
cmake --build build --config Release
build\Release\minigolf.exe
```

Run the program from the project folder or the folder that contains the executable. It loads the shaders from `shaders/`, which CMake copies next to the executable.

## Controls

| Key | Action |
|---|---|
| Left / Right arrows | Aim (hold Left Shift for slow aiming) |
| Space (hold, then release) | Choose the power and shoot |
| Right mouse button + move | Rotate the camera |
| A / D, Up / Down | Rotate the camera |
| Mouse wheel, W / S | Zoom |
| L | Lamp and lantern lights on / off |
| N | Day / night |
| P | Pause the windmill |
| + / - | Windmill faster / slower |
| 1 / 2 / 3 | Jump to a hole |
| R | Restart from hole 1 |
| H | Show / hide the help |
| Esc | Quit |

## Features

**Topic requirements (17. Mini Golf Hole)**
- A flat course with a textured grass floor and straight wooden walls.
- The ball rolls with friction and bounces off the walls, with the velocity reflected about the wall normal.
- Aim with the arrow keys; hold Space for power.
- Hole detection: the ball falls into the cup when it is over it and slow enough.
- A windmill with rotating blades, drawn as a hierarchy of transformations.

**Stretch goals**
- The windmill blades block the ball.
- 3 holes: The Windmill (par 2), The Dogleg (L-shape with two blocks, par 3) and The Gauntlet (fast windmill, posts and a gate, par 3).
- A stroke counter, the result after each hole (hole in one, birdie, par, ...) and a win screen with the scorecard of all 3 holes.

**Wow moment:** a hole-in-one through the spinning windmill. On hole 1, shoot straight at about 70-80% power when the tunnel is free.

**Course requirements**

| | How it is done |
|---|---|
| R1 | OpenGL 3.3 core, VAO/VBO/EBO for the meshes, our shaders in `shaders/` |
| R2 | Objects: green, field, walls, windmill (base, tower, roof, hub, 4 blades), ball, cup, flag, lamp post, lantern |
| R3 | Model, view and projection matrices with GLM. Hierarchies: windmill → rotor → blades → spar and sail, and pole → flag (`Scene.cpp`) |
| R4 | Blinn-Phong per fragment (`basic.frag`): ambient, diffuse and specular. 3 lights: the sun, the lamp post and a lantern that flies around the course. L turns the lamp and lantern off |
| R5 | Grass, wood and brick textures made in code, with mipmaps and repeating UVs (`Texture.cpp`) |
| R6 | Orbit camera with mouse and keyboard. Interactions: aim and shoot, lights on/off, day/night, windmill pause/speed, hole select, restart |
| R7 | FPS on screen and in the window title |
| R8 | CMake, build steps above |

## Code structure

| File | What it does |
|---|---|
| `main.cpp` | Window, input, game rules, HUD layout, main loop |
| `Shader.*` | Loads and compiles shaders, sets uniforms |
| `Mesh.*` | Uploads meshes to the GPU; box, sphere, cylinder, plane and disc shapes |
| `Texture.*` | Creates the grass, wood and brick textures |
| `Camera.*` | Orbit camera |
| `Course.*` | The `Level` struct and the layouts of the 3 holes |
| `Ball.*` | Ball movement, friction, wall bounces |
| `Windmill.*` | Blade rotation, base walls, blade collision |
| `Scene.*` | Builds the meshes of the current hole and draws all 3D objects |
| `Hud.*`, `Font.h` | 2D text and rectangles on the screen |

## Team

| Member | Parts of the project |
|---|---|
| Sagi Alpyssov | Window and OpenGL setup, shaders, meshes, camera, documentation |
| Anatoliy Tsoy | Course scene, lighting, textures, ball physics, aiming and shooting |
| Nargiza Kuzybahova | Hole detection and flag, windmill model and collision, 3 holes, HUD, win screen |
