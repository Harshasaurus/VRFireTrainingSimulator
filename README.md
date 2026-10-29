# 🧯 VR Fire Training Simulator

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.4-313131?logo=unrealengine&logoColor=white)](https://www.unrealengine.com/)
[![Platforms](https://img.shields.io/badge/Platforms-Win64%20%7C%20Android%20(Meta%20Quest)-blue.svg)]()
[![Standard](https://img.shields.io/badge/Standard-OpenXR%20%7C%20OculusXR-orange.svg)]()
[![Git LFS](https://img.shields.io/badge/Assets-Git%20LFS-brightgreen.svg)](https://git-lfs.com/)

An immersive Virtual Reality safety training simulator developed in **Unreal Engine 5.4**. The simulator prepares users for workplace fire emergencies by combining hands-on fire suppression training using the **P.A.S.S. technique**, emergency alarm activation, NPC evacuation procedures, and real-time performance evaluation.

---

## 🌟 Key Features

### 🧯 Realistic Fire Extinguisher Mechanics (P.A.S.S.)
- **Pull the Pin**: Trainees must grab and pull out the safety pin before discharge is permitted.
- **Aim at the Base**: Advanced nozzle and head-on pitch aiming calculations to evaluate whether the trainee is directing suppressant at the fire's source.
- **Squeeze the Trigger**: Continuous or burst discharge with dynamic pressure, particle spray, and haptic feedback.
- **Sweep Side-to-Side**: Sweep detection logic rewarding trainees for proper sweeping motions rather than static spraying.

### 🔥 Dynamic Fire Simulation
- Dynamic fire actors featuring health decay, spreading behavior, and interactive responses to extinguisher agent.
- Realistic Niagara visual effects, audio cues, and light propagation.

### 🚨 Emergency Alarm & Facility Evacuation
- Interactable emergency wall buzzer/alarm stations that alert the building.
- **Worker NPC AI**: Non-player characters react to alarm triggers, initiate evacuation procedures, and follow escape paths.

### 📊 In-VR Training HUD & Scoring System
- **Real-Time Performance Tracking**: Live timer and instruction guidance instructing users step-by-step.
- **Comprehensive Score Breakdown**: Evaluates reaction time, accuracy, proper P.A.S.S. sequence execution, and safety efficiency.
- **3D Spatial VR UI**: Fully interactive 3D menus with laser pointer navigation, scenario restarts, and difficulty tuning.

---

## 🏗️ Architecture & Modules

The project is structured into modular plugins and C++ gameplay systems:

```
VRFireTraining/
├── Source/
│   └── VRFireTraining/               # Core project module
│       ├── VRInstructionSystem        # Step-by-step safety guidance logic
│       ├── VRScoreWidget              # Trainee score calculation and display
│       ├── VRTimerWidget              # Real-time training stopwatch
│       └── VRWorkerNPC                # Evacuating civilian/worker AI
├── Plugins/
│   ├── VRFireSimulationToolkit/       # Fire and equipment simulation plugin
│   │   ├── VRFire                     # Fire propagation & health state
│   │   ├── VRFireExtinguisher         # Extinguisher handling, pin pull, & discharge
│   │   ├── VRSimulationManager        # Scenario state machine & PASS grading
│   │   ├── VRBuzzer                   # Wall-mounted emergency alarm
│   │   ├── VRMenuManager / Widget     # In-VR spatial menus & laser interaction
│   │   └── VRAimWidget                # Aim assist and nozzle pitch guidance
│   └── VRInteractionToolkit/          # VR interaction subsystem
│       ├── VRGrabbable                # Base class for grab-enabled objects
│       └── VRGrabComponent            # Hand tracking & motion controller grab handling
├── Content/                           # Blueprints, maps, Niagara FX, audio, & 3D models
└── Config/                            # Engine, Input, and OpenXR configurations
```

---

## 🎮 VR Controls & Keybindings

| Action | Primary / Secondary Controller |
| :--- | :--- |
| **Locomotion / Movement** | Thumbstick (Smooth locomotion / Snap turn) |
| **Grab Object (Extinguisher / Pin)** | Grip Trigger (`IA_Grab_Left` / `IA_Grab_Right`) |
| **Pull Safety Pin** | Grip / Trigger on Safety Pin (`IA_PullPin`) |
| **Discharge Extinguisher** | Index Trigger (`IA_Shoot_Right` / `IA_Shoot_Left`) |
| **Trigger Fire Alarm Buzzer** | Hand collision or Trigger press (`IA_BuzzerTrigger`) |
| **Menu Toggle & Selection** | Menu Button + Index Trigger (`IA_Menu_Interact`) |

---

## 💻 Hardware & Software Requirements

- **Engine Version**: Unreal Engine 5.4 or higher
- **Target Hardware**:
  - Meta Quest 2 / Quest 3 / Quest Pro (Oculus Link / AirLink / Standalone Android APK)
  - OpenXR-compliant PCVR headsets (Valve Index, HTC Vive, WMR)
- **Development Environment**:
  - Windows 10/11 (64-bit)
  - Visual Studio 2022 (with *Desktop Development with C++* and *Game Development with C++*)
  - [Git LFS](https://git-lfs.com/) (Required for downloading 3D assets and binary files)

---

## 🚀 Getting Started

### 1. Clone the Repository
Because this project uses **Git LFS** for `.uasset`, `.umap`, and mesh files, ensure Git LFS is installed prior to cloning:

```bash
# Install Git LFS
git lfs install

# Clone the repository
git clone https://github.com/Harshasaurus/VRFireTrainingSimulator.git
cd VRFireTrainingSimulator

# Pull LFS binaries (if not downloaded automatically)
git lfs pull
```

### 2. Generate Project Files
1. Right-click `VRFireTraining.uproject`.
2. Select **Generate Visual Studio project files**.
3. Open `VRFireTraining.sln` in **Visual Studio 2022**.
4. Set the build configuration to **Development Editor** and platform to **Win64**.
5. Build the solution (`Ctrl + Shift + B`).

### 3. Launch in Unreal Engine
1. Launch the project via `VRFireTraining.uproject` or run from Visual Studio.
2. In the Unreal Editor, open the main training map:
   `Content/VRTemplate/Maps/VRTemplateMap.umap`
3. Connect your VR headset via Link/AirLink or OpenXR runtime.
4. Click the **Play** dropdown and select **VR Preview**.

---

## 📄 License

This project is developed for educational and training purposes. Check individual plugin and asset licenses for proprietary or template assets.
