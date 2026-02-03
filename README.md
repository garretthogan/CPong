# CPong - Cross-Platform 3D Pong

A 3D Pong clone that runs on **Windows**, **macOS**, and **Linux** using OpenGL 3.3, GLFW, and GLM.

## Controls

- **W/S** - Left paddle (Player 1)
- **Up/Down arrows** - Right paddle (Player 2)
- **Escape** - Quit

## Building

### Prerequisites

- CMake 3.16+
- C++17 compiler (MSVC, GCC, Clang)
- Git (for fetching dependencies)

### Windows (Visual Studio)

1. Open the project folder in Visual Studio (File → Open → Folder)
2. Select the **x64-debug** or **x64-release** preset
3. Build → Build All (or F7)
4. Run from the project root (so `shaders/` is found), or set working directory to the build output folder

### Windows (Command Line)

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug
.\out\build\x64-debug\CPong.exe
```

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

cmake --preset linux-release
cmake --build out/build/linux-release
./out/build/linux-release/CPong
```

### macOS

```bash
cmake --preset macos-release
cmake --build out/build/macos-release
./out/build/macos-release/CPong
```

## Project Structure

```
CPong/
├── CPong.cpp          # Entry point
├── src/
│   ├── Game.cpp/h     # Game logic
│   ├── Renderer.cpp/h # 3D rendering
│   └── Shader.cpp/h   # GLSL shader loading
├── shaders/
│   ├── vertex.glsl
│   └── fragment.glsl
└── CMakeLists.txt
```

## Dependencies (auto-fetched via CMake FetchContent)

- **GLFW 3.4** - Window and input
- **GLM 1.0.1** - Math library
- **GLAD** - OpenGL function loader (from DawidLokiec/glad)

## License

MIT
