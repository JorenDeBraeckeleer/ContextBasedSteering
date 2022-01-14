# Context based steering

## Basic steering behaviour
**Start:** we make a simple 'seek' behaviour.
- The agent moves to the marked location.

![ALT_TEXT](/Media/Seek.gif?raw=true "SeekBehaviour")

## Basic context steering
1. Casting directional rays.
    - We cast rays outwards from the agent.
    - The amount of rays can vary depending on the wanted behaviour.

![ALT_TEXT](/Media/DirectionalRays.png?raw=true "Context_DirectionalRays")
> A simple example with 16 rays.

2. Create a vector for positive values or 'interests'.
    - Interests are postitions we want to move to.
      - ```ray = dot(direction, postiveRay);```

![ALT_TEXT](/Media/PositiveRays.png?raw=true "Context_PositiveRays")
> Interest is in front of the agent.

3. Create a vector for negative values or 'dangers'.
    - Dangers can be obstacles, enemies, forbidden zones etc.
      - ```negativeRay = isIntersectingDanger ? 1 : 0;```
    - Negative values negate positive values.
      - ```positiveRay = negativeRay > 0 ? 0 : postiveRay;```

![ALT_TEXT](/Media/NegativeRays.png?raw=true "Context_NegativeRays")
> Rays intersecting with the obstacle aren't used.

4. Calculate final direction with remaining 'interests'.
    - ```steering += direction * positiveRay;```

![ALT_TEXT](/Media/FinalDirection.gif?raw=true "Context_FinalSteering")
> Blue ray is the final direction.

## Experimental context steering
I tried improving the algorithm to get a closer result to the behaviour I was after.

The vector with direction rays stays the same, but we change our 'interests' and 'dangers' into 1 weight vector.
A ray in the weight vector has a default value. This default value can vary depending on what behaviour you are after:
-Changing the default value to the speed you are travelling at will cause better response time when going fast and slower response time when going slow, which mimics a much more natural feeling of avoiding an obstacle.
-Changing it to a larger value will cause the agent to start dodging an obstacle earlier, a smaller value will to the opposite.

![ALT_TEXT](/Media/DefaultRayLength.gif?raw=true "Context_StartRayLength")
> Ray length depends on velocity.

![ALT_TEXT](/Media/CounterSteering.gif?raw=true "Context_CounterSteering")
> Counter steering for better obstacle avoidance.

Weights: 'interests' make the ray longer (total direction will be closer to this one), 'dangers' make the ray shorter (total direction will be further away from this one).
If a ray finds an obstacle we also give a notification to the neighbouring rays, those will shrink aswell to focus on avoiding the obstacle.

![ALT_TEXT](/Media/WeightSteering.gif?raw=true "Context_WeightSteering")
> Blue ray is the final direction.

To make the code a bit more visible I added some colorful debugging options:

Untill this point we did everything with a seek behaviour, let's change this up and add a wander:
-We change our 'interest' to a random point, this means that the agent will wander through the level but when it notices an obstacle the agent will move around it and not go straight into a wall.

![ALT_TEXT](/Media/Wander.gif?raw=true "Context_Wander")
> Wander behaviour with context steering.

We can do this aswell with flee:
-Positve weights in the opposite direction from a specific point

![ALT_TEXT](/Media/Flee.gif?raw=true "Context_Flee")
> Flee behaviour with context steering.

For arrive:
-Same as seek but change velocity when approaching the 'seek' point

## Sumarry
We covered 4 basic steering behaviours and implemented those with context steering.
-Context steering is very useful and can be combined with other steering behaviours, it's definitely a good option if you have a continuously changing environment, for static environments there are better options out there.
-I didn't expect to find as much useful cases as I did, with a bit more time I would definitely like to add a way to move from one location to another while keeping hold of the context (like going around a race track).


This project used the Elite framework by Matthieu Delaere, Thomas Goussaert, Andries Geens and Yosha Vandaele.
