<div align="center">

# CS 499 Milestone Two Narrative<br>
## Category One: Software Design & Engineering<br>
## David Deist<br>

</div>

**_1. Briefly describe the artifact. What is it? When was it created?_**<br>
&emsp;&emsp;&emsp;This artifact is a C++ OpenGL program created for my CS 330 (Computational Graphics and Visualization) class. The application renders a 3D still-life scene using modern OpenGL practices, among which include a shader-based pipeline, camera/view controls, textured objects, and lighting. The original version of the artifact was completed during the CS 330 course term as a final project.

**_2. Justify the inclusion of the artifact in your ePortfolio. Why did you select this item? What specific components showcase your skills? How was it improved?_**<br>
&emsp;&emsp;&emsp;I chose to select this artifact because it demonstrates my ability to work within a C++ codebase which uses multiple modules and classes to deliver a working application. From a software engineering perspective, the project shows a decomposition of responsibilities across files (for example, separating the view and camera concerns from scene setup and rendering). It also reflects practical problem-solving and debugging typical of graphics development.<br>
&emsp;&emsp;&emsp;For Milestone Two, I decided to focus on a design-oriented refactor, in order to improve maintainability and also clarity while still preserving the existing functionality and visual output. In particular, I removed redundant projection-calculation logic from both the view preparation flow and also the standardized null-pointer usage by replacing legacy NULL checks with nullptr. These changes help to reduce confusion, eliminate duplicate computations, and also improve general code readability without changing the program’s runtime behavior.

**_3. Did you meet the course outcomes you planned to meet with this enhancement in Module One? Do you have any updates to your outcome-coverage plans?_**<br>
&emsp;&emsp;&emsp;Yes. This enhancement supports the outcomes I had planned to emphasize—particularly Outcome 3 (“Design and evaluate computing solutions while managing trade-offs”) and Outcome 4 (“Use well-founded techniques, skills, and tools to implement solutions that deliver value”). The refactor reflects the evaluation of existing code, identification of a maintainability issue (in this case, redundant logic), and also a targeted improvement that reduces complexity while preserving output. At this time, I do not have any further updates to my outcome-coverage plan for Category One.

**_4. Reflect on the process of enhancing and modifying the artifact. What did you learn? What challenges did you face?_**<br>
&emsp;&emsp;&emsp;Enhancing this artifact reinforced the importance of revisiting working code with a software engineering mindset. Even when a program produces correct output, things like duplicate logic and inconsistent conventions can still make future changes more error-prone. The primary challenge was making improvements that were meaningful for design quality, while also keeping scope controlled and avoiding unintended changes to the rendered scene. This process highlighted how even small, well-justified refactors can help to improve clarity and maintainability, which is consistent with real-world development where software is often improved incrementally (rather than rewritten).
