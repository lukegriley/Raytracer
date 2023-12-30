| Example Outputs |
| :--------------: |
| ![Point Light 1](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/point_light/point_light_1.png) |
| ![Point Light 2](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/point_light/point_light_2.png) |
| ![Spot Light 1](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/spot_light/spot_light_1.png) |
| ![Spot Light 2](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/spot_light/spot_light_2.png) |
| ![Simple Shadow](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/shadow/simple_shadow.png) |
| ![Shadow Test](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/shadow/shadow_test.png) |
| ![Shadow Special Case](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/shadow/shadow_special_case.png) |
| ![Reflections Basic](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/reflection/reflections_basic.png) |
| ![Reflections Complex](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/reflection/reflections_complex.png) |
| ![Texture Cone](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/texture_tests/texture_cone.png) |
| ![Texture Cone2](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/texture_tests/texture_cone2.png) |
| ![Texture Cube](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/texture_tests/texture_cube.png) |
| ![Texture Cube2](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/texture_tests/texture_cube2.png) |
| ![Texture Cylinder](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/texture_tests/texture_cyl.png) |
| ![Texture Cylinder2](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/texture_tests/texture_cyl2.png) |
| ![Texture Sphere](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/texture_tests/texture_sphere.png) |
| ![Texture Sphere2](https://raw.githubusercontent.com/BrownCSCI1230/scenefiles/main/illuminate/required_outputs/texture_tests/texture_sphere2.png) |

My Raytracer calculates intersections and object normals by creating an object of the Intersection class associated with a given pixel's P and d values in object space. The main function of this object, calculateIntersection(), uses a switch statement to call intersection calculation functions based on the primitive type. These functions themselves have helper functions, which calculate possible T values for each face. The cylinder intersection function, for example, calls a function for intersection with a cylindrical surface, along with two calls to flat base intersection function (at 2 different Y levels). This allows me to reuse code for certain faces across multiple primitives. Once each faces' T values have been calculated, the primitive intersection function picks the smallest positive T value, if any, and returns. Separating my implicit equations by face also allows me to keep track of which face was hit by the ray, which I allows me to quickly calculate object normals. Because of this, my calculateObjectNormal() function follows a similar philosophy of face-based helper functions.

While looping through the primitives in my scene, I keep track of the shape with the closest T value. I save this shape's intersection object in closestI and primitive object in closestShape. These are both used to calculate lighting. After looping through, I calculate the object's normal in world space using the inverse transpose of the shape's CTM, as discussed in lecture. I calculate the direction to the camera using's the object's and the camera's positions in world space. I pass all the necessary data to a phong() function in a separate light.cpp class. My current version only implements directional light. I loop through all lights, check if any are directional, and implement phong illumination as I did in Lab 6.

