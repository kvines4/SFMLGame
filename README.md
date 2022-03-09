# **About The Project**

This Game Engine is a basic Entity Component System (ECS) that uses RAII (Resource Acquisition Is Initialization) concepts for memory management and the SFML Graphics Library for Graphics. 
This Game Engine uses data driven design to dynamically load custom configurations, assets, and level data. This means changes can be made to external files to tweak the gameplay at runtime without the need to recompile.

Features of the current iteration include:

## Input/Action System

- The Game Engine handles the input from hardware such as keyboard and mouse then sends 'Actions' to the Current Scene.
- The Current Scene contains a mapping of Actions. These Actions have no knowledge of the method of input, meaning they could be passed in from anywhere.
- Decoupling the Action from the input method will allows us to more easily add functionality later such as replays.

## Multiple scene types

- There is an abstract base Scene class that contains everything common across all scene types.
- Each Scene will contain it's own Entity Manager, Input/Action map and Systems.
- Many additional derived scene classes can be created each with their own functionality. 
- For example:
    - Menu Scene
    - Gameplay Scene
    - Dialogue Scene
    - Shop Scene
    - etc.

## Entity Components

- In an ECS style engine, components are simply containers for data. Each Scene will have its own systems which will use these componets accordingly.
- Current Component types:
    - Transform
    - Lifespan
    - Input
    - BoundingBox
    - Animation
    - Gravity
    - State

## Assets

- At runtime, Assets are loaded into the Game Engine's Assets System.
- These assets are defined in the Assets.txt file and are included in the Bin folder.
- When a Scene is loaded, its contents (Entities) will be read from a 'level.txt' file.
- Each Entity created will be given appropriate components based on its type and will be assigned animations from the Assets System.
- Current Asset types:
    - Textures
    - Animations
    - Fonts

## Collisions

- The current physics system uses Axis-Aligned Bounding Box (AABB) style collision detection.

A Mario/Megaman clone was implemented as a means of testing the concepts mentioned above.

<p align="right">(<a href="#top">back to top</a>)</p>

## Assets File Specification

There will be three different line types in the Assets file, each of which
correspond to a different type of Asset. They are as follows:

### **Texture Asset Specification:**

    Texture N P 

<table class="tg">
<tbody>
  <tr>
    <td>Texture Name</td>
    <td>N</td>
    <td>std::string (it will have no spaces)</td>
  </tr>
  <tr>
    <td>Texture FilePath</td>
    <td>P</td>
    <td>std::string (it will have no spaces)</td>
  </tr>
</tbody>
</table>

### **Animation Asset Specification:**

    Animation N T F S 

<table class="tg">
<tbody>
  <tr>
    <td>Animation Name</td>
    <td>N</td>
    <td>std::string (it will have no spaces)</td>
  </tr>
  <tr>
    <td>Texture Name</td>
    <td>T</td>
    <td>std::string (refers to an existing texture)</td>
  </tr>
  <tr>
    <td>Frame Count</td>
    <td>F</td>
    <td>int (number of frames in the Animation)</td>
  </tr>
  <tr>
    <td>Anim Speed</td>
    <td>S</td>
    <td>int (number of game frames between anim frames)</td>
  </tr>
</tbody>
</table>

### **Font Asset Specification:**

    Font N P

<table class="tg">
<tbody>
  <tr>
    <td>Font Name</td>
    <td>N</td>
    <td>std::string (it will have no spaces)</td>
  </tr>
  <tr>
    <td>Font File Path</td>
    <td>P</td>
    <td>std::string (it will have no spaces)</td>
  </tr>
</tbody>
</table>

<p align="right">(<a href="#top">back to top</a>)</p>

## **SFML Installation**

This engine makes use of SFML which shuld be installed as outlined in the SFML installation instructions found [here](https://www.sfml-dev.org/tutorials/2.5/start-vc.php).

**NOTE:** the SFML path in the project settings is *C:\libraries\SFML-2.5.1* and may need to be updated to your install location.

<p align="right">(<a href="#top">back to top</a>)</p>

## **License**

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>

## **Contact**

Kristian Vines - kvines4@outlook.com

Project Link: [https://github.com/kvines4/SFMLGame](https://github.com/kvines4/SFMLGame)

<p align="right">(<a href="#top">back to top</a>)</p>
