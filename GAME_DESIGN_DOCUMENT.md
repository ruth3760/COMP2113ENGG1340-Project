# Summer Maxxing - Game Design Document

## Game Overview
**Summer Maxxing** is a humorous text-based RPG where players attempt to optimize their summer break by balancing relationships, fitness, finances, and academics. The goal is to survive the 90-day summer period while achieving the best possible outcome based on player choices.

## Core Game Mechanics

### 1. Stats System
Players must manage five core stats that interact with each other:

- **Energy (0-100)**: Required for all activities, replenished by sleeping/resting
- **Health (0-100)**: Affected by food choices, exercise, and random events
- **Social Points (0-100)**: Represents relationship status with friends and love interests
- **Academic Performance (0-100)**: Determines grade outcome and future opportunities
- **Money ($0-$5000)**: Currency for all purchases and activities
- **Fitness Level (0-100)**: Affects health, energy efficiency, and relationship opportunities

### 2. Time Management
- Each day is divided into 4 time slots: Morning, Afternoon, Evening, Night
- Players can perform 1 major activity per time slot
- Some activities span multiple time slots
- Weekends have different available activities

### 3. Location & Activities

#### **Gym** 
- **Work Out** (2 hrs, -30 Energy, +5-8 Fitness, -$5)
- **Protein Shake** (30 min, -$8, +10 Health)
- **Flirt with Gym Crush** (1 hr, -10 Energy, varies Social outcome)

#### **University Hall**
- **Attend Class** (2 hrs, -20 Energy, +10-15 Academic)
- **Study Group** (3 hrs, -25 Energy, +15-20 Academic, +5 Social)
- **Club Activities** (2 hrs, -15 Energy, +10-15 Social)

#### **Room**
- **Sleep** (8 hrs, +50 Energy)
- **Study Alone** (4 hrs, -30 Energy, +10-15 Academic)
- **Online Side Hustle** (3 hrs, -20 Energy, +$20-40)
- **Gaming Marathon** (4 hrs, -15 Energy, +5 Social (online), -5 Academic)

#### **Cafeteria**
- **Healthy Meal** (1 hr, -$12, +15 Health)
- **Cheap Fast Food** (30 min, -$5, +5 Health, risk of -2 Fitness)
- **Coffee Date** (1.5 hrs, -$15, varies Social outcome)

#### **Interview Room** (Available after day 30)
- **Internship Interview** (2 hrs, -25 Energy, success based on Academic + Fitness)
- **Part-time Job Interview** (1 hr, -15 Energy, immediate income if successful)

### 4. Relationship System

#### Romance Options:
- **Gym Crush (Alex)**: Prefers high fitness, available at gym
- **Study Buddy (Sam)**: Prefers high academic performance, available at library
- **Coffee Shop Barista (Riley)**: Likes consistent attention, available at cafeteria
- **Gaming Friend (Jordan)**: Online relationship, requires gaming activities

#### Friendship Options:
- **Study Group Friends**: Academic-focused relationships
- **Gym Bros**: Fitness-focused relationships
- **Club Members**: Social activity relationships

**Relationship Progression:**
- Stranger → Acquaintance → Friend → Close Friend → Best Friend/Romantic Partner
- Each level requires different amounts of interaction and specific stat thresholds

### 5. Economic System

#### Income Sources:
- **Part-time Job**: $50/day (requires 2 hrs, available after successful interview)
- **Internship**: $80/day (requires 3 hrs, available day 45+, requires high academic)
- **Online Side Hustle**: $20-40 per session (unreliable)
- **Weekly Allowance**: $30 (from parents, requires Academic > 60)

#### Expenses:
- **Food**: $5-20/day (healthier options cost more)
- **Gym Membership**: $30/week
- **Transportation**: $10/week
- **Entertainment**: $15-50 per activity
- **Emergency Expenses**: Variable (random events)

### 6. Random Events System

#### Positive Events:
- **Inheritance**: +$500 (5% chance per week)
- **Gym Equipment Sale**: -50% on gym costs (10% chance)
- **Study Breakthrough**: +20 Academic (15% chance when studying)
- **Unexpected Date**: +30 Social Points (20% chance with high social)

#### Negative Events:
- **Sports Injury**: -20 Health, -10 Fitness, -$100 medical bill
- **Financial Emergency**: -$200 (family issues)
- **Food Poisoning**: -15 Health, -2 days energy regeneration
- **Academic Crisis**: -15 Academic Performance
- **Social Drama**: -20 Social Points

#### Neutral Events:
- **Weather Change**: Affects outdoor activity efficiency
- **Internet Outage**: Online activities unavailable for 1 day
- **Gym Closure**: 1 day maintenance

### 7. Difficulty Levels

#### **Easy Mode** - "Trust Fund Kid"
- Starting Money: $1000
- Starting Stats: All at 70
- Random events: 70% positive, 20% neutral, 10% negative
- Lower stat decay rates

#### **Medium Mode** - "Average Student"
- Starting Money: $300
- Starting Stats: All at 50
- Random events: 40% positive, 30% neutral, 30% negative
- Normal stat decay rates

#### **Hard Mode** - "Struggling Immigrant"
- Starting Money: $50
- Starting Stats: Energy 40, Health 60, Social 30, Academic 60, Fitness 20
- Random events: 20% positive, 30% neutral, 50% negative
- Higher stat decay rates
- Additional challenge: Must send $50 home every week

### 8. Ending System (Based on Final Stats)

#### **Ultra Chad Ending** (Perfect)
- Money > $2000, Fitness > 80, Social > 80, Academic > 80, Health > 80
- *Description*: "You crushed this summer! Perfect body, perfect grades, loaded wallet, and an amazing social life. You're basically a superhero!"

#### **Balanced Ending** (Good)
- All stats > 60, Money > $1000
- *Description*: "Solid summer! You managed to improve in all areas without going overboard. Future you thanks present you."

#### **Workaholic Ending** (Money-focused)
- Money > $3000, but Social < 40 and Fitness < 40
- *Description*: "You're rich but at what cost? Your Tinder matches have dried up and you get winded climbing stairs."

#### **Gym Bro Ending** (Fitness-focused)
- Fitness > 90, but Academic < 50 and Money < $500
- *Description*: "You're absolutely shredded! Too bad you failed your summer courses and can't afford protein powder anymore."

#### **No-life Ending** (Academic-focused)
- Academic > 90, but Social < 30 and Fitness < 30
- *Description*: "Straight A's! Your parents are proud but your only friend is the library security guard."

#### **Social Butterfly Ending** (Relationship-focused)
- Social > 90, but Money < $300 and Academic < 50
- *Description*: "You're the life of the party! Unfortunately, you partied so much you forgot about real life responsibilities."

#### **Barely Survived Ending** (Low overall)
- Multiple stats below 40
- *Description*: "You made it through summer but... was this really living? Time to reflect on your life choices."

## Technical Implementation Notes

### Required Features per COMP2113 Guidelines:
1. **Random Events**: Implemented through daily event generation system
2. **Data Structures**: Player stats, relationship tracking, activity queues
3. **Dynamic Memory Management**: Character relationship arrays, event systems
4. **File I/O**: Save/load game state, high score tracking
5. **Multiple Files**: Separate modules for game logic, UI, events, relationships
6. **Multiple Difficulty Levels**: Three distinct difficulty modes with different starting conditions

### File Structure:
- `main.cpp`: Game loop and primary interface
- `player.h/cpp`: Player stats and core mechanics
- `relationships.h/cpp`: NPC interaction system
- `events.h/cpp`: Random event generation
- `activities.h/cpp`: Location and activity management
- `savegame.h/cpp`: File I/O operations
- `Makefile`: Compilation instructions

### Key Programming Elements:
- Classes for Player, NPC, Activity, Event systems
- Inheritance for different types of events/activities
- File streams for save/load functionality
- Dynamic arrays for relationship tracking
- Random number generation for events
- String manipulation for text-based interface