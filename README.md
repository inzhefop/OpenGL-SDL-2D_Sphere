# OpenGL-SDL-2D_Sphere
A small project to create a fake sphere (using shaders) from a circle! (circle polygons are created in the program -> count variable)

On windows you can compile the project with .\build.bat. You need sdl2, glew, gl and cglm installed! </br>
With pressing '+' you can increase the count of polygons, with '-' you can decrease the polygon count. </br>

The Polgons are created with a cos function in the circle equation multiplied with the radius: </br>
y = sqrt(-(cos(β) * r) + r^2) or for the lower part: y = (-1) * sqrt(-(cos(β) * r) + r^2) </br>

The Cos-function actually results by calculating the beta-angle in the circle: </br>
Pythagoras: A^2 + G^2 = H^2 -> G = sqrt(-x^2+r^2); H = sqrt((1-(r-x))^2+(-x^2+r^2) </br>
-> cos(β) = sqrt(-x^2+r^2) / sqrt((1-(r-x))^2+(-x^2+r^2) </br>
<=> cos^2(β) = (2x-2xr+x^2+1-2r+r^2) / (-2xr+2x+1-2r+2r^2) </br>

for r=1 -> cos^2(β) = (2x-2x+x^2+1-2+1) / (-2x+2x+1-2+2) = (2x-2x+x^2) / (-2x + 2x + 1) </br>
<=> cos^2(β) = (x*(2-2+x)) / (x*(-2+2+1/x)) = x^2 / 1 = x^2 | sqrt </br>
<=> cos(β) = x </br>

With more polygons the β angle gets lower: β = 2π / polygons. While creating the polygons you cycle trough the angles while adding angle + β until the angle is equal or greater than 2π. </br>

The 3D light effect is created in the fragment shader, where I interpolated the 2D plane to a 3D sphere using the sphere function. This equation is used to create the normal vectors from the middle of the sphere: </br>
x^2 + y^2 + z^2 = r^2 <=> sqrt(-pos.x^2 - pos.y^2 + radius^2) = z </br>

After passing a uniform light-source vector, the angle between those two vectors is calculated and normalized to a value between 0 and 1 and used for the brightness of this fragment. For realism this brightness-value is potntiated with 4 (or 3) and then passed as the value for red, green and blue
