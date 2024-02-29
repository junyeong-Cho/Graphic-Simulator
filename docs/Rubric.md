
## Submission

You will be submitting your work on GitHub with the provided class assignment repository. You will also create a copy of your project as a 7z file on the course site. 

The name of the 7z file must follow this naming convention: `[class_name]_[login]_project.7z`

For example, if a student had the login name `fun.frank` then the appropriate filename would be `cs250_fun.frank_project.7z`. Note: The .7z file names are case-sensitive and must be in all lowercase, exactly as described above.

In addition to the project development files, add pre-built binary files so that we can quickly and easily see your work running.

Note that we will be grading from the GitHub repo and we are using the 7z copy as a way to backup your work.

## Rubric

- [ ] [**core**] All topics are implemented by student
- [ ] [**core**] Demo has a scene that demonstrates **Procedural Geometric Modeling**
- [ ] [**core**] Demo has a scene that demonstrates **Fog**
- [ ] [**core**] Demo has a scene that demonstrates **Toon / Cell Shading**
- [ ] [**core**] Demo has a scene that demonstrates **Shadow Mapping**
- [ ] [**core**] Demo has a scene that demonstrates **Geometry and Tessellation Shaders**
- [ ] [**core**] Demo has a scene that demonstrates **Compute Shaders**
- [ ] [**core**] Demo has a scene that demonstrates **Value noise and Gradient noise / Perlin noise**
- [ ] [**core**] Demo has a scene that demonstrates **Hermite Curves and Catmull-Rom Splines**
- [ ] [**core**] Demo is implemented with OpenGL
- [ ] Demo has Windows Version and Web Version that builds and runs
- [ ] All source files compile without warnings and without errors.
- [ ] Correct files submitted. No unnecessary files submitted.
- [ ] A 7z archive of the project is created and submitted on the course site
- [ ] 7z archive includes pre-built binaries that can be run right away
- [ ] All source files have a proper header comment with your name, assignment name/number, course name, term.
- [ ] Followed the appropriate Code Quality Guidelines and Code Style Guidelines.


Scores for Assignments will be given as the following:

Score        | Assessment
------------ | ----------
F | Nothing turned in at all
D | Close to meeting core requirements
C | Meets all of the core requirements
B | Close to meeting all requirements
A | Clearly meets all requirements 
S | High quality, well beyond the requirements

### Suggestions to earn extra credit

#### Add a new topic

Study and create a Demo that wasn't discussed in class.

If you are aiming for extra credit then make sure to discuss the topic with the Instructor to make sure it is a good fit.


#### Create an Interactive Experience

To help show off your understanding of the topics, create another application that is an interactive experience utilizing all of the topics.

You can think of it like a game prototype where the prototype goal is to show off the graphics technology.

We suggest you create something like [The Aviator](https://tympanus.net/codrops/2016/04/26/the-aviator-animating-basic-3d-scene-threejs/). This has very minimal depth in terms of interactivity. All you do is move the mouse around to move the plane around a bit. That would be enough of an experience. Going all the way to making it a game where you earn points and avoid obstacles is not required. Even though you would just be moving the plane around, it has enough going on that you could easily use all the topics in the course to make it. 

You could use Toon Shading with Fog for the lighting. Geometry shaders to generate outlines. Procedural modeling to create all the geometry in the scene like clouds, the plane, the pilot, etc... You could use Value noise and Catmull-Rom Splines to drive animations. You could use Gradient noise to generate the ground terrain. You could use shadow mapping to cast shadows on the ground. You could add grass to the terrain with tessellation shaders.

Of course your group doesn't need to recreate The Aviator. You are welcome to create another kind of 3D experience, just so long as you keep the interactivity simple and utilize the graphics topics.
