# First Person Shooter Game
## Based on OpenGL, Glew, Glue
### Jinglun Dong Lab7
### 2012/04/13

0. Command line arguments: 	-t 			terrainTextureIndex
							-p			planeTextureIndex
							-a			amount of spiders
							-m			amount of mines
							-s			global speed  (1 - 1000 is a reasonable range. There is no compile or run time error in this range. 
										However, it is too fast to see if there is any abnormal behavior between 100-1000 on my computer)
							-rs			if this flag is used, spiders will have random size. Default average size is 5.
							-aves		average spider size ( 3 - 10 is a reasonable range, default value is 5.0)
							-sp			display plane. If -sp is used the plane in Lab6 will be displayed. Make sure that do not use -p at the same time	
										Haven't fix the memory leaking problem. Recommend: do not turn this option on when testing others.
							-nf			Do not use flat height map. Did not modify this part.
							
	eg: -a 5 -s 10 -t file1 -p file2    (5 spiders with speed 10/1000, using texture file1 for terrain texture, file2 for plane texture)
		-s 1 -rs -aves 7 -m 15			(default 20 spiders with speed 1/1000, random spider size, with average size 7, 15 mines)				
	
1. The animation speed of spiders' legs is a function of their moving speed. 

2. Each spider has its own random speed which might be changed after redirection.

3. If spiders moving too fast, they may pass through mines without trigger them. Reason: to reduce time complexity, only check if a mine is triggered 
	every 25 or 50 frames.
	
4. Spiders wouldn't trigger a missile to explode. (If flying too slow you may see a missile fly through a spider).
	Reason: A missile only check if it hits the ground.
	
5. Utilize two arms to model the legs of spiders instead of three. Reason: it looks good enough with only two arms.

6. Always display mines for it is easy to check if they are triggered correctly.

7. You may have to decrease the height of plane to shot spiders. (Haven't made the shooting angle a function of plane's animation angle)

8. Didn't modify the code for height map that is not flat i.e. step 14. I will implement it later if it is very important (deduct lots of score...).

9. I implement spiders in different files. If it is not acceptable, I could put them into a single file.

10. Please put attached height map file "flatHeightMap" to "../data/"

