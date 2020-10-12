# Dodgin' Boxes
A simple game using my (bad) Vulkan abstraction layer.

[![Dodgin' Boxes](http://img.youtube.com/vi/SX8QdKs8QtY/0.jpg)](http://www.youtube.com/watch?v=SX8QdKs8QtY "Dodgin' Boxes")

Dependencies:
-Vulkan
-OpenAL
-GLFW
-GLM
-stb


To build:
Install Vulkan SDK and put include files in SimpleGame\Dependencies\IncludeDependencies\vulkan and SimpleGame\Dependencies\IncludeDependencies\shaderc

Install OpenAL and put include files in SimpleGame\Dependencies\IncludeDependencies\OpenAL

Put GLFW include files in SimpleGame\Dependencies\IncludeDependencies\GLFW

Put glm include files in SimpleGame\Dependencies\IncludeDependencies\glm

Put stb include files in SimpleGame\Dependencies\IncludeDependencies\stb

Add libraries to SimpleGame\Dependencies\*DEPENDENCY NAME*\lib and SimpleGame\Dependencies\*DEPENDENCY NAME*\lib32 (Note: glm and stb are header only)

Run compile_shaders.bat in SimpleGame\DodgingBoxes\Resources

After that just open the solution in Visual Studio and build.



# List of effects and how they were achieved:
-Shadows

Omni-directional shadow mapping using cubemaps.


-Reflections

Render scene to cubemap texture centered in the middle of the reflective cube. Then, when choosing which point on the cubemap to sample, reflect view vector and find where that reflected vector intersects the bounds of the room. Use the vector from the center of the cube to that vector to sample the cubemap. This method is still not perfect and when other objects are near the cube they tend to distort, but they are moving fast enough that it is not too noticeable.


-Rough glass

The reflective cube mentioned earlier also has rougher and smoother spots on it. This is achieved by rendering the inside of the cube to a cubemap. The cubemaps for the internals of the cube and the reflections are both mipmapped and the mip-level to sample is chosen based on a roughness texture. (The reflection is actually mixed with the internals of the cube incorrectly, because if I just used the Fresnel as an interpolator like I should, the top would want to reflect the blackness above leading to a black spot on the top of the cube, which I don't like the look of.)


-Fog

After everything other than text and menus has been rendered, a raymarching process is performed to include fog with is lit and also shadowed. (Note: A simplified and linear falloff is being used for the lighting here because it was expensive to compute a complex falloff for each light for each step in the raymarching.)
