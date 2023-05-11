# OpenGL-SDL-2D_Sphere
A small project to create a fake sphere (using shaders) from a circle! (circle polygons are created in the program -> count variable)

On windows you can compile the project with .\build.bat. You need sdl2, glew, gl and cglm installed! </br>
With pressing '+' you can increase the count of polygons, with '-' you can decrease the polygon count. </br>

The Polgons are created with a cos function in the circle equation multiplied with the radius: </br>
y = sqrt(-(cos(β) * r) + r^2) or for the lower part: y = (-1) * sqrt(-(cos(β) * r) + r^2) </br>

The Cos-function actually results by calculating the beta-angle in the circle: </br>
