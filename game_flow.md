# Summer Maxxing - Game Script & Flow Outline

## OPENING SEQUENCE

```         
[SCENE: Your dorm room, final exam week]
"Another semester down! Time to make this summer COUNT."
"Your goal: Survive 8 weeks while balancing relationships, fitness, academics, and finances."

[DIFFICULTY SELECTION]
► Trust Fund Kid (Easy) - "Life on easy mode, just don't screw up"
► Average Student (Medium) - "Typical college student struggles"  
► Struggling Immigrant (Hard) - "Got a family back home to provide for so you better work hard buddy"

[GAME INSTRUCTIONS APPEAR]

"WELCOME TO SUMMER MAXXING!"

YOUR MISSION:
"Survive 12 weeks of summer while balancing your stats and relationships. 
Make smart choices to achieve your ideal summer ending!"

KEY STATS TO MANAGE:
⚡ ENERGY (0-100) - Required for all activities
❤️ HEALTH (0-100) - Affects everything you do  
👥 SOCIAL (0-100) - Friendships and relationships
📚 ACADEMIC (0-100) - Summer courses and learning
💪 FITNESS (0-100) - Physical health and appearance
💰 MONEY ($0-5000) - Campus life isn't free!

CRITICAL WARNINGS:
🚨 If HEALTH ≤ 10: Medical emergency! Forced hospital visit
😴 If ENERGY ≤ 10: Collapse from exhaustion! Forced sleep  
💸 If MONEY ≤ $0: Financial crisis! GAME OVER

RELATIONSHIP SYSTEM:
• Meet Alex (Gym), Sam (Library), Riley (Cafe), Jordan (Online)
• Build affinity through interactions
• Relationships unlock special opportunities and storylines

HOW TO PLAY:
• There are 12 weeks for you this summer
• Each week has 4 scenarios (like time slots)
• Choose activities that balance your stats
• Random events will spice things up

TIPS FOR SUCCESS:
• Don't neglect any stat for too long
• Balance work, rest, and social life
• Build relationships consistently
• Watch your Energy levels
• Save money for emergencies

READY TO MAX YOUR SUMMER?
[Press any key to continue...]
```

### **EASY MODE - "Trust Fund Kid"**

```         
Energy: 80/100
Health: 75/100
Social: 70/100  
Academic: 40/100
Fitness: 65/100
Money: $1500

[SPECIAL BONUSES]:
- Weekly allowance: $50 (instead of $30)
- Lower stat decay (-20% reduction)
- Random events: 70% positive, 20% neutral, 10% negative
- All activities cost 15% less Energy
```

### **MEDIUM MODE - "Average Student"**

```         
Energy: 60/100
Health: 65/100
Social: 50/100
Academic: 50/100
Fitness: 45/100
Money: $300

[STANDARD CONDITIONS]:
- Weekly allowance: $30 (if Academic > 60)
- Normal stat decay rates
- Random events: 40% positive, 30% neutral, 30% negative
- Standard activity Energy costs
```

### **HARD MODE - "Struggling Immigrant"**

```         
Energy: 40/100
Health: 60/100
Social: 30/100
Academic: 60/100
Fitness: 20/100
Money: $100

[ADDITIONAL CHALLENGES]:
- Must send $50 home every week (automatic deduction)
- Higher stat decay (+25% increase)
- Random events: 40% positive, 30% neutral, 30% negative  
- Activities cost 10% more Energy
- No parental allowance
```

## ESSENTIAL GAME RULES

**Health & Energy Critical Thresholds - show it to users:**

-   If Health ≤ 10: "MEDICAL EMERGENCY!" → Forced hospital visit, Health +30, Energy +20, Money -\$100

-   If Energy ≤ 10: "COLLAPSE FROM EXHAUSTION!" → Forced sleep, Energy +40, skip next scenario

-   If Money ≤ \$0: "FINANCIAL CRISIS!" → GAME ENDS. "SORRY YOU DID NOT SURVIVE SUMMER ALONE IN HK"

## WEEK 1: THE FRESH START

### Scenario 1

```         
LOCATION: Dorm Room
"Summer break begins! Your alarm blares at 8 AM cus you forgot to change them from all the exam stress. You choose to:"
[CHOICES]:
1. "Sleep in" → Energy +40, Academic -3
2. "Go for a run" → Energy -25, Fitness +8, Health +5
3. "Plan your summer" → Energy -5, All stats +2 (motivation boost)
```

### Scenario 2

```         
LOCATION: Hall
"Everyone seems to be done with exams, so things are pretty quiet. Where to first?"
[CHOICES]:
1. "Check out the gym" → Energy -20, Fitness +5, "You spot an attractive person at the weights"
2. "Visit the library" → Energy -10, Academic +12, "Empty study rooms available"
3. "Hang in the common area in hall" → Energy -10, Social +8, Money -$5 (coffee)
```

#### Choice 1 - "Check out the gym"

```         
LOCATION: Gym
"You enter the gym near your hall - it's surprisingly modern and well-equipped. 
Only a handful of dedicated students are here this early in summer."

"You do a light workout to test the equipments
While adjusting the bench press, you notice an incredibly jacked person doing 
deadlifts with perfect form. They catch you looking and give a small smile before returning to their set.

This must be Alex， you've heard about them from your other friends about his insane physique."

[RELATIONSHIP IMPACT]:
- Alex affinity: +5 

[SPECIAL UNLOCKS]:
- "Gym" location now available for future scenarios
- Future gym activities cost 25% less Energy (familiarity bonus)

[ADDITIONAL DIALOGUE]:
"Leaving the gym, you feel energized and motivated. Maybe you should come back tomorrow..."
```

#### Choice 2 - "Visit the Library"

```         
LOCATION: Campus Library  
"You push through the heavy library doors. 
The summer session means only the most dedicated students are here."

[NARRATIVE FLOW]:
"You find a perfect study carrel with natural light and power outlets. You spent two hours pre-viewing the courses you plan on taking next semester.


While taking a break in the reference section, you notice someone with incredibly 
organized notes and color-coded textbooks. They seem to be working through the same problems as you, but at a much faster pace.

The student - Sam - looks up and notices you looking at them. They adjust their glasses before diving back to the course materials."

[RELATIONSHIP IMPACT]:
- Sam affinity: +3

[SPECIAL UNLOCKS]:
- "Study Group" activity now available after Week 2
- Future library studying costs 20% less Energy (comfort bonus)

[ADDITIONAL DIALOGUE]:
"As you leave, the librarian reminds you: 'Summer hours are 8 AM to 8 PM. 
We have study rooms available if you need group space.' You feel prepared for 
the academic challenges ahead."
```

#### Choice 3: "Hang at student union"

```         
LOCATION: Hall Common Area
"People are hanging around tables with laptops, board games, and coffee cups. Everyone seems to be unwinding from the busy semester."

"You grab an iced coffee from the campus cafe and find a comfortable armchair:

"You notice Riley, who is ordering coffee for everyone here. They have been an acqauintance of yours since the beginning of uni."

Riley: "Iced Americano again like usual right?"

[RELATIONSHIP IMPACT]:
- Riley affinity: +4 (positive interaction)

[SPECIAL UNLOCKS]:
- Future social activities cost 15% less Energy (comfort bonus)

[ADDITIONAL DIALOGUE]:
"Summer might be the perfect time to build real friendships without academic pressure."
```

### Scenario 3

```         
LOCATION: Cafeteria  
"Lunch time. Your stomach rumbles."
[CHOICES]:
1. "Salad bar" → Money -$12, Health +15, "Healthy but expensive"
2. "Pizza slice" → Money -$6, Health +3, "Quick and satisfying"
3. "Skip lunch" → Health -5, "Saving money but feeling hungry"
```

#### Choice 1: "Salad bar"

```         
LOCATION: Cafeteria
"You load up a plate with fresh greens, grilled chicken, and colorful vegetables. 
It looks healthy but costs a premium."

[RANDOM EVENT TRIGGERS]:
"Just as you're about to pay, you notice that Riley is part-timing here for the summer as a cashier"

RILEY: "Hey, fancy seeing you here! I work both spots during summer. That's a 
way healthier choice than most people make."

[EVENT OUTCOMES]:
- If you met Riley in Scenario 2: 
  "Riley gives you the employee discount!" → Money only -$8 instead of -$12
  Riley affinity: +3

- If you haven't met Riley yet:
  "Riley seems friendly and makes conversation" → Riley affinity: +5
  "They seem impressed by your healthy choice"

[ADDITIONAL DIALOGUE]:
"Eating your salad, you feel virtuous and energized. The quality is definitely 
better than the cheaper options."
```

#### Choice 2: "Pizza slice"

```         
LOCATION: Cafeteria  
"You grab a quick pepperoni slice from the counter. It's greasy but satisfying."

[RANDOM EVENT TRIGGERS]:
"As you're eating, you overhear two fitness-types at the next table..."

GYM BRO 1: "Can you believe some people actually eat that garbage during summer cut?"
GYM BRO 2: "I know, right? No discipline."

[EVENT OUTCOMES]:
- If you visited the gym in Scenario 2:
  "One of them recognizes you from the gym and gives an approving nod despite 
  your food choice" → No penalties, just shame lol

- If you haven't visited gym:
  "You feel self-conscious about your meal choice" → Social: -2 (awkwardness)
  "Maybe you should consider healthier options"
```

#### Choice 3: "Skip lunch"

```         
LOCATION: Campus Grounds
"You decide to save money and skip proper lunch, just grabbing a water fountain 
drink instead."
```

### Scenario 4

```         
LOCATION: Dorm Room
"Evening approaches. How to wind down?"
[CHOICES]:
1. "Video games" → Energy -10, Social +3 (online), Academic -2
2. "Study for summer course" → Energy -25, Academic +15
3. "Early bedtime" → Energy +35, "Well-rested for tomorrow"
```

**[End of Week event]**

```         
"WEEKLY UPDATE: Your parents check in..."
[EVENT]:
- If Academic > 40: "They're proud of your dedication! +$30 allowance"
- If Academic < 20: "They're concerned about your grades. -10 Social"
- Default: "Standard weekly allowance: +$30"

[STAT DECAY - Applied after Week 1]:
"End of Week 1 - Some skills naturally decline without practice:"
- Health: -3 (sedentary adjustment)
- Social: -4 (isolation tendency) 
- Academic: -5 (summer learning loss)
- Fitness: -6 (muscle atrophy)
```

**[Random Events]**

```         
[RANDOM EVENT 1 - Positive]:
"Your phone rings - it's an old high school friend!"
FRIEND: "Hey! I'm in town for the weekend and heard you're staying on campus. Want to catch up tomorrow?"
→ If accept: Social +10, Money -10, Academics -3
→ If decline: Social -3, "Missed connection"

[RANDOM EVENT 2 - Negative]:
"Power surge fries your router! No internet for the night."
→ Online activities impossible, Gaming/Streaming blocked
→ Academic -5 (can't access online materials), Energy -5 (frustration)

[RANDOM EVENT # - NEUTRAL]
LOCATION: Dorm Room
"Evening approaches. Your phone buzzes with notifications from group chats, but you decide to..."
[CHOICES]:
1. "Video games" → Energy -10, Social +5, Academic -2
2. "Study for summer course" → Energy -20, Academic +15  
3. "Early bedtime" → Energy +35, "Well-rested for tomorrow"

## each random event to be generated in between sceanrios
```

------------------------------------------------------------------------

## WEEK 2: SETTLING INTO YOUR ROUTINE

## Scenario 1

```         
LOCATION: Dorm Room
"Monday again. The summer sun is already bright. You wake up feeling..."
[CHOICES]:
1. "Hit the gym early" → Energy -25, Fitness +8, Money - 2
2. "Review summer course materials" → Energy -20, Academic +12
3. "Grab coffee and socialize" → Energy -10, Social +8, Money -$5
```

### Choice 1 - "Hit the gym early"

```         
LOCATION: Gym
"The morning crowd is sparse but dedicated. Alex is already there."

ALEX: "Hey sorry, do you mind spotting me for a sec?"

[CHOICES]: 
1. "Sure no problem" -> Alex affinity: +8 (offering help), Fitness: +3 bonus (they shared you some tips on how to train more efficiently)
2. "Sorry I'm in the middle of my set too." → Alex affinity: -5

[RANDOM EVENT - Positive]:
"Gym manager announces free personal training session giveaway!"
→ You win! → Fitness +10
```

### Choice 2 - "Review summer course materials"

```         
LOCATION: Library
[RANDOM EVENT - printer breakdown with chance 50%]

"Sam is at the library printer, he seems to be having some troubles. He stops you as you walk past."

SAM: "Hey sorry, do you know what's going on with the printer today?"

"You helped them printing our materials for the course you two are both studying for. Afterwards you guys had a nice study session together before you headed back home."

[RELATIONSHIP IMPACT]:
- Sam affinity: +7 
```

### Choice 3 - "Grab coffee and socialize"

```         
LOCATION: Hall Common Area
"Riley is sitting there on her laptop, they seems to be working on her last assignment for the semester."

"They share funny stories about weekend customers at their part-time."

[RELATIONSHIP IMPACT]:
- Riley affinity: +9 (consistent morning ritual)
- Social: +3 bonus (comfortable interaction)
```

## Scenario 2

```         
LOCATION: Various
"Midday energy slump hits. Time to decide your afternoon focus."
[CHOICES]:
1. "Intense workout session" → Energy -20, Fitness +10, Health +2
2. "Library deep dive" → Energy -25, Academic +15
3. "Take a nice nap" - Enegy +35
```

### Choice 1 - "Intense workout session"

```         
LOCATION: Gym
"Alex notices you pushing heavier weights than last week. You asked if they could spot you for your top set. "
ALEX: "Of course."
"As you are racking the weights, you almost slipped and injured yourself, thankfully Alex grabbed on to the barbells just in time."
ALEX: "WOAH WOAH, be careful."

[RELATIONSHIP IMPACT]:
- Alex affinity: +10 (investment in your growth)
- Fitness: +4 bonus (pushed beyond comfort zone)

[RANDOM EVENT - Positive]:
"Gym is giving out free protein shakers" - Fitness +1
```

### Choice 2 - "Library deep dive"

```         
LOCATION: Library Study Room
[RANDOM EVENT - Negative]
"Library is surprisingly crowded today, there are spare seats so you have to go home to study instead." → Academic -7 (slowed progress)
```

### Choice 3 - "Take a nice nap"

```         
LOCATION: DORM
[RANDOM EVENT - Negative]:
"You ended up scrolling too much instead of actually napping" → Energy -15 (less energy gain), 
```

## [RANDOM EVENT]

```         
"UNEXPECTED CAMPUS WIDE EVENT!"

[RANDOM ROLL]:
- POSITIVE: "Free campus movie night on the quad!" 
  → Social +8, "Great chance to mingle"

- NEGATIVE: "Campus construction starts nearby"
  → Noise distractions, -2 to all stat gains today
```

## Scenario 3

```         
LOCATION: Various
"Late afternoon. The day is winding down but there's still time for..."
[CHOICES]:
1. "Evening gym session" → Energy -20, Fitness +6, "Less crowded now", Health +3
2. "Quick study review" → Energy -15, Academic +8, "Reinforce today's learning"
3. "Wind down socially" → Energy -10, Social +6, Money -$8, 
```

### Choice 1 - "Evening gym session"

```         
LOCATION: Gym

"You do lighter work, focusing on form and recovery."

[RANDOM EVENT - Positive (20% chance)]:
"Gym is unexpectedly empty - perfect for trying new equipment!"
→ Fitness +4 (trying advanced machines), Health +2
```

### Choice 2 - "Quick study review"

```         
LOCATION: Library
"You review today's material while it's still fresh. The concepts click into place more easily."

[If you have met Sam before]
"Sam waves from across the room."

[RELATIONSHIP IMPACT]:
- Sam affinity: +2 

[RANDOM EVENT - Negative (25% chance)]:
"Brain fog sets in - you've been studying too long!"
→ Academic gains reduced by 50%
→ Energy -8 (mental exhaustion)
→ "Maybe time for a break..."
```

### Choice 3 - "Wind down socially"

```         
LOCATION: Various Social Spaces
"People are decompressing from the day. The mood is relaxed and friendly."

"Riley spots you and waves you over to their table where friends are sharing stories and laugh."

[RELATIONSHIP IMPACT]:
- Riley affinity: +7 (casual bonding)
- Social: +4 bonus (comfortable environment)

[RANDOM EVENT - Neutral]:
"Riley asks if you want to meet some of her friends who have recently graduated. This could be a great opportunity to network with alumni."
→ Join? → Money -$20, Social +20, Health -3
→ Decline → No effect
```

## Scenario 4

```         
LOCATION: Dorm Room
"Evening settles in. Your phone shows various notifications. Time to..."
[CHOICES]:
1. "Gaming session" → Energy -5, Social +5 (online), Academic -3
2. "Light studying" → Energy -10, Academic +10
3. "Early rest" → Energy +30, Health +5, "Recover for tomorrow"
```

### Choice 1 - "Gaming session"

```         
LOCATION: Dorm Room
"You log into your favorite game. Jordan is already online and immediately invites you to party up."

JORDAN: "Perfect timing! We need a reliable teammate for this raid. You in?"

"The session is smooth and coordinated. Jordan's strategic mind makes even difficult content feel manageable."

[RELATIONSHIP IMPACT]:
- Jordan affinity: +8 (gaming partnership)
- Social: +3 bonus (online camaraderie)

[RANDOM EVENT - Positive]:
"You get an ultra-rare item drop!" → Energy +5
```

### Choice 2 - "Light studying"

```         
LOCATION: Dorm Desk
"You review notes and start to prepare for the upcoming recruitment cycle. The quiet evening is perfect for consolidation."

[RANDOM EVENT - Negative (20% chance)]:
"Neighbors are having a loud party next door!"
→ Concentration broken, study efficiency -50%
→ Energy -10 (frustration), Social +2 (if you join them?)
```

### Choice 3 - "Early rest"

```         
LOCATION: Dorm Room
"You ignore the notifications and focus on recovery. The extra sleep feels luxurious."
"Waking up refreshed tomorrow will give you an edge, though you might miss some social opportunities."

[RANDOM EVENT - Positive]:
"Your body thanks you with perfect sleep!"
→ Energy +10 bonus tomorrow
→ All stats +1 (well-rested boost)
```

## [END OF WEEK 2 EVENTS]

### Stat Decay

```         
"End of Week 2 - Maintaining momentum gets harder:"
- Health: -4 (routine stress)
- Social: -5 (relationships need maintenance)
- Academic: -6 (information overload)
- Fitness: -7 (plateau risk)
- Energy: -8 (cumulative fatigue)
```

### Additional Random Events

```         
[RANDOM EVENT 1 - Positive]:
"Unexpected care package from home!"
→ Health +8 (homemade food), Social +3 (family connection)
→ Money +$20 (emergency cash)

[RANDOM EVENT 2 - Negative]:
"Summer course assignment harder than expected!"
→ Academic -5, Energy -10 (extra work required)
→ "Should have started this earlier..."

[RANDOM EVENT 3 - Neutral]:
"Campus wifi upgrade in progress"
→ Online activities unavailable in Scenario 4
```

# WEEK 3: DEEPENING CONNECTIONS

## Scenario 1

```         
LOCATION: Dorm Room
"Wednesday morning. You're starting to find your summer rhythm. Time to..."
[CHOICES]:
1. "Gym workout" → Energy -25, Fitness +8
2. "Library session" → Energy -20, Academic +12  
3. "Look for part-time work" → Energy -15, Money opportunity
```

### Choice 1 - "Gym workout"

```         
LOCATION: Gym
"Alex is working on pull-ups when you arrive. They drop down and approach you."

ALEX: "Hey, I was hoping you'd come. Some of us are doing a weekend hiking trip - thought you might be interested?"

[CHOICES]:
1. "Definitely! When?" → Alex affinity: +12, Energy -15, Fitness +5
2. "Maybe another time" → Alex affinity: +3, Fitness: +6
3. "Not really my thing" → Alex affinity: -5, "Alex seems disappointed"
```

### Choice 2 - "Library session"

```         
LOCATION: Library  
"Sam is here today. You sat down next to them since there are not many spaces today."

[If never talked to Sam before]
SAM: "Oh are you also taking this course next semester?"
"You guys had a nice study session together."
Sam affinity +3

[Else]
SAM: "I am stuck on this question for quite some while now, do you have any idea on to do it?"
if academic > 70 - solves the question, Sam affinity +5
otherwise - stuck too, Sam affinity +2

[RANDOM EVENT - Positive]:
"Library assistant job posting spotted!"
→ Apply now: Requires Academic > 60
→ If qualified: Money +$45 per week, Academic +3 weekly
```

### Choice 3 - "Look for part-time work"

```         
LOCATION: Campus Job Board
"You browse through summer job postings. Several options catch your eye."

[CHOICES]:
1. "Campus cafe barista" → Money +$12/hour, Social +2 per shift, requires Social > 40
2. "Library assistant" → Money +$15/hour, Academic +2 per shift, requires Academic > 60  
3. "Gym attendant" → Money +$10/hour, Fitness +2 per shift, requires Fitness > 50

[RANDOM EVENT - Positive]:
"Quick online survey opportunity!"
→ Complete during job search: Money +$25
```

## Scenario 2

```         
LOCATION: Various
"Midday approaches. Your energy is holding up. Focus on..."
[CHOICES]:
1. "Serious training" → Energy -30, Fitness +10, Health +2
2. "Academic push" → Energy -25, Academic +15
3. "Cafeteria Lunch" → Energy -10, Social +12, Money -$15
```

### Choice 1 - "Serious training"

```         
LOCATION: Gym
"Alex pushes you through an intense circuit training session."

[RELATIONSHIP IMPACT]:
- Alex affinity: +8 

[RANDOM EVENT - Negative (20% chance)]:
"Minor muscle strain from overexertion!"
→ Fitness -15, Health -5, Energy -10
→ "Maybe should have warmed up better..."
```

### Choice 2 - "Academic push"

```         
LOCATION: Library Study Room
"Sam has reserved a study room for an afternoon study sesh."

[RELATIONSHIP IMPACT]:
- Sam affinity: +4

[RANDOM EVENT - Positive]:
"One-off Tutor needed for an upcoming assignment!"
→ Requires Academic > 70, pays $20/hour
```

### Choice 3 - "Networking lunch"

```         
LOCATION: Campus Cafe
"You are hanging out with Riley during her lunch break."

[RANDOM EVENT - Neutral]
RILEY: "This is the person I was telling you about - always productive and reliable."
MANAGER: "We're actually looking for summer help. Interested in an interview?"

If social > 60 - passes interview, additional $10 each week. Riley affinity +5
Otherwise - nothing happens
[Negative random event]
"You arrived late by 20 minutes and completely bomb your interview. Riley's manager told her you are very unprofessional and asked her why she even introduced you." - Riley affinity -15, Social - 20 (increase anxiety for future interviews)
```

## [RANDOM EVENT - Between Scenarios 2 and 3]

```         
"GPA RESULTS CAME OUT!"

[RANDOM ROLL]:
- 35% POSITIVE: "STELLA GRADES WOOHOO!"
  → All stats +3 (motivation boost), Energy +10

- 35% NEGATIVE: "You got a D for one of the courses, and C for another."
  → -2 to all stat gains (major confidence loss + scroll in bed to rot away your sorrows)

- 30% NEUTRAL: "It was aight, maybe should have worked harder but nonetheless happy with your results.."
```

## Scenario 3

```         
LOCATION: Various  
"Late afternoon. The summer heat is peaking. Time for..."
[CHOICES]:
1. "Cool gym workout" → Energy -20, Fitness +6, "Air conditioned"
2. "Focused study" → Energy -20, Academic +10, "Beat the heat indoors"
3. "Pool time!" → Energy -15, Fitness +2, Money -$8
```

### Choice 1 - "Cool gym workout"

```         
LOCATION: Gym
"The AC is a welcome relief. Alex is there with a smaller, more serious group."

ALEX: "Heat training builds mental toughness. Let's push through this."

[RELATIONSHIP IMPACT]:
- Alex affinity: +6 (shared dedication)


[RANDOM EVENT - Negative (25% chance)]:
"Gym AC breaks down!"
→ Workout becomes miserable, Fitness gains halved
→ Energy -15, Health -3
→ "Should have gone to the pool instead..."
```

### Choice 2 - "Focused study"

```         
LOCATION: Library

[Sam appears with 50% probability] - and if they appear
"Sam has secured the quietest study corner, away from the afternoon crowd."

SAM: "Perfect studying conditions. Let's make the most of this."

[RELATIONSHIP IMPACT]:
- Sam affinity: +7 
```

### Choice 3 - "Pool socializing"

```         
LOCATION: Campus Pool
[Riley appears with 50% probability] - and if they appear
"Riley and friends have claimed a prime spot. They wave you over enthusiastically."

[RELATIONSHIP IMPACT]:
- Riley affinity: +10 (summer fun together)
- Social: +5 bonus (relaxed environment)

[RANDOM EVENT - Positive (35% chance)]:
"Perfect summer afternoon!"
→ Social +8 bonus, Health +5, Fitness +3
→ "This is what summer memories are made of"
```

## Scenario 4

```         
LOCATION: Dorm Room
"Evening brings relief from the heat. Your phone shows various opportunities..."
[CHOICES]:
1. "Online gaming" → Energy -10, Social +6 (online), Academic -2
2. "Evening study" → Energy -20, Academic +12
3. "Rest and plan" → Energy +25, All stats +1 (reflection)
```

### Choice 1 - "Online gaming"

```         
LOCATION: Dorm Room
"Jordan is already online and immediately sends you a voice chat invite."

JORDAN: "Perfect timing! I found this new co-op game I think you'll love."

[RELATIONSHIP IMPACT]:
- Jordan affinity: +9 (gaming innovation)
- Social: +4 bonus (shared discovery)

[RANDOM EVENT - Money Opportunity (15% chance)]:
"Win gaming tournament prize!"
→ Money +$50, Social +10, Jordan affinity: +15
→ "Your skills are actually paying off!"
```

### Choice 2 - "Evening study"

```         
LOCATION: Dorm Desk
"You organize your notes and plan the rest of the week's academic work."

"If you've been studying with Sam, they text you helpful resources and encouragement."

[RELATIONSHIP IMPACT]:
- Sam affinity: +5 (academic support)
- Academic: +3 bonus (consistent effort)

[RANDOM EVENT - Negative (20% chance)]:
"Summer course assignment more complex than expected!"
→ Academic -5, Energy -15 (extra work required)
→ "Should have started this earlier in the week..."
```

### Choice 3 - "Rest and plan"

```         
LOCATION: Dorm Room
"You take time to reflect on your summer progress and plan upcoming weeks."

"This strategic thinking pays off in better time management and clearer goals."

[RELATIONSHIP IMPACT]:
- All stats: +1 bonus (strategic planning)
```

## [END OF WEEK 3 EVENTS]

### Weekly Update

```         
"WEEK 3 COMPLETE: Summer is flying by!"
[EVENT]:
- If employed: "Paycheck!" → Money +$80-120 (depending on job)
- If multiple stats low: "Time to focus before it's too late" → Warning
```

### Stat Decay

```         
"End of Week 3 - Maintaining multiple areas gets tougher:"
- Health: -5 (summer fatigue)
- Social: -6 (relationships need active maintenance)
- Academic: -7 (information overload)
- Fitness: -8 (plateau risk increases)
- Energy: -9 (cumulative summer drain)
```

### Additional Random Events

```         
[RANDOM EVENT 1 - Positive (20% chance)]:
"Unexpected scholarship refund!"
→ Money +$150, Academic +5
→ "All that studying actually pays sometimes"

[RANDOM EVENT 3 - Money Opportunity (30% chance)]:
"Quick freelance gig available!"
→ Complete online: Money +$35, Energy -10
→ "Easy money between main activities"
```

# WEEK 4: PRESSURE BUILDS

## Scenario 1

```         
LOCATION: Dorm Room
"Monday morning. You're feeling the summer pressure mounting. Time to..."
[CHOICES]:
1. "Gym - push through fatigue" → Energy -30, Fitness +10
2. "Library - midterm prep" → Energy -25, Academic +15  
3. "Side hustle search" → Energy -20, Money opportunity
```

### Choice 1 - "Gym - push through fatigue"

```         
LOCATION: Gym
"Alex looks concerned when they see you."

[If Alex affinity > 25]:
ALEX: "You look exhausted, man. Maybe take it easy today?"

[CHOICES]:
1. "Push harder" → Energy -35, Fitness +12, Health -8
2. "Listen to Alex" → Energy -15, Fitness +5, Health +3, Alex affinity: +8
3. "Leave and rest" → Energy +20, Fitness -3

[RANDOM EVENT - Negative (40% chance if pushed harder)]:
"OVERTRAINING HITS HARD!"
→ Health -15, Energy -25, Fitness -5
→ "Your body rebels against the abuse"
```

### Choice 2 - "Library - midterm prep"

```         
LOCATION: Library  
"Sam looks stressed but determined."

[If Sam affinity > 20]:
SAM: "Tomorrow here same time and grab lunch afterwards?"

[CHOICES]:
1. "Study marathon" → Energy -40, Academic +20, Sam affinity: +10
2. "Quick session" → Energy -25, Academic +12, Sam affinity: +3
3. "Too busy" → Energy -20, Academic +8, Sam affinity: -10

[RANDOM EVENT - Positive (50% chance if studying)]:
"STUDY BREAKTHROUGH!"
→ Academic +15, Energy +10 (adrenaline)
→ "Everything finally clicks into place"
```

### Choice 3 - "Side hustle search"

```         
LOCATION: Campus Job Center
"More urgent financial pressure this week."

[CHOICES]:
1. "Apply for multiple jobs" → Energy -25, chance of employment +30%
2. "Focus on one good opportunity" → Energy -20, chance of employment +10%  
3. "Online freelancing" → no change in chance of employment

[Chance of emplyoment] = 40% for extra $10 per week
```

## Scenario 2

```         
LOCATION: Various
"Midweek energy crash. The summer grind is real..."
[CHOICES]:
1. "Forced workout" → Energy -35, Fitness +12, Health -5
2. "Academic emergency" → Energy -30, Academic +18, Social -5
3. "Money desperation" → Energy -25, Money +$50, All other stats -3
```

### Choice 1 - "Forced workout"

```         
LOCATION: Gym
"Your body screams in protest with every rep."

[If Alex affinity > 30]:
ALEX: "Seriously, you look awful. Let me buy you a protein shake and we can talk."

[CHOICES]:
1. "Accept help" → Health +10, Energy +15, Alex affinity: +8
2. "Push through pain" → Fitness +8, Health -12, "Maybe not worth it"

[RANDOM EVENT - Extreme Negative (20% chance)]:
"MAJOR INJURY!"
→ Health -25, Fitness -15, Energy -30, Medical bill: $140
→ "You're out of commission for days"
```

### Choice 2 - "Academic emergency"

```         
LOCATION: Library
"Sam looks like they haven't slept in days."

[If Sam affinity > 25]:
"Sam opens up to you about the academic pressure that they are constantly under, and they seems to be really stressed out about his future. You listen to them and decides to:"

[CHOICES]:
1. "Share your experience which might end up with a long therapy session to talk about uni and life" → Energy -45, Sam affinity: +15, Health -10
2. "Short but sweet words of affirmation" → Sam affinity: +1
3. "Sounds like a you problem lol." → Sam affinity -50
```

### Choice 3 - "Money desperation"

```         
LOCATION: Various
"You're running dangerously low on funds."

[CHOICES]:
1. "Sell textbooks" → Money +$60-120, Academic -8 (losing resources)
2. "Take sketchy gig" → Money +$80, Risk of negative event (50% chance)
3. "Borrow from friends" → Money +$50, Social -10, "Awkward conversations"

[If Riley affinity > 30]:
RILEY: "I heard you're struggling. I can lend you some cash?"
→ Money +$75, Riley affinity: +8, "True friend emerges"

[RANDOM EVENT - Negative (50% chance with sketchy gig)]:
"GET ROBBED!"
→ Money -$70
→ "Never take dark alley gigs again"
```

## [RANDOM EVENT - Between Scenarios 2 and 3]

```         
"HEALTH CRISIS DECLARED!"

[RANDOM ROLL]:
- 40% NEGATIVE: "SUMMER FLU EPIDEMIC HITS!"
  → Health -20, Energy -25, All activities +10 Energy cost for 3 scenarios
  → "Should have washed your hands more"

- 30% POSITIVE: "CAMPUS WELLNESS PROGRAM LAUNCHES!"
  → Free health resources: Health +15, Energy +20
  → "Perfect timing for struggling students"

- 30% NEUTRAL: "HEAT WAVE CONTINUES"
  → Outdoor activities impossible, Indoor spaces overcrowded
  → All stat gains reduced by 3 this scenario
```

## Scenario 3

```         
LOCATION: Various  
"The week is breaking you. One more push before weekend..."
[CHOICES]:
1. "Final workout" → Energy -25, Fitness +2, "Last chance for gains"
2. "Last-minute studying" → Energy -25, Academic +12, "Midterms approaching"
3. "Social recovery" → Energy -15, Social +10, Money -$12, "Mental health break"
```

### Choice 1 - "Final workout"

```         
LOCATION: Gym

[If Fitness > 60]:
"You're actually still making marginal progress despite everything"

[If Fitness < 50]:
"Yeah bro don't over-train yourself. You leave feeling worse than when you entered."
→ Fitness -8, Health -10, "Maybe focus elsewhere next week"

[RANDOM EVENT - Neutral (20% chance)]:
"MAGIC POTION: increase your health and fitness by 20 for only $50!"
→ Buy protein powder? → Money -$50, Fitness +20, Health +20
→ "Nah magic potions are fake."
```

### Choice 2 - "Last-minute studying"

```         
LOCATION: Library

[If Academic > 70]:
"Decent progress, though slowed"

[If Academic < 50]:
"You're so demotivated and cannot study at all..."
→ Academic -10, "This might not be enough"

[RANDOM EVENT - Negative (35% chance)]:
"MENTAL BURNOUT!"
→ Academic -10, Energy -20, Health -5
→ "Your brain refuses to cooperate anymore"
```

### Choice 3 - "Social recovery"

```         
LOCATION: Campus Hangout
"Riley notices you look exhausted."

[If Riley affinity > 25]:
RILEY: "You look like you need a break. My treat today."
→ Money saved: $15, Social +8, Riley affinity: +10

[Else]:
"You try to relax but keep thinking about everything you should be doing"
→ Social +3, "Not very effective"

[RANDOM EVENT - Positive (30% chance)]:
"UNEXPECTED CAMPUS PARTY!"
→ Social +15, Energy -20, Academic -10
→ "Sometimes you need to let loose"
```

## Scenario 4

```         
LOCATION: Dorm Room
"Friday night. You're completely drained. Time to..."
[CHOICES]:
1. "Complete collapse" → Energy +40, Health +10
2. "Weekend prep" → Energy -20, All stats +3 (planning payoff)
3. "Emergency measures" → Money -$30, Energy +25, Health +15 (delivery food/supplements)
```

### Choice 1 - "Complete collapse"

```         
LOCATION: Dorm Room
"You fall into bed and don't move for hours."

[If Health < 40]:
"Your body desperately needed this"
→ Health +15 bonus, Energy +20 bonus

[If multiple stats < 50]:
"Maybe you've been pushing too hard"
→ All stats +2 (wisdom from exhaustion)

[RANDOM EVENT - Negative (25% chance)]:
"WAKE UP SICK ANYWAY!"
→ Health -10, Energy -15
→ "The damage was already done"
```

### Choice 2 - "Weekend prep"

```         
LOCATION: Dorm Desk  
"You plan the weekend to maximize recovery and progress."

[If any stat < 40]:
"You identify your weakest areas and plan focused improvement"
→ That stat +8 by end of week

[If all stats > 60]:
"You're maintaining impressive balance"
→ All stats +1, "The perfect summer continues"
```

### Choice 3 - "Emergency measures"

```         
LOCATION: Dorm Room
"You spend money to solve your problems temporarily."

[CHOICES]:
1. "Food delivery feast" → Money -$45, Health +12, Energy +15
2. "Energy drinks/supplements" → Money -$40, Energy +20, Health -10  
3. "Entertainment escape" → Money -$15, Social +8, Academic -3

[RANDOM EVENT - Negative (40% chance)]:
"BUYER'S REMORSE!"
→ Money wasted feeling: All stats -2
→ "That money could have been saved"
```

## [END OF WEEK 4 EVENTS]

### Weekly Update

```         
"WEEK 4 COMPLETE: The summer grind is real!"
[EVENT]:
- If employed: "Paycheck arrives just in time!" → Money +$80-120
- If Health < 30: "HEALTH WARNING: You're burning out!" → Health -5 additional
```

### Stat Decay

```         
"End of Week 4 - The pressure shows:"
- Health: -8 (accumulated stress)
- Social: -7 (neglected relationships) 
- Academic: -9 (midterm pressure)
- Fitness: -8 (overtraining risk)
- Energy: -12 (summer exhaustion)

[CRISIS THRESHOLDS WARNING FOR HEALTH, ENERGY, and MONEY]
```

### Additional Random Events

```         
[RANDOM EVENT 1 - Extreme Negative (15% chance)]:
"MENTAL HEALTH CRISIS!"
→ All stats -10, Energy -30
→ "The summer pressure becomes too much"
→ Requires 2 scenarios of rest to recover

[RANDOM EVENT 2 - Positive (20% chance)]:
"UNEXPECTED INHERITANCE FROM RELATIVE!"
→ Money +$200, Social +5 (family connection)
→ "Financial pressure relieved!"

[RANDOM EVENT 3 - Money Opportunity (25% chance)]:
"URGENT FREELANCE GIG - HIGH PAY!"
→ Complete immediately: Money +$75, Energy -25
→ "The grind pays off big time"
```

# WEEK 5: TURNING POINT

## Scenario 1

```         
LOCATION: Dorm Room
"Monday morning. The pressure from last week lingers, but there's hope ahead."
[CHOICES]:
1. "Recovery workout" → Energy -15, Fitness +6, Health +3
2. "Study planning" → Energy -10, Academic +8, "Organize for the week"
3. "Financial planning" → Energy -10, Money +$20 (smart budgeting)
```

### Choice 1 - "Recovery workout"

```         
LOCATION: Gym
"Alex greets you with genuine concern."

[If Health < 40]:
ALEX: "You look rough. Let's do a light active recovery day, okay?"

[CHOICES]:
1. "Listen to Alex" → Energy -10, Fitness +3, Health +5, Alex affinity: +8
2. "Push anyway" → Energy -25, Fitness +8, Health -5, "Stubborn but effective"

[If Alex affinity > 50]:
ALEX: "I've noticed you've been consistent. Want to be workout partners officially?"
→ "Of course, thought we already are haha." - Alex affinity: +8
→ "No bruh I have a workout partner already and they're like so much bigger than you lol" -Alex affinity -60
```

### Choice 2 - "Study planning"

```         
LOCATION: Library
"Sam looks more organized than ever."

[If Academic < 50]:
SAM: "I made a study schedule that might help us both. Want to see it?"
→ Academic +10, Sam affinity: +8


[CHOICES]:
1. "Follow Sam's plan" → Energy -15, Academic +12, Sam affinity: +8
2. "I have a better plan but thanks anyawys" → Sam affinity: -10
3. "Wing it" → Energy -5, Academic +4, "Risky strategy"

[RANDOM EVENT - Money Opportunity (30% chance)]:
"Research study pays participants!"
→ Quick $40 for 1-hour cognitive test, Academic +2
```

### Choice 3 - "Financial planning"

```         
LOCATION: Dorm Room
"You review your summer finances with growing concern."

[If Money < $100]:
"FINANCIAL WAKE-UP CALL - need stable income now!"
[CHOICES]:
1. "Budget strictly" → Money +$30 (savings)
2. "Find additional work" → Energy -30, Health -15, Money +$50 
3. "Cut expenses drastically" → Money +$50, Health -5, Social -15, "Austerity measures"

[RANDOM EVENT - Positive (25% chance)]:
"UNEXPECTED REFUND!"
→ Money +$60, "Campus overcharged you last semester"
```

## Scenario 2

```         
LOCATION: Various
"Midweek balance. Time to address your weakest area..."
[CHOICES]:
1. "Fitness focus" → Energy -20, Fitness +10, "Build momentum"
2. "Academic catch-up" → Energy -25, Academic +15, "Midterms approaching"  
3. "Social rebuilding" → Energy -15, Social +12, Money -$10
```

### Choice 1 - "Fitness focus"

```         
LOCATION: Gym
"Your body feels more responsive today."

[If Fitness < 50]:
"Breaking through the plateau feels amazing!"
→ Fitness +5 bonus, "Finally seeing progress"

[If Alex affinity > 40]:
ALEX: "Do you try some advanced techniques with me? I saw them yesterday on Instagram and they seem pretty cool"
→ Fitness +8, Alex affinity: +6

[RANDOM EVENT - Negative]:
"OLD INJURY FLAIR UP!"
→ Fitness -10, Health -8, Energy -15
→ "Should have listened to your body"
```

### Choice 2 - "Academic catch-up"

```         
LOCATION: Library  
[Sam here today with probability of 60%] - if he appears, Academic +10
SAM: "Wanna grab dinner together tonight?"
→ "Sure" Sam affinity: +8
→ "Been waiting for you to ask me out", Sam affinity: -10 (if affinity level < 60, why you making it weird) (otherwise, +1, you made it a bit awkward but it's ok cus they might actually be into you like that?)
→ "Sorry busy tonight", Sam affinity: -5 
→ "No.", Sam affinity: -15 

[RANDOM EVENT - Positive (35% chance)]:
"PROFESSOR OFFERS EXTRA HELP!"
→ Academic +15, "One-on-one guidance makes everything clearer"
```

### Choice 3 - "Social rebuilding"

```         
LOCATION: Campus Social Spaces
"Riley notices you've been distant."

[If Social < 40]:
RILEY: "Hey stranger! We've missed you. Everything okay?"
→ Social +10, Riley affinity: +8, "Reconnecting feels good"

[If Riley affinity > 50]:
RILEY: "I was getting worried about you. Let's catch up properly."
→ Social +12, Riley affinity: +12, "Deep conversation"

[CHOICES]:
1. "Open up" → Energy -10, Social +15, Riley affinity: +10
2. "Keep it light" → Energy -5, Social +8, Riley affinity: +3  
3. "Make excuses" → Energy -2, Social +2, Riley affinity: -5

[RANDOM EVENT - Money Opportunity (25% chance)]:
"FRIEND NEEDS HELP MOVING - PAYS WELL!"
→ Money +$60, Energy -20, Social +5, "Helping friends pays off"
```

## [RANDOM EVENT - Between Scenarios 2 and 3]

```         
"TIME TO SPICE UP THE SOCIAL GAME"

[RANDOM ROLL]:
- 40% POSITIVE: "It's your birthday! You hosted a great party with all your friends!"
  → Social +15, Energy -10, "Great morale boost"
  → All relationships +5 affinity

- 35% NEGATIVE: "Someone has been spreading rumours about you and your latest relationship with Alex, Sam, and Riley..."
  → All relationship -20

- 25% NEUTRAL: "Nothing special happened"
  → No changes
```

## Scenario 3

```         
LOCATION: Various  
"Late afternoon. The summer rhythm feels more natural now."
[CHOICES]:
1. "Sustainable workout" → Energy -15, Fitness +7, Health +2
2. "Effective studying" → Energy -18, Academic +13, "Quality over quantity"
3. "Meaningful socializing" → Energy -12, Social +10, Money -$8
```

### Choice 1 - "Sustainable workout"

```         
LOCATION: Gym
"You focus on form and consistency over intensity."

[RANDOM EVENT - Positive (30% chance)]:
"DISCOVER NEW TRAINING METHOD!"
→ Fitness +8, Energy efficiency +10% for future workouts
→ "Innovation leads to breakthroughs"
```

### Choice 2 - "Effective studying"

```         
LOCATION: Library
"You use proven study techniques instead of just grinding."

[CHOICES]:
1. "Pomodoro method" → Energy -15, Academic +12, "Focused bursts"
2. "Active recall" → Energy -18, Academic +14, "Deep learning"
3. "Spaced repetition" → Energy -12, Academic +10, "Long-term retention"

[RANDOM EVENT - Money Opportunity (20% chance)]:
"PAID STUDY GROUP LEADER NEEDED!"
→ Money +$25/hour, Academic +3, Social +2
→ Requires Academic > 65
```

### Choice 3 - "Meaningful socializing"

```         
LOCATION: Various
"You prioritize quality connections over quantity."

[If Riley affinity > 45]:
RILEY: "I feel like we're becoming real friends! Wanna go out tomorrow?"
→ Social +12, Riley affinity: +10

[CHOICES]:
1. "One-on-one time" → Energy -8, Social +10, Riley affinity +10, Money -$20 (if Riley affinity >45, otherwise affinity -5)
2. "Small group gathering" → Energy -12, Social +8, Money -$12
3. "Help someone in need" → Energy -10, Social +12, "Good karma"

[RANDOM EVENT - Positive (40% chance)]:
"FRIENDS PLAN WEEKEND GETAWAY!"
→ Social +15, Energy -15, Money -$25
→ "Summer memories in the making"
```

## Scenario 4

```         
LOCATION: Dorm Room
"Friday evening. You've found better balance this week."
[CHOICES]:
1. "Restorative activities" → Energy +25, Health +8, "Proper recovery"
2. "Weekend preparation" → Energy -15, All stats +4 (strategic planning)
3. "I need money so let's invest in some stocks" → Energy -10, Social +8, Money -$5
```

### Choice 1 - "Restorative activities"

```         
LOCATION: Dorm Room
"Your body and mind thank you for the proper rest."

[If Health < 60]:
"Your body was desperately craving this recovery"
→ Health +12 bonus, Energy +15 bonus, "Healing properly"

[If multiple stats improved this week]:
"Balanced effort leads to balanced recovery"
→ All stats +2, "The perfect summer week"

[CHOICES]:
1. "Yoga and meditation" → Energy +20, Health +6, Academic +2
2. "Quality sleep" → Energy +30, Health +8, "Deep restoration"
3. "Nature walk" → Energy +15, Health +5, Social +3

[RANDOM EVENT - Positive (35% chance)]:
"PERFECT RECOVERY DAY!"
→ Health +10, Energy +20, All stats +3
→ "Sometimes doing less accomplishes more"
```

### Choice 2 - "Weekend preparation"

```         
LOCATION: Dorm Desk  
"You plan the perfect balanced weekend."

[If any stat < 45]:
"You identify exactly what needs focus this weekend"
→ That stat +10 bonus if focused this weekend

[If all stats > 55]:
"You're maintaining impressive all-around progress"
→ All stats +3, "The balanced life continues"

[CHOICES]:
1. "Productivity focus" → Energy -12, Academic +8, Money +$20
2. "Social focus" → Energy -10, Social +10, "Connection planning"
3. "Health focus" → Energy -8, Health +6, Fitness +4

[RANDOM EVENT - Money Opportunity (30% chance)]:
"WEEKEND GIG PAYS DOUBLE!"
→ Money +$80, Energy -25, "Short-term sacrifice for cash"
```

### Choice 3 - "Stock investing"

```         
LOCATION: Dorm
"Let us see... What should I invest in?"

[CHOICES]:
1. "Mag7" → Money +30 (80%), Money -5 (20%)
2. "Small cap tech stock" → Money +200 (10%), Money -50 (90%)
3. "SPY ETF" → Money +20
```

## [END OF WEEK 5 EVENTS]

### Weekly Update

```         
"WEEK 5 COMPLETE: Summer halfway point reached!"
[EVENT]:
- If employed: "Regular paycheck stability!" → Money +$80-120
```

### Stat Decay

```         
"End of Week 5 - Better balance reduces decay:"
- Health: -4 (managed stress)
- Social: -5 (maintained connections)
- Academic: -6 (consistent learning)
- Fitness: -5 (sustainable training)
- Energy: -8 (summer fatigue accumulation)
```

### Additional Random Events

```         
[RANDOM EVENT 1 - Positive (25% chance)]:
"SUMMER JOB BONUS!"
→ Money +$200, Social +3

[RANDOM EVENT 2 - Relationship Boost (30% chance)]:
"DEEPENING FRIENDSHIP!"
→ Highest affinity relationship: +10 affinity
→ "Summer connections becoming meaningful"

[RANDOM EVENT 3 - Money Opportunity (20% chance)]:
"SKILL-BASED FREELANCE WORK!"
→ Money +$100
→ "Your specialized skills are valuable"
```

# WEEK 6: LOVE TRIANGLES & DRAMA

## Scenario 1

```         
LOCATION: Dorm Room
"Monday morning. Your phone blows up with messages from multiple people..."
[CHOICES]:
1. "Gym with Alex" → Energy -20, Fitness +8
2. "Study session with Sam" → Energy -18, Academic +12  
3. "Coffee date with Riley" → Energy -12, Social +10, Money -$6
```

### Choice 1 - "Gym with Alex"

```         
LOCATION: Gym
"Alex is waiting for you, but Sam unexpectedly shows up too."
ALEX: "Hey, ready for our workout? Who's this guy?"
SAM: "I thought we could study after your workout? But I can join you guys if that's cool?"

[CHOICES]:
1. "Stick with Alex" → Fitness +8, Alex affinity: +10, Sam affinity: -8
2. "Invite Sam to join" → Fitness +5, Alex affinity: -5, Sam affinity: +8, "Awkward three-way workout"
3. "Bail on both" → Energy +10, Alex affinity: -10, Sam affinity: -10, "Too much pressure"

[Drama]:
"COMPETITIVE TENSION ERUPTS!"
→ Alex and Sam start subtly competing for your attention
→ Whoever you choose gets +12 affinity, the other gets -15 (50-50 chance)
→ "Is it just ego or is it something else?"
```

### Choice 2 - "Study session with Sam"

```         
LOCATION: Library
"Sam has books spread everywhere, but Riley shows up looking for you."

SAM: "I reserved this study room for us... Riley?"

RILEY: "Hey! I was hoping to catch you before my shift. Got a minute?"

[CHOICES]:
1. "Study with Sam" → Academic +12, Sam affinity: +10, Riley affinity: -10
2. "Quick chat with Riley" → Social +8, Riley affinity: +8, Sam affinity: -12
3. "Try to include both" → Academic +5, Social +5, Both affinities: -25, "Don't tryna please everyone cus you end up pleasing none."

[RANDOM EVENT - Positive (30% chance)]:
"UNEXPECTED STUDY GROUP FORMS!"
→ Academic +15, Social +8, Both affinities: +25
→ "Turns out group studying is actually effective"
```

### Choice 3 - "Coffee date with Riley"

```         
LOCATION: Campus Cafe
"Riley has your usual ready, but Alex texts they're nearby and wants to join."
RILEY: "I was hoping we could have some one-on-one time... but if Alex wants to come..."

[CHOICES]:
1. "Keep it just us" → Social +10, Riley affinity: +12, Alex affinity: -8
2. "Invite Alex over" → Social +6, Riley affinity: -5, Alex affinity: +8
3. "Reschedule both" → Energy +15, Both affinities: -30 "BRUH... The developers are disappointed in you"

[RANDOM EVENT - Drama (35% chance)]:
"AWKWARD SILENCE!"
→ The person you didn't choose shows up anyway
→ Social -5, Both affinities: -15, "This is so uncomfortable"
```

## Scenario 2

```         
LOCATION: Various
"Midday chaos!"
[CHOICES]:
1. "Campus event with multiple friends" → Energy -15, Social +15, "Group dynamics"
2. "Quiet escape alone" → Energy +20, All stats +2, "Mental health break"  
3. "Help someone in crisis" → Energy -25, Random relationship +20, "Hero moment"
```

### Choice 1 - "Campus event with multiple friends"

```         
LOCATION: Campus Quad
"Alex, Sam, and Riley are all at the summer festival. They keep glancing your way."

[CHOICES]:
1. "Hang with Alex" → Social +8, Alex affinity: +12, Others: -10
2. "Study corner with Sam" →Social +6, Sam affinity: +12, Others: -10  
3. "Food trucks with Riley" → Social +10, Riley affinity: +12, Others: -10
4. "Try to rotate between all" → Social +5, All affinities: +1, "Exhausting but diplomatic"
```

### Choice 2 - "Quiet escape alone"

```         
LOCATION: Hidden Campus Spot
"You find a secret garden nobody knows about. Perfect solitude."

[If Social > 70]:
"Your social battery is completely drained. This is necessary self-care."
→ Energy +25, Health +10, "Much needed recharge"

[If multiple relationships > 60]:
"You realize you've been spreading yourself too thin. Time to reflect."
→ All stats +3, "Clarity emerges from solitude"
```

### Choice 3 - "Help someone in crisis"

```         
LOCATION: Various
"You get an urgent message from someone having a rough day."

[RANDOM ROLL for who needs help]:
- 33% Alex: "Injured during workout, needs help getting to clinic"
- 33% Sam: "Academic meltdown before big exam"  
- 33% Riley: "Family emergency, needs emotional support"

[CHOICES]:
1. "Drop everything and help" → Energy -30, Their affinity: +20
2. "Send help but can't come" → Energy -10, Their affinity: +5
3. "Ignore the message" → Energy +5, Their affinity: -40
```

## Scenario 3

```         
LOCATION: Various  
"Afternoon decisions get more intense as feelings surface."
[CHOICES]:
1. "Fitness competition with Alex" → Energy -25, Fitness +12, "Show off your gains"
2. "Academic decathlon with Sam" → Energy -22, Academic +15, "Brain battle"  
3. "Double date situation" → Energy -18, Social +15, Money -$15, "Romantic tension"
```

### Choice 1 - "Fitness competition with Alex"

```         
LOCATION: Gym Competition
"Alex and you signed up for a fitness challenge, and you invited Sam and Riley to both show up to cheer you on."

[CHOICES]:
1. "Focus only on Alex" → Fitness +12, Alex affinity: +15, Others: -8
2. "Acknowledge the crowd" → Fitness +8, Social +5, Alex affinity: -10
3. "Try to impress everyone" → Fitness +5, Social +8, All affinities: +1
```

### Choice 2 - "Hackathon with Sam"

```         
LOCATION: Hackathon Competition  
"You and Sam make an unbeatable team, but distractions abound."

SAM: "I know we can win this if we focus. Ignore everything else."

[If multiple relationships > 50]:
"You keep getting texts from your other friends, Sam notices your phone buzzing every 2 seconds."
"Your fan club is getting distracting."

[CHOICES]:
1. "Hyper-focus with Sam" → Academic +15, Sam affinity: +15, Others: -10
2. "Silence the sound but keep the notification anyways" → Academic +5, Social +5, Sam affinity: -8
3. "Do nothing" → Sam affinity: -15

[RANDOM EVENT - Positive (35% chance)]:
"YOU WIN!"
→ Money +$200, Academic +10, Sam affinity: +20
→ "Brain power pays off literally"
```

### Choice 3 - "Double date situation"

```         
[This choice is unavailable unless player has over 65 affinity points for over 2 relationship, if unavailable print "Sorry this option is not available to you because noone likes you like that yet."]

LOCATION: Fancy Restaurant
"You agreed to a group date, but it's clearly two people competing for you."

[WHO SHOWS UP? Based on highest affinities]:
- Alex vs Sam: "Brain vs Brawn date night"
- Alex vs Riley: "Athlete vs Artist tension"  
- Sam vs Riley: "Scholar vs Socialite showdown"

[CHOICES]:
1. "Flirt with person A" → Social +10, Person A affinity: +15, Person B: -15
2. "Flirt with person B" → Social +10, Person B affinity: +15, Person A: -15
3. "Play neutral" → Social +5, Both affinities: -5, "Nobody feels special"
```

## Scenario 4

```         
LOCATION: Dorm Room
"Friday night. Time to make some relationship decisions before the weekend."
[CHOICES]:
1. "Confront the love triangle" → Energy -20, Force relationship choices
2. "Avoid everyone and game" → Energy -10, Social +5 (online), Jordan affinity: +10  
```

### Choice 1 - "Confront the love triangle"

```         
LOCATION: Your Dorm

[If multiple affinities > 65]:
"This is going to be painful but I need to focus."

[If all affinities < 65]:
"Sorry this option is not available to you because noone likes you like that yet."

[if only one affinity >65]:
"No need for confrontation. You clearly has someone in mind."

[CHOICES]:
1. "Choose Alex" → Alex affinity: +25, Others: -40, Fitness +5
2. "Choose Sam" → Sam affinity: +25, Others: -40, Academic +5  
3. "Choose Riley" → Riley affinity: +25, Others: -40, Social +5
4. "Propose polyamory" → 5% chance success: All affinities +30, otherwise: All affinities -50

"You proceeded to distant yourself from the rest of them."
```

### Choice 2 - "Avoid everyone and game"

```         
LOCATION: Dorm Room
"You log into your favorite game. Jordan is your safe space."

JORDAN: "Rough week? Let's forget real life for a few hours."

[RANDOM EVENT - Positive (45% chance)]:
"GAMING MARATHON TURNS THERAPEUTIC!"
→ Jordan gives surprisingly good relationship advice
→ Social +12, Jordan affinity: +20, "Who knew gamers were so wise?"
```

## [END OF WEEK 6 EVENTS]

### Weekly Update

text

```         
"WEEK 6 COMPLETE: Relationship drama reached peak levels!"
[EVENT]:
- If chose one person: "You've made your choice! Relationship solidified." → That affinity +15
- If remained single: "Playing the field has its perks!" → Social +10
- If caused major drama: "Campus is talking about your love life..." → Social +15, Random affinity -20
- If avoided everything: "Sometimes the wisest choice is no choice." → Energy +20
```

### Stat Decay

```         
"End of Week 6 - Emotional exhaustion takes its toll:"
- Health: -6 (stress eating/sleep)
- Social: -8 (drama drain)
- Academic: -7 (distracted studying)
- Fitness: -6 (skipped workouts)
- Energy: -20 (emotional labor)

[RELATIONSHIP STATUS UPDATE]:
- If any affinity > 80: "Serious romance blooming!"
- If multiple affinities > 70: "Love triangle continues..."
- If all affinities < 40: "Burned all your bridges"
```

### Additional Random Events

```         
[RANDOM EVENT 1 - Negative]:
"FELL ILL!"
→ Health -30, Money -$120

[RANDOM EVENT 2 - Money Opportunity (30% chance)]:
"DRAMA-FREE FREELANCE WORK!"
→ Take a job that has nothing to do with your love life
→ Money +$80, Energy -15, "Finally, something simple"
```

# WEEK 7: AFTERMATH & CONSEQUENCES

## Scenario 1
LOCATION: Dorm Room
"Monday morning. The fallout from last week is still fresh."
[CHOICES]:
1. "Spend time with your chosen partner" (only if you chose one person) → Energy -20, Affinity +15, Social +10
2. "Enjoy single life freedom" (if remained single) → Energy -15, Social +12, Money -10
3. "Deal with campus gossip" (if caused major drama) → Energy -25, Social -5, Random affinity -10
4. "Recharge alone" (if avoided everything) → Energy +25, Health +10

### Choice 1 - "Spend time with your chosen partner"
LOCATION: Partner’s Spot
"You meet up with your chosen partner to start the week together."
[CHOICES]:
1. "Go for breakfast together" → Social +10, Affinity +15, Money -$15
2. "Workout/study together" → Fitness +8 or Academic +10, Affinity +12, Energy -20
3. "Skip plans and just hang out" → Energy +10, Affinity +8

### Choice 2 - "Enjoy single life freedom"
LOCATION: Campus Grounds
"You decide to embrace being single and free."
[CHOICES]:
1. "Flirt casually with someone new" → Social +12, Random affinity +10, Energy -15
2. "Go shopping alone" → Money -$30, Social +5, Energy -10
3. "Relax solo at a café" → Energy +20, Social +3, Money -10

### Choice 3 - "Deal with campus gossip"
LOCATION: Hallway
"Whispers follow you everywhere after last week’s drama."
[CHOICES]:
1. "Confront the gossip directly" → Social +5, Random affinity +10, Energy -20
2. "Laugh it off publicly" → Social +10, Random affinity +5, Energy -15
3. "Hide from everyone" → Energy +25, Social -10

### Choice 4 - "Recharge alone"
LOCATION: Dorm Room
"You decide to ignore everyone and focus on yourself."
[CHOICES]:
1. "Meditate and journal" → Health +10, Energy +20
2. "Cook a healthy meal" → Health +12, Money -15, Energy +10
3. "Sleep in all day" → Energy +35, Health +5

## Scenario 2 — Attempted Polyamory
LOCATION: Campus Café
"Word spreads that you’re trying to balance multiple relationships at once. Alex, Sam, and Riley all show up."
[CHOICES]:
1. "Propose polyamory openly" → Risky attempt, Energy -25, All affinities ± depending on reaction
2. "Try to juggle conversations" → Social +10, Random affinity +5, Energy -20
3. "Back out awkwardly" → Social -5, Random affinity -10, Energy +15

### Choice 1 - "Propose polyamory openly"
LOCATION: Café Table
"You explain your idea of being with more than one person honestly."
[CHOICES]:
1. "Make a heartfelt case" → 20% chance success: All affinities +20, Social +15; otherwise: All affinities -30
2. "Frame it as casual fun" → Riley affinity -15, Alex affinity -10, Sam affinity -10, Social +5
3. "Apologize mid‑speech" → Energy +10, All affinities -5

### Choice 2 - "Try to juggle conversations"
LOCATION: Café
"You bounce between Alex, Sam, and Riley, trying to keep everyone happy."
[CHOICES]:
1. "Focus more on Alex" → Alex affinity +15, Sam affinity -10, Riley affinity -10
2. "Focus more on Sam" → Sam affinity +15, Alex affinity -10, Riley affinity -10
3. "Focus more on Riley" → Riley affinity +15, Alex affinity -10, Sam affinity -10

### Choice 3 - "Back out awkwardly"
LOCATION: Café Exit
"You panic and leave suddenly."
[CHOICES]:
1. "Send apology texts later" → Random affinity +5, Social -5
2. "Ignore everyone" → All affinities -10, Energy +20
3. "Distract yourself with Jordan online" → Jordan affinity +10, Social +5, Energy -15

## Scenario 3
LOCATION: Various
"Afternoon choices reflect your summer trajectory."
[CHOICES]:
1. "Couple workout/study session" (partner path) → Fitness +8 or Academic +10, Affinity +12
2. "Party hopping" (single path) → Social +15, Money -30, Energy -20
3. "Damage control with friends" (drama path) → Social +10, Random affinity +8, Energy -15
4. "Solo meditation" (avoidance path) → Health +12, Energy +25

### Choice 1 - "Couple workout/study session"
LOCATION: Gym/Library
"You and your partner commit to growth together."
[CHOICES]:
1. "Push each other hard" → Fitness +10 or Academic +15, Affinity +12, Energy -25
2. "Take it easy together" → Fitness +5 or Academic +8, Affinity +8, Energy -10
3. "Skip halfway" → Energy +15, Affinity -5

### Choice 2 - "Party hopping"
LOCATION: Off‑Campus Party
"You bounce between parties, enjoying the single life."
[CHOICES]:
1. "Dance all night" → Social +20, Energy -30, Money -20
2. "Drink heavily" → Social +10, Health -15, Energy -20
3. "Leave early" → Energy +20, Social +5

### Choice 3 - "Damage control with friends"
LOCATION: Common Area
"You try to repair relationships after the drama."
[CHOICES]:
1. "Apologize sincerely" → Random affinity +15, Social +10, Energy -15
2. "Deflect blame" → Random affinity -10, Social -5, Energy +10
3. "Offer favors" → Money -20, Random affinity +10, Social +5

### Choice 4 - "Solo meditation"
LOCATION: Dorm Room
"You focus inward, ignoring external chaos."
[CHOICES]:
1. "Deep breathing exercises" → Health +12, Energy +20
2. "Yoga session" → Fitness +8, Health +5, Energy +15
3. "Fall asleep mid‑meditation" → Energy +30, Health +5

## Scenario 4
LOCATION: Dorm Room
"Friday night. The week’s choices weigh on you."
[CHOICES]:
1. "Deep talk with partner" → Affinity +20, Social +10
2. "Casual gaming with Jordan" (single path) → Social +8, Jordan affinity +10
3. "Distract yourself with work" (drama path) → Academic +12, Energy -20
4. "Early sleep" (avoidance path) → Energy +30, Health +10

### Choice 1 - "Deep talk with partner"
LOCATION: Partner’s Room
"You open up about your feelings."
[CHOICES]:
1. "Confess deeper emotions" → Affinity +20, Social +10
2. "Keep it lighthearted" → Affinity +10, Social +5, Energy +10
3. "Avoid serious topics" → Affinity -5, Social -5, Energy +20

### Choice 2 - "Casual gaming with Jordan"
LOCATION: Online
"You log in with Jordan for a chill session."
[CHOICES]:
1. "Competitive mode" → Social +8, Jordan affinity +12, Energy -20
2. "Casual fun" → Social +5, Jordan affinity +8, Energy -10
3. "Quit early" → Energy +20, Jordan affinity -5

### Choice 3 - "Distract yourself with work"
LOCATION: Library
"You bury yourself in assignments to avoid drama."
[CHOICES]:
1. "Full focus" → Academic +15, Energy -25
2. "Half‑hearted effort" → Academic +8, Energy -10
3. "Give up quickly" → Academic -5, Energy +20

### Choice 4 - "Early sleep"
LOCATION: Dorm Room
"You decide to rest instead of engaging."
[CHOICES]:
1. "Sleep immediately" → Energy +30, Health +10
2. "Read before bed" → Academic +5, Energy +20
3. "Scroll phone until sleep" → Energy +15, Social +2
## [END OF WEEK 7 EVENTS]

### Weekly Update

```
WEEK 7 COMPLETE: "The consequences of your Week 6 choices are clear."
[EVENT]:
- Partner path: "Your relationship deepens." → Affinity +20
- Single path: "You’re thriving socially." → Social +15
- Drama path: "Campus whispers continue." → Social -5, Random affinity -10
- Avoidance path: "You feel rested but disconnected." → Health +10, Social -5

### Stat Decay

``` 
- Health: -8
- Social: -10
- Academic: -10
- Fitness: -9
- Energy: -12

### Additional Random Events

[Random Event 1 - Positive, Partner Path]:
"COUPLE SPOTLIGHT ON CAMPUS!"
→ Social +10, Affinity +15
→ "People admire your relationship, boosting your confidence together."

[Random Event 2 - Neutral, Single Path]:
"NEW FRIEND GROUP INVITE."
→ If you join: Social +12, Random affinity +8, Energy -15
→ If you decline: Social -5, Energy +10
→ "Being single means more freedom to choose your circles."

[Random Event 3 - Negative, Drama Path]:
"RUMOR ESCALATES!"
→ Social -10, Random affinity -15, Energy -20
→ "The gossip spreads further, making it harder to focus."

[Random Event 4 - Positive, Avoidance Path]:
"PEACEFUL CAMPUS WALK."
→ Health +10, Energy +20
→ "Your solitude pays off with clarity and calm."

[Random Event 5 - Neutral, Partner Path]:
"MINOR ARGUMENT WITH PARTNER."
→ If resolved quickly: Affinity +5, Social +5
→ If ignored: Affinity -10, Energy +10
→ "Relationships aren’t always smooth sailing."

[Random Event 6 - Positive, Single Path]:
"FREE ENTRY TO A CLUB NIGHT!"
→ Social +15, Money +20 (saved), Energy -15
→ "Single perks — you get invited everywhere."

[Random Event 7 - Negative, Avoidance Path]:
"FEELING LEFT OUT."
→ Social -10, Health -5
→ "Isolation starts to weigh on you emotionally."
*Attach random probabilities to each random event*

# WEEK 8: ENDGAME & RESOLUTION

## Scenario 1
LOCATION: Library
"Final exams week. Your focus depends on your path."
[CHOICES]:
1. "Study with partner" (partner path) → Academic +20, Affinity +10, Energy -25
2. "Solo cram session" (single/avoidance path) → Academic +25, Energy -30
3. "Distracted by drama" (drama/polyamory path) → Academic +10, Social -5, Energy -20

### Choice 1 - "Study with partner"
LOCATION: Library Study Room
"You and your partner sit side by side, motivating each other."
[CHOICES]:
1. "Push through all night" → Academic +30, Affinity +15, Energy -40
2. "Balanced review" → Academic +20, Affinity +10, Energy -25
3. "Get distracted chatting" → Academic +10, Affinity +12, Energy -15

### Choice 2 - "Solo cram session"
LOCATION: Dorm Desk
"You grind through textbooks alone."
[CHOICES]:
1. "Go all night" → Academic +25, Energy -35, Health -10
2. "Take breaks" → Academic +15, Energy -20, Health +5
3. "Give up halfway" → Academic -10, Energy +20

### Choice 3 - "Distracted by drama"
LOCATION: Library
"Your phone keeps buzzing with messages about last week’s chaos."
[CHOICES]:
1. "Ignore the drama" → Academic +15, Social -5, Energy -20
2. "Respond to everyone" → Social +10, Academic -5, Energy -25
3. "Vent online" → Social +5, Jordan affinity +10, Academic -10

## Scenario 2
LOCATION: Campus Quad
"The big end‑of‑summer party arrives. Everyone is there."
[CHOICES]:
1. "Attend with partner" → Social +20, Affinity +15, Money -40
2. "Go wild single" → Social +25, Money -50, Energy -20
3. "Show up despite drama/polyamory tension" → Social +10, Random affinity ±, Money -20
4. "Skip party" (avoidance path) → Energy +30, Social -10

### Choice 1 - "Attend with partner"
LOCATION: Party Grounds
"You and your partner arrive together, drawing attention."
[CHOICES]:
1. "Dance together all night" → Social +20, Affinity +20, Energy -30
2. "Stay low‑key" → Social +10, Affinity +10, Energy -15
3. "Leave early" → Affinity +5, Social +5, Energy +20

### Choice 2 - "Go wild single"
LOCATION: Party Grounds
"You embrace the chaos of being single."
[CHOICES]:
1. "Flirt with multiple people" → Social +20, Random affinity +10, Energy -25
2. "Drink heavily" → Social +10, Health -15, Energy -20
3. "Dance until collapse" → Social +25, Energy -40, Health -10

### Choice 3 - "Show up despite drama/polyamory tension"
LOCATION: Party Grounds
"Alex, Sam, and Riley all notice you at once."
[CHOICES]:
1. "Try to balance everyone" → 20% success: All affinities +10, otherwise: All affinities -20
2. "Focus on one person" → That affinity +15, others -10
3. "Avoid them entirely" → Social -5, Energy +20

### Choice 4 - "Skip party"
LOCATION: Dorm Room
"You stay in, resting instead of partying."
[CHOICES]:
1. "Sleep early" → Energy +30, Health +10
2. "Study quietly" → Academic +10, Energy -15
3. "Game with Jordan" → Social +10, Jordan affinity +12, Energy -15

## Scenario 3
LOCATION: Dorm Room
"Final weekend. Time to reflect on your summer."
[CHOICES]:
1. "Future planning with partner" → All stats +5, Affinity +10
2. "Set solo goals" (single path) → All stats +8, Energy -15
3. "Try to repair drama/polyamory damage" → Social +10, Random affinity +10, Energy -20
4. "Rest deeply" (avoidance path) → Energy +40, Health +15

### Choice 1 - "Future planning with partner"
LOCATION: Partner’s Room
"You and your partner talk about the future."
[CHOICES]:
1. "Plan next semester together" → Academic +10, Affinity +15
2. "Plan fitness goals together" → Fitness +10, Affinity +15
3. "Plan social adventures" → Social +10, Affinity +15

### Choice 2 - "Set solo goals"
LOCATION: Dorm Desk
"You write down your ambitions for the next year."
[CHOICES]:
1. "Academic focus" → Academic +15, Energy -20
2. "Fitness focus" → Fitness +15, Energy -20
3. "Social focus" → Social +15, Energy -20

### Choice 3 - "Try to repair drama/polyamory damage"
LOCATION: Café
"You reach out to those you hurt."
[CHOICES]:
1. "Apologize sincerely" → Random affinity +15, Social +10, Energy -15
2. "Offer favors" → Money -20, Random affinity +10, Social +5
3. "Deflect blame again" → Random affinity -10, Social -5, Energy +10

### Choice 4 - "Rest deeply"
LOCATION: Dorm Room
"You prioritize recovery above all else."
[CHOICES]:
1. "Sleep long hours" → Energy +40, Health +15
2. "Meditate deeply" → Health +10, Energy +20
3. "Do nothing at all" → Energy +25, Social -5

## Scenario 4
LOCATION: Campus Quad
"Sunday evening. The summer ends."
[CHOICES]:
1. "Farewell with partner" → Affinity +20, Social +10
2. "Farewell with friends" (single path) → Social +15, Random affinity +10
3. "Farewell awkwardly" (drama/polyamory path) → Social -5, Random affinity -10
4. "Farewell quietly" (avoidance path) → Energy +20, Social -5

### Choice 1 - "Farewell with partner"
LOCATION: Gym/Library/Café
"You share a final moment with your partner."
[CHOICES]:
1. "Promise to stay in touch" → Affinity +20, Social +10
2. "Make future plans" → Affinity +15, Academic +5
3. "Keep it casual" → Affinity +10, Energy +10

### Choice 2 - "Farewell with friends"
LOCATION: Common Area
"You gather your closest friends for a goodbye."
[CHOICES]:
1. "Host a small dinner" → Social +15, Money -30
2. "Play games together" → Social +10, Jordan affinity +10
3. "Quick hugs and goodbyes" → Social +5, Energy +15

### Choice 3 - "Farewell awkwardly"
LOCATION: Campus Quad
"You bump into Alex, Sam, and Riley at once."
[CHOICES]:
1. "Try to smooth things over" → Random affinity +5, Social +5
2. "Say nothing" → Social -5, Random affinity -10
3. "Leave abruptly" → Energy +20, Social -10

### Choice 4 - "Farewell quietly"
LOCATION: Dorm Room
"You avoid everyone and reflect alone."
[CHOICES]:
1. "Write in journal" → Academic +5, Energy +10
2. "Sleep early" → Energy +25, Health +10
3. "Watch a movie alone" → Energy +15, Social -5

## [END OF WEEK 8 EVENTS]

### Weekly Update

WEEK 8 COMPLETE: "Summer concludes — the final outcomes of your choices are revealed."
[EVENT]:
- Partner path: "You leave with a strong bond." → Affinity +25
- Single path: "You leave with wide social circles." → Social +20
- Drama/polyamory path: "You leave with mixed reputations." → Social -10, Random affinity -15
- Avoidance path: "You leave rested but isolated." → Health +15, Social -10

### Stat Decay

- Health: -12
- Social: -15
- Academic: -15
- Fitness: -15
- Energy: -20


### Additional Random Events (Week 8)

[Random Event 1 - Positive, Partner Path, 30% chance]:
"PARTNER SURPRISES YOU WITH A GIFT!"
→ Affinity +20, Social +10
→ "A thoughtful gesture strengthens your bond right before summer ends."

[Random Event 2 - Neutral, Single Path, 25% chance]:
"LAST-MINUTE FRIEND TRIP INVITE."
→ If you join: Social +15, Money -$40, Energy -20
→ If you decline: Social -5, Energy +15
→ "Single life means spontaneous opportunities."

[Random Event 3 - Negative, Drama/Polyamory Path, 20% chance]:
"CONFRONTATION AT THE PARTY!"
→ Social -10, Random affinity -20, Energy -25
→ "Tensions boil over in front of everyone."

[Random Event 4 - Positive, Avoidance Path, 30% chance]:
"PEACEFUL STUDY NIGHT."
→ Academic +10, Energy +20
→ "Your solitude pays off with quiet productivity."

[Random Event 5 - Neutral, Partner Path, 25% chance]:
"PARTNER WANTS TO TALK ABOUT THE FUTURE."
→ If you engage: Affinity +15, Academic +5
→ If you dodge: Affinity -10, Energy +10
→ "End-of-summer conversations can be heavy."

[Random Event 6 - Positive, Single Path, 30% chance]:
"FREE CONCERT TICKETS!"
→ Social +20, Energy -15, Money +30 (saved)
→ "You and your friends enjoy one last big night out."

[Random Event 7 - Negative, Avoidance Path, 20% chance]:
"FEELING REGRET."
→ Social -10, Health -5
→ "You wonder if avoiding everyone was the right choice.

## CLOSING SEQUENCE

[SCENE: Campus Quad, sunset of the final summer day]
"The semester is over. Your summer journey has reached its end."

[FINAL CHECK]
• Did you survive all 12 weeks without hitting critical warnings?
• Did you balance your stats and relationships?
• Did you achieve your chosen path’s ending?

[RESULTS APPEAR]

"WELCOME TO SUMMER MAXXING — FINAL REPORT!"

YOUR PERFORMANCE:
• ⚡ ENERGY: [Final Value]
• ❤️ HEALTH: [Final Value]
• 👥 SOCIAL: [Final Value]
• 📚 ACADEMIC: [Final Value]
• 💪 FITNESS: [Final Value]
• 💰 MONEY: [Final Value]
• RELATIONSHIPS: [Affinity Scores]

PASS / FAIL CONDITIONS:
🚨 If HEALTH ≤ 0: "You collapsed from illness. GAME OVER."
😴 If ENERGY ≤ 0: "You collapsed from exhaustion. GAME OVER."
💸 If MONEY ≤ 0: "Financial crisis hit. GAME OVER."
✅ Otherwise: "You survived the summer!"

*Only Recieve a rating if they have passed*
ENDING RATING (Stars out of 5):
5/5 "Legendary Summer!" — You mastered balance, thrived socially, and left with strong bonds.
4/5 "Great Summer!" — You did well, with only minor struggles.
3/5 "Average Summer." — You survived, but missed key opportunities.
2/5 "Rough Summer." — You barely scraped by, stats suffered.
1/5  "Disaster Summer." — You failed to balance life, relationships, or finances.

[FINAL MESSAGE]
"Thank you for playing SUMMER MAXXING!"
"Your choices shaped your destiny. Every summer tells a different story."
[Press any key to exit...]
