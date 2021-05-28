# BRICKS Arcade Game
**part 1 of tutorial with sdl2:
https://www.youtube.com/watch?v=z-o6nmrJbsE&ab_channel=AveryMakesGames

SDL - for handling low-level I/O


# ideas for alterations:

easy to implement:

- decrease size of paddle after certain lengths of time have passed

med to implement:

- make bricks [ROW *COL] into a vectors array so diff rows can have diff amounts of bricks


hard to implement:

- create powerups
-   each pup lasts for a specified time(5 sec for example)
-   each pup increases a stat (paddle speed for example)
-   struct powerUps_paddleSpeed
-   {
paddleSpeedInc;  // increment of new paddle speed
frameObtained;    // time when it was gotten(when brick was destroyed)
lifetime;         // hhow many seconds it lasts
}

-   have a class variable vector<powerUps_paddleSpeed>  // one vecor for ach type of power up
-   
 
