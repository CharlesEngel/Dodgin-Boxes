# Dodgin' Boxes
A simple game using my (bad) Vulkan abstraction layer.


Video:

[![Dodgin' Boxes](http://img.youtube.com/vi/SX8QdKs8QtY/0.jpg)](http://www.youtube.com/watch?v=SX8QdKs8QtY "Dodgin' Boxes")

# List of effects and how they were achieved:
-Shadows

Omni-directional shadow mapping using cubemaps.


-Reflections

Render scene to cubemap texture centered in the middle of the reflective cube. Then, when choosing which point on the cubemap to sample, reflect view vector and find where that reflected vector intersects the bounds of the room. Use the vector from the center of the cube to that point to sample the cubemap. This method is still not perfect and when other objects are near the cube they tend to distort, but they are moving fast enough that it is not too noticeable.


-Rough glass

The reflective cube mentioned earlier also has rougher and smoother spots on it. This is achieved by rendering the inside of the cube to a cubemap. The cubemaps for the internals of the cube and the reflections are both mipmapped and the mip-level to sample is chosen based on a roughness texture. (The reflection is actually mixed with the internals of the cube incorrectly, because if I just used the Fresnel as an interpolator like I should, the top would want to reflect the blackness above leading to a black spot on the top of the cube, which I don't like the look of.)


-Fog

After everything other than text and menus has been rendered, a raymarching process is performed to include fog which is lit and shadowed. (Note: A simplified and linear falloff is being used for the lighting here because it was expensive to compute a more complicated falloff for each light for each step in the raymarching.)


# Dependencies:
-Vulkan

-OpenAL

-GLFW

-GLM

-stb



# To build:
Clone repository.

Make sure CMake, git, OpenAl, and the Vulkan SDK are all installed.



If on Windows, change the paths in src/Resources/compile_shaders.bat to point to glslc then run it.

If on Linux, run src/Resources/compile_shaders.sh.



From the Dodgin-Boxes directory, call:

>mkdir build

>cd build

>cmake ..



If on Windows, open the created Visual Studio project and build.

If on Linux, run make.
