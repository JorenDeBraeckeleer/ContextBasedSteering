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

1. We adapt our ray length on variables.
    - More natural response.

![ALT_TEXT](/Media/DefaultRayLength.gif?raw=true "Context_StartRayLength")
> Ray length depends on velocity.

2. Counter steering when encountering 'danger'.
    - Better avoidance of obstacle.

![ALT_TEXT](/Media/CounterSteering.gif?raw=true "Context_CounterSteering")
> Counter steering for better obstacle avoidance.

3. We change 'interests' and 'dangers' to 1 'weights' vector.
    - Allows for better control over the length of rays.

4. Influencing neighbouring rays.
    - Neighbours gain or lose length depending on what neighbouring rays encounter.

![ALT_TEXT](/Media/WeightSteering.gif?raw=true "Context_WeightSteering")
> Weight steering with neighbour influence. (Red is short or low influence, green is long or high influence.)

### Adding different behaviours
1. Wander.
    - We change our 'interest' to a random point.

![ALT_TEXT](/Media/Wander.gif?raw=true "Context_Wander")
> Wander behaviour with context steering.

2. Flee.
    - Opposite from seek.

![ALT_TEXT](/Media/Flee.gif?raw=true "Context_Flee")
> Flee behaviour with context steering.

## Sumarry
- Context steering is very useful and can be easily be combined with other steering behaviours.
- It's a good behaviour if you have a continuously changing environment, for static environments there are better options out there.
- If I have more time I would definitely like to add a way to move from one location to another (like going around a race track).

## End
*This project used the Elite framework by Matthieu Delaere, Thomas Goussaert, Andries Geens and Yosha Vandaele.*

## Editor's note
> Project is in Master branch, add the lib and include folders to get the project working (first time with GitHub, I don't what what I did wrong 😅)
