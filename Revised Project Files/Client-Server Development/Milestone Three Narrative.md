<div align="center">

# CS 499 – Milestone Three Narrative<br>
## Category Two: Algorithms & Data Structures<br>
## David Deist

</div>

**_1. Briefly describe the artifact. What is it? When was it created?_**<br>
&emsp;&emsp;&emsp;This artifact is a Python-based interactive dashboard originally created for CS 340 (Client-Server Development). The application connects to a MongoDB database containing animal shelter records and allows users to apply predefined filters, view the results in a data table, and visualize information using a pie chart and a map. The original artifact was developed as a course project to demonstrate database interaction, user-driven filtering, and basic data visualization within a single application.

**_2. Justify the inclusion of the artifact in your ePortfolio. What specific components showcase your skills and abilities in algorithms and data structure? How was the artifact improved?_**<br>
&emsp;&emsp;&emsp;I selected this artifact because it demonstrates the practical use of data structures and algorithmic reasoning within an application that repeatedly processes user-driven queries. In its original form, each filter selection triggered a new database read, even when the same query had been used previously. For this enhancement, I introduced an in-memory least-recently-used (LRU) cache implemented with an ordered dictionary structure. Queries are normalized into stable keys and stored with their corresponding results, allowing repeated requests to be resolved in constant average time rather than requiring repeated database access. This improvement reduces redundant computation and demonstrates thoughtful data structure selection to optimize performance while preserving the original behavior and output of the dashboard.

**_3. Did you meet the course outcomes you planned to meet with this enhancement in Module One? Do you have any updates to your outcome-coverage plans?_**<br>
&emsp;&emsp;&emsp;Yes, this enhancement meets the planned outcomes by demonstrating Outcome 3, which emphasizes designing and evaluating computing solutions using algorithmic principles while managing trade-offs. The caching mechanism improves time efficiency at the cost of limited additional memory usage, reflecting an intentional design decision. It also supports Outcome 4 by applying a well-founded and industry-relevant technique to deliver measurable value through improved responsiveness and scalability. At this time, I do not have updates to my outcome-coverage plans for the algorithms and data structures category.

**_4. Reflect on the process of enhancing and modifying the artifact. What did you learn as you were creating it and improving it? What challenges did you face?_**<br>
&emsp;&emsp;&emsp;Enhancing this artifact reinforced the importance of applying algorithmic, logical thinking to real-world software systems, rather than treating algorithms as isolated academic concepts. Designing the cache required careful consideration of how to generate reliable cache keys, how to maintain bounded memory usage through an eviction strategy, and how to integrate the enhancement without altering user-facing behavior. The primary challenge was ensuring that the enhancement remained focused on performance improvement rather than expanding the project’s scope. Overall, this process demonstrated how small, targeted changes to data structures and control flow can significantly improve efficiency and maintainability in an interactive application.
