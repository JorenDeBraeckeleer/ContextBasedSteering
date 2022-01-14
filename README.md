#Context based steering

##Basic steering behaviour
To start of simple we make a simple seek behaviour.
The agent will go to the marked place.

Once we add a flee, arrive and wander behaviour we can start adjusting those with context steering.

##Basic context steering
To start with the basics of context steering:
We cast rays around the agent, this amount can vary depending on the behaviour you want.
To make a simple example here, we cast 16 rays around the agent.

![ALT_TEXT](Screenshots/Agent_16Rays.png?raw=true "Agent_16Rays")

Those rays combined will give us our final steering, but how do we get this...

We create a vectors, with a size of the amount of rays, and initialize them with 0's at the start.

The first vector we create is for the direction of the rays:
-Calculate the angle between every ray and assign the final direction of every ray to the vector.

The second vector is for our positive values or 'interests':
-Our interest is the position we want to move to.
-With our direction we are interested in and the direction of the ray we can calculate our interest for every ray.

![ALT_TEXT](Screenshots/Agent_PosRays.png?raw=true "Agent_PosRays")

The third and final vector is for our negative values or 'dangers':
-Our dangers can be obstacles, like we will use for the example, or many other things like enemies etc.
-If an obstacle intersects with a ray we can assign a value to the ray.
-This value, which means 'danger', will nullify the ray in the positive vector because something is blocking our way.

![ALT_TEXT](Screenshots/Agent_NegRays.png?raw=true "Agent_NegRays")

To get our final direction we add all the remaining positive rays and apply this direction to our agent.

![ALT_TEXT](Screenshots/Agent_FinalDirection.png?raw=true "Agent_FinalDirection")

##Experimental context steering
I tried improving the algorithm to get a closer result to the behaviour I was after.

The vector with direction rays stays the same, but we change our 'interests' and 'dangers' into 1 weight vector.
A ray in the weight vector has a default value. This default value can vary depending on what behaviour you are after:
-Changing the default value to the speed you are travelling at will cause better response time when going fast and slower response time when going slow, which mimics a much more natural feeling of avoiding an obstacle.
-Changing it to a larger value will cause the agent to start dodging an obstacle earlier, a smaller value will to the opposite.

Weights: 'interests' make the ray longer (total direction will be closer to this one), 'dangers' make the ray shorter (total direction will be further away from this one).
If a ray finds an obstacle we also give a notification to the neighbouring rays, those will shrink aswell to focus on avoiding the obstacle.

To make the code a bit more visible I added some colorful debugging options:

Untill this point we did everything with a seek behaviour, let's change this up and add a wander:
-We change our 'interest' to a random point, this means that the agent will wander through the level but when it notices an obstacle the agent will move around it and not go straight into a wall.

We can do this aswell with flee:
-Positve weights in the opposite direction from a specific point

For arrive:
-Same as seek but change velocity when approaching the 'seek' point

##Sumarry
We covered 4 basic steering behaviours and implemented those with context steering.
-Context steering is very useful and can be combined with other steering behaviours, it's definitely a good option if you have a continuously changing environment, for static environments there are better options out there.
-I didn't expect to find as much useful cases as I did, with a bit more time I would definitely like to add a way to move from one location to another while keeping hold of the context (like going around a race track).


This project used the Elite framework by Matthieu Delaere, Thomas Goussaert, Andries Geens and Yosha Vandaele.
