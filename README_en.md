# 🐺 Canino Engine

*Read in [Português](README.md) | [English](README_en.md)*

**Canino Engine** is an incredibly lightweight 3D graphics/game engine built from scratch. It focuses exclusively on **Data-Oriented Design (DOD)** and **Zero-Allocation** memory architectures. This project was conceptualized and rigorously architected under the strong opinions and technical direction of **Hablocher**. The foundational code, algorithms, and low-level engineering were developed through intense collaboration with Advanced AI (Google Agents / LLMs).

---

## 🚀 About the Project
The grand goal of Canino Engine is not to compete with established giants like Unreal or Unity, but rather to act as an extreme and hardcore educational base for C++ engineering. It is aimed at those who want to understand cycle-perfect CPU clocks and how a GPU talks and consumes pointers at the lowest and most native level of the Windows API (Win32 & DirectX).

**Our Sacred Programming Core Values:**
- We do not use/abuse classic Object-Oriented C++ programming (Dense systems full of Virtual Polymorphism and heavy abstractions are rejected).
- No amateur *Garbage Collection*.
- **ZERO `new` OR `malloc` DURING SIMULATION**: Heap memory allocations only occur on Boot. Data structures aggressively devour massive, pre-allocated RAM Buffers up-front, strictly resolving everything in local cache via *Offsets/Pointers* using *Memory Arenas* during framerate iterations!

---

## 🔥 Key Features & Architecture

### 1. Base Mathematical System (SIMD & O(1) Alignment)
The foundation of all our code! We built our own `Mat4` matrix mathematical routines heavily exploiting Intel CPU Intrinsics (128-bit `__m128` Registers). Every Camera calculation (*LookAt, Rotate, Cross Product*) was developed strictly adhering to *Row-Major memory layouts* to dispatch directly to Shaders without any pipeline overhead.

### 2. Advanced 3D Graphics Engine (RHI DirectX 11)
Our current RHI dispatches atomic rendering commands straight to the GPU.
* **Separation of Concerns (CBO Splitting)**: Matrices and Materials use independent Constant Buffers to ensure optimal hardware cycles. `CBOMatrices` (Slot b0) manages pure MVPs while `CBOMaterials` (Slot b1) dictates vector Flags and color settings without truncating GPU memory struct boundaries.
* **Z-Buffer / Viewport**: Built-in rendering pipeline to draw exact Z-Depth.
* **Wavefront Mesh Loading & Texturing**: Natively parses complex geometry meshes via our Custom `OBJ` Parser, instantiating Indexed Vertices directly onto `D3D11_BIND_INDEX_BUFFER` and decoding on-disk Bitmaps (`stb`).

### 3. Solid Friction 3D Mathematical Physics (AABB Collision)
Proprietary physics module `canino::physics`. The physics engine maps the environmental volume of static masses and mathematically solves bounding box interpenetration by subvectorially calculating the O(1) **Minimum Translation Vector (MTV)**. This purist mechanism perfectly recreates the "Wall Sliding" friction algorithm from the classic 1996 Quake engine without the bloated necessity of third-party physics libraries.

### 4. Null-Alloc Configuration Reader (`.ini`)
A highly-performant manual parser written in a super-light C style (`canino::ConfigParser`). It reads `canino.ini` on-the-fly, loads runtime variables, and performs immediate logical branching during `LoadFromFile` routines.

### 5. Multi-Threading Job System & ECS
Asynchronous Task Queues structurally locked by condition variables. ECS-Based standard `std::vector` Entities aren't handled as bloated abstraction classes with memory-consuming property setters. Instead, they are array-allocated contiguously and scattered through the concurrent CPU pipeline via aggressive *Help-Stealing Threads*.

---

## 🎮 The Demo Ecosystem (Sandboxes)

The Engine orchestrates and validates its capabilities through independent sub-projects (Sandboxes) located in the `/demos/` root directory:

1. **FPS Demo (The Colossus)**: The current pinnacle of the project! A purist First-Person Demo.
   - Fully handles AABB Physics in Free-Fall Gravity, using mathematical MTV offsets to actively prevent clipping into structural Pillars.
   - Employs *Quake-Like Look FPS Lock* matrix rotation managed by precision Raw Mouse Deltas (Supports Hot-Parsed `InvertMouse X/Y` flipping directly from the runtime `.ini` file).
   - Computes *Right/WalkForward* Terrain walking via Cross Products and explicitly prints a static *Mesh HUD overlay* on top of the world utilizing an Absolute Projection Matrix independently from the Viewport.
2. **Demo 1 (MULTI-CORE Stress Sandbox)**: Instantly fires `2000 parallel Thread Jobs` computing simultaneous abstract physics logic. During the Render phase, structs burst in purely O(1) complexity, translating virtual positions into 2000 independent `Mat4` matrices painted entirely by the CPU using `RenderCommand::DrawCube()`. 
3. **Demo 2 (Cubic Spin)**: A primitive textured static cube rendering test asserting sound math Perspective Matrix calculations.
4. **Texture Test**: A rigid QA Automotive Unit Test exclusively written to stress-test the DirectX compiler against C++ null pointers and VRAM `_align(16)` structural misallocations!

---

## 💻 How to Build (CMake)

Following our "Keep the Build System clean" doctrine, this project refuses to contaminate your main source branch with temporary local IDE host setups. It only requires **CMake** installed on your workstation alongside the classic Windows MSVC compilers.

1. Git Clone this repository.
2. Open your preferred Shell (Powershell recommended).
3. Generate the native target makefiles via our unified CMake Engine:
   ```shell
   cmake -B build
   ```
4. Compile the core Engine Linker lib and orchestrate all SubDemos running at maximum available C++ optimization powers by pushing the Release -O2 flags (Or use `DEBUG` to retain safety Assertions):
   ```shell
   cmake --build build --config DEBUG
   ```

*(Ensure your Visual Studio / Windows SDK includes the native C++ Desktop Development package).*

---

## 🕹 How To Play / Run

In the name of clean architectures, CMake strictly forbids standalone DLLs or rogue files bleeding over the source code footprint! All compiled binaries are safely bundled inside the target bin directory.

Just launch the ready-to-run Demo Sandboxes directly:

```shell
.\build\bin\DEBUG\FPSDemo.exe
```

**[ 🎮 Controls: FPS Demo ]** 
* **Mouse** aims and turns your Neck. (*You can tweak Axis Inversions inside `canino.ini` and instantly test the runtime parser*)
* **`W, A, S, D`** Smoothly glides and rubs against collision Geometry Terrain at a fixed `proposedVelocity`.
* **`SPACEBAR`**: Ascends applying reversed Y acceleration. You will slowly degrade via `player.velocityY` inertia until you intercept an obstacle below your feet, triggering the engine's O(1) detection algorithm back to a _Grounded_ state.
* Hit **`ESCAPE (ESC)`** at anytime to violently terminate the Multithreading Copter ECS, Purge the Native DirectX context (0-bytes Leaked), and gracefully kill the Windows32 shell window using the clean OS API.
