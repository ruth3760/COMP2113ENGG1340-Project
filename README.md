# Summer Maxxing (Text-Based Mini-Game)

A small, terminal-based life-sim for COMP2113/ENGG1340.
You play through a summer break, balancing Energy, Health, Social, Academic, Fitness, and Money.

## Team Members

Venkata Sai Ruthvik Reddy NELLORE 3036384713 (ruth3760)

Matthew Chow SMITHSEREE 3036452338 (MatthewSmithseree)

Grace Yuhan PUI 3036136893 (gp13-dot)

Ingram BUDIANTO 3036440866 (IngramBudianto123)

*NOTE: Our GitHub contributions may not provide an accurate representation of individual contributions as we frequently exchanged devices. We all contributed equally in various aspects beyond coding, such as video editing, ideation, and testing, which are not fully reflected in GitHub.*

## Game Rules

Summer Maxxing is a life simulation game where you take on the role of a college student navigating the highs and lows of summer break.
With 8 weeks of freedom, you’ll make choices that shape your relationships, health, academics, fitness, and finances.
The scenarios are inspired by daily, real struggles of college students, and your decisions will decide not only your experience this summer, but also for the following semester, and perhaps your future.
So balance your stats, manage your time wisely, and make the most of your summer to achieve your goals.

This game is a blend of strategy, life simulation, and storytelling.
You’ll face random events, relationship drama, and tough choices that test your ability to manage resources and relationships.
With multiple endings and paths to explore, every summer tells a unique story.
Will you thrive or barely survive?

### Key Stats

Energy (0–100): Represents your ability to complete activities.
If it falls to 0, you’ll collapse from exhaustion, and it’s Game Over.

Health (0–100): Stay healthy by eating well, sleeping enough, and avoiding stress.
If it drops to 0, you’ll face a medical emergency, and it’s Game Over.

Social (0–100): Build relationships with friends (or more than friends?).

Academic (0–100): Some argue this is bare minimum for a college student...
So stay on top of your summer courses?

Fitness (0–100): Work on your summer bod!

Money (\$0–∞): Earn and spend wisely.
Running out of money leads to financial ruin and Game Over.

### How to Play

-   Each week has 4 scenarios, representing different time slots in your day (morning, afternoon, evening, and night).
-   For each scenario, you’ll choose from several activities that impact your stats and relationships.
-   Random events will occur! So don't think you've got it all figured out.
-   At the end of each week, your stats will decay slightly to reflect the toll of summer.
-   At the end of the game, your final stats will determine your summer’s outcome! There are multiple endings to discover with a special ending for players who master the art of balance and community building.

## How to Build & Run

To compile and run the game, run the following command onto the command line:

```         
make
./summer_maxxing
```

## Features implemented

-   **Generation of Random Game Sets or Events**:\
    Our game incorporates randomization to create dynamic and unpredictable gameplay experiences, including

    -   **Trivia mini-games**: Trivia questions are randomly selected from a pool of questions to ensure variety in each playthrough.

    -   **Random Events**: Between scenarios, random events are triggered that can have positive, negative, or neutral outcomes, impacting player stats and relationships.

-   **Data Structures for Storing Game Status**

    -   The game uses various data structures such as maps, vectors, structs, and lists, to manage and store the player's progress and stats

-   **Dynamic Memory Management**

    -   The game features efficient use of dynamic memory to handle gameplay elements and ensure smooth performance.
        Trivia questions, random events, and character dialogue are dynamically loaded into memory only when required, reducing unnecessary memory usage.
        Player stats and progress are updated dynamically during gameplay, ensuring real-time tracking without overloading the system.

-   **File Input/Output (e.g., for Loading/Saving Game Status)**

    -   The game implements a file management system to handle data efficiently.
        Player progress, including stats, relationships, and completed scenarios, is saved to a file, enabling players to resume their game seamlessly.
        Logs of random events and player decisions are stored in files for debugging purposes or storytelling enhancements.

-   **Proper Indentation and Naming Styles**

    -   The game adheres to consistent coding practices to ensure readability and maintainability.

-   **Multiple Difficulty Levels**

    -   Players can choose from three difficulty levels at the start of the game: **Easy** (Lower stat decay and more resources), **Medium (**Balanced gameplay with standard challenges), and **Hard** (Higher stat decay, limited resources, and additional financial obligations)

-   **Program Codes in Multiple Files**

    -   The game is modularized into multiple files for better readability and maintainability:

        -   `main.cpp`: Contains the main game plot and integrates all components.

        -   `player.cpp` and `player.h`: Manages player stats and functionality.

        -   `relationships.cpp` and `relationships.h`: Handles relationship management.

        -   `events.cpp` and `events.h`: Manages random events and outcomes.

        -   `activities.cpp` and `activities.h`: Implements mini-games and player activities.

        -   `savegame.cpp` and `savegame.h`: Manages saving and loading game progress.

## **List of Non-standard C++ Libraries**

N/A
