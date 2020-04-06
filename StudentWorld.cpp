#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"

#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{
}

//the following init___ functions each add one of their respective actor (___) to the m_actors list

void StudentWorld::initDirt(double X, double Y)
{
	m_Actors.push_back(shared_ptr<Actor>(new Dirt(X, Y, this)));
}

void StudentWorld::initPit(double X, double Y)
{
	m_Actors.push_back(shared_ptr<Actor>(new Pit(X, Y, this)));
}

void StudentWorld::initFood(double X, double Y)
{
	m_Actors.push_back(shared_ptr<Actor>(new Food(X, Y, this)));
}

void StudentWorld::initFlame(double X, double Y, int direction)
{
	m_Actors.push_back(shared_ptr<Actor>(new Flame(X, Y, direction, this)));
}

void StudentWorld::initSpray(double X, double Y, int direction)
{
	m_Actors.push_back(shared_ptr<Actor>(new Spray(X, Y, direction, this)));
}

void StudentWorld::initRegularSalmonella(double X, double Y)
{
	m_Actors.push_back(shared_ptr<Actor>(new RegularSalmonella(X, Y, this)));
}

void StudentWorld::initAggressiveSalmonella(double X, double Y)
{
	m_Actors.push_back(shared_ptr<Actor>(new AggressiveSalmonella(X, Y, this)));
}

void StudentWorld::initEColi(double X, double Y)
{
	m_Actors.push_back(shared_ptr<Actor>(new EColi(X, Y, this)));
}

int StudentWorld::getOrUpdateRemainingEnemies(int number)
{
	m_remainingEnemyCount += number;
	return m_remainingEnemyCount;

}

bool StudentWorld::hitWithWeapon(double X, double Y, int damage)
{
	//iterate through list of all actors
	list<shared_ptr<Actor>>::iterator it;
	for (it = m_Actors.begin(); it != m_Actors.end();it++)
	{
		shared_ptr<Actor> curr = *it;
		double currX = curr->getX();
		double currY = curr->getY();
		//if projectile is colliding with current actor, the actor is weaponDamageable and is alive
		if (sqrt((X - currX) * (X - currX) + (Y - currY) * (Y - currY)) < (2.0 * SPRITE_RADIUS) 
			&& curr->isWeaponDamageable() && curr->isAlive())
		{
			//apply the projectile's damage to the actor it collided with
			curr->takeDamage(damage);
			//if the actor that was hit was an enemy
			if (curr->isHarmful() && !curr->isBooster())
			{
				//if damaged but not killed
				if (curr->getHp() > 0)
				{
					//play damage sound based on type of enemy damaged
					if (curr->isFoodSeeker())
						playSound(SOUND_SALMONELLA_HURT);
					else if (curr->isHunterKiller())
						playSound(SOUND_ECOLI_HURT);
				}
				//if killed
				else if (curr->getHp() <= 0)
				{
					//all enemies give 100 points on death
					increaseScore(100);

					//play death sound based on type of bacteria
					if (curr->isFoodSeeker())
					{
						playSound(SOUND_SALMONELLA_DIE);
					}
					else if (curr->isHunterKiller())
					{
						playSound(SOUND_ECOLI_DIE);
					}

					//50% chance for food to spawn in enemy death area
					int chance = randInt(0, 1);
					if (chance == 0)
						initFood(currX, currY);
					//reduce enemy count
					m_remainingEnemyCount--;
				}
			}

			//confirm successful hit
			return true;
		}
	}
	//false for unsuccessful hit
	return false;
}

bool StudentWorld::isOverlappingSocrates(double X, double Y)
{
	double socX = m_Socrates->getX();
	double socY = m_Socrates->getY();
	//if within euclidian distance of 2 x the sprite radius, yes colliding
	if (sqrt((X - socX) * (X - socX) + (Y - socY) * (Y - socY)) < (2.0 * SPRITE_RADIUS))
		return true;
	return false;
}

double StudentWorld::getDistanceToSocrates(double X, double Y)
{
	double socX = m_Socrates->getX();
	double socY = m_Socrates->getY();
	//return euclidian distance between inputted coords and player
	return sqrt((X - socX) * (X - socX) + (Y - socY) * (Y - socY));
}

double StudentWorld::getAngleToSocrates(double X, double Y)
{
	const double PI = 4 * atan(1);
	double socX = m_Socrates->getX();
	double socY = m_Socrates->getY();
	//return directional angle to player
	return atan2(socY - Y, socX - X) * 180 / PI;
}

double StudentWorld::getAngleOfNearestFood(double X, double Y)
{
	const double PI = 4 * atan(1);
	int closest = 129;
	int closestAngle = 0;
	//iterate through all actors
	list<shared_ptr<Actor>>::iterator it = m_Actors.begin();
	for (;it != m_Actors.end();it++)
	{
		shared_ptr<Actor> curr = *it;
		//if the current actor is a kind of food (edible) and alive
		if (curr->isEdible() && curr->isAlive())
		{
			double currX = curr->getX();
			double currY = curr->getY();
			int distance = sqrt((X - currX) * (X - currX) + (Y - currY) * (Y - currY));
			//if the food is within 128 pixels and the distance to the food is less than the previous closest distance
			if (distance <= 128 && distance < closest)
			{
				//set closest to this distance
				closest = distance;
				//set closestAngle to the directional angle to this current food item
				closestAngle = atan2(currY - Y, currX - X) * 180 / PI;
			}
		}

	}
	//if a close food item is found return the directional angle to that food
	if (closest != 129)
		return closestAngle;
	//otherwise return -999 to indicate no edible item was found
	else
		return -999;
}

bool StudentWorld::isValidEnemyMovement(double X, double Y)
{
	//will not go out of petri dish
	if (sqrt((X - VIEW_WIDTH / 2) * (X - VIEW_WIDTH / 2) + (Y - VIEW_HEIGHT / 2) * (Y - VIEW_HEIGHT / 2))
		>= VIEW_RADIUS)
	{
		return false;
	}

	//no collision with an actor that blocks movement (eg. dirt)
	list<shared_ptr<Actor>>::iterator it = m_Actors.begin();
	for (;it != m_Actors.end();it++)
	{
		shared_ptr<Actor> curr = *it;
		if (curr->isMovementBlocker() && curr->isAlive())
		{
			double currX = curr->getX();
			double currY = curr->getY();
			if (sqrt((X - currX) * (X - currX) + (Y - currY) * (Y - currY))
				<= (SPRITE_WIDTH / 2))
			{
				return false;
			}
		}

	}

	//return true to signify a valid movement
	return true;
}

bool StudentWorld::consumeFoodIfOverlapping(double X, double Y)
{
	//iterate through all actors
	list<shared_ptr<Actor>>::iterator it = m_Actors.begin();
	for (;it != m_Actors.end();it++)
	{
		shared_ptr<Actor> curr = *it;
		//if the current actor is edible
		if (curr->isEdible() && curr->isAlive())
		{
			double currX = curr->getX();
			double currY = curr->getY();
			//if colliding with edible actor
			if (sqrt((X - currX) * (X - currX) + (Y - currY) * (Y - currY)) < (2.0 * SPRITE_RADIUS))
			{
				//edible actor disappears and is eaten
				curr->takeDamage();
				//return true for successful eat
				return true;
			}
		}

	}
	//return false for no eat
	return false;
}

void StudentWorld::updateSocratesHp(int number)
{
	//0 restores to full health
	m_Socrates->restoreHp(number);
	//negative number means socrates is being damaged
	if (number < 0)
		playSound(SOUND_PLAYER_HURT);
}

int StudentWorld::getOrUpdateSprays(int number)
{
	//static cast player pointer from actor base class to player subclass to access its function
	return static_pointer_cast<Socrates>(m_Socrates)->getOrUpdateSprays(number);
}

int StudentWorld::getOrUpdateFlames(int number)
{
	//static cast player pointer from actor base class to player subclass to access its function
	return static_pointer_cast<Socrates>(m_Socrates)->getOrUpdateFlames(number);
}

void StudentWorld::updateStatusText()
{
	//get info for status text
	int score = getScore();
	int level = getLevel();
	int lives = getLives();
	int health = m_Socrates->getHp();
	//static cast player pointer from actor base class to player subclass to access its functions
	int sprays = static_pointer_cast<Socrates>(m_Socrates)->getOrUpdateSprays();
	int flames = static_pointer_cast<Socrates>(m_Socrates)->getOrUpdateFlames();

	//create ostringstream called status
	ostringstream status;

	//add score to stream
	if (score < 0)
	{
		//add dash for negative score
		status << "Score: -";
		score += (-score * 2);
		status.fill('0');
		status << setw(5) << score;
	}
	else
	{
		status << "Score: ";
		status.fill('0');
		status << setw(6) << score;
	}

	//add level to stream
	status << "  Level: " << level;

	//add lives to stream
	status << "  Lives: " << lives;

	//add health to stream
	status << "  Health: " << health;

	//add sprays to stream
	status << "  Sprays: " << sprays;

	//add flames to stream
	status << "  Flames: " << flames;

	//set string to the stream
	string statusText = status.str();
	//display status text
	setGameStatText(statusText);
}

void StudentWorld::initHelper(double& X, double& Y)
{
	//relies on init having overlappable objects (eg. dirt) initialized last

	//init coordinate variables
	double randX = 0;
	double randY = 0;
	double centerX = VIEW_WIDTH / 2;
	double centerY = VIEW_HEIGHT / 2;

	bool valid = false;
	while (valid == false)
	{
		//add object in a random location
		//Each must be no more 120 pixels from the center (VIEW_WIDTH/2, VIEW_HEIGHT/2)

		//keep generating random coordinates +- 120 from the center until the euclidian distance from the center is less than or equal to 120 pixels
		do {
			randX = randInt(centerX - 120, centerX + 120);
			randY = randInt(centerY - 120, centerY + 120);
		} while (sqrt((randX - centerX) * (randX - centerX) + (randY - centerY) * (randY - centerY)) > 120);

		//if no actors in actor list, valid is true because no collisions possible
		if (m_Actors.size() == 0)
			valid = true;
			
		/*centers must be 2*SPRITE_RADIUS apart from FOOD OBJECTS or PITS
		ok to overlap with other dirt*/
		
		//iterate through all actors
		list<shared_ptr<Actor>>::iterator it;
		for (it = m_Actors.begin(); it != m_Actors.end();it++)
		{
			shared_ptr<Actor> curr = *it;
			double currX = curr->getX();
			double currY = curr->getY();
			//if colliding with an actor
			if (sqrt((randX - currX) * (randX - currX) + (randY - currY) * (randY - currY)) <= (2.0 * SPRITE_RADIUS))
			{
				//if colliding with a movementblocker (eg. dirt), break from iterating through actors and generate new coordinates
				if (!(curr->isMovementBlocker()))
				{
					valid = false;
					break;
				}
				
			}

			//set valid to true upon one successful check of collision, if at least one invalid overlap is detected,
			//valid will be set to false and the loop will terminate, never reaching this statement
			valid = true;
		}

	}
	//set passed in variables to randomly generated valid coordinates
	X = randX;
	Y = randY;
	
}

int StudentWorld::init()
{

	//set L to the current level (starts at one)
	int L = getLevel();

	//init socrates
	m_Socrates = shared_ptr<Socrates>(new Socrates(this));

	//init pits//
	
	double randX;
	double randY;

	//create L number of pits at random locations
	int count = L;
	for (int i = 0; i < count; i++)
	{
		initHelper(randX, randY);
		initPit(randX, randY);
	}

	//init food//

	//create the lower of 5*L and 25 foods at random locations
	count = min(5 * L, 25);
	for (int i = 0; i < count; i++)
	{
		initHelper(randX, randY);
		initFood(randX, randY);
	}

	//init dirt//

	//create the higher of 180 - 20*L and 20 dirt at random locations
	count = max(180 - 20 * L, 20);
	for (int i = 0; i < count; i++)
	{
		initHelper(randX, randY);
		initDirt(randX, randY);
	}

	//a pit for each L, each pit has 10 bacterium
	m_remainingEnemyCount = L * 10;
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	//set L to current level (1+)
	int L = getLevel();
	//activate player function
	m_Socrates->doSomething();
	//if player is dead, play death sound, decrease lives, and return player death status
	if (!(m_Socrates->isAlive()))
	{
		playSound(SOUND_PLAYER_DIE);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	//iterate through all actors in actor list
	list<shared_ptr<Actor>>::iterator it;
	for (it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		shared_ptr<Actor> curr = *it;
		//if current actor is alive
		if (curr->isAlive())
		{
			//activate the current actor
			curr->doSomething();
			//if player is dead play death sound, decrease lives, and return death status
			if (!m_Socrates->isAlive())
			{
				playSound(SOUND_PLAYER_DIE);
				decLives();
				return GWSTATUS_PLAYER_DIED;
			}
		}
		//if there are no more active enemies, play finished level sound and return the finished level status to advance
		if (m_remainingEnemyCount <= 0)
		{
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}

	}


	//delete any dead actors (collected collectibles, killed enemy)
	list<shared_ptr<Actor>>::iterator death = m_Actors.begin();
	for (; death != m_Actors.end();)
	{
		shared_ptr<Actor> curr = *death;
		if (!curr->isAlive()) //if dead
		{
			//remove from actor list
			death = m_Actors.erase(death);
		}
		else
			death++;
	}

	//set PI constant
	const double PI = 4 * atan(1.0);

	//set lifetime to value that potentially changes based on level
	int lifetime = max(rand() % (300 - 10 * L), 50);

	//chance variables to appropriate values
	int chanceFungus = max(510 - (L * 10), 200);
	int randFungus = randInt(0, chanceFungus - 1);

	int randAngle;

	//if randFungus is 0, spawn a fungus at a random location along player's path
	if (randFungus == 0)
	{
		randAngle = randInt(0, 359);
		double FungusX = VIEW_WIDTH / 2 + (VIEW_RADIUS) * 1.0 * cos(randAngle * (2 * PI / 360));
		double FungusY = VIEW_HEIGHT / 2 + (VIEW_RADIUS) * 1.0 * sin(randAngle * (2 * PI / 360));
		m_Actors.push_back(shared_ptr<Actor>(new Fungus(FungusX, FungusY, this, lifetime)));
	}

	//chance variables to appropriate values
	int chanceGoodie = max(510 - (L * 10), 250);
	int randGoodie = randInt(0, chanceGoodie - 1);

	//if randGoodie is 0
	if (randGoodie == 0)
	{
		randAngle = randInt(0, 359);
		double goodieX = VIEW_WIDTH / 2 + (VIEW_RADIUS) * 1.0 * cos(randAngle * (2 * PI / 360));
		double goodieY = VIEW_HEIGHT / 2 + (VIEW_RADIUS) * 1.0 * sin(randAngle * (2 * PI / 360));

		//simulate percent chance
		int percentChance = randInt(1, 10);
		//60% chance to spawn health powerup
		if (percentChance <= 6)
		{
			m_Actors.push_back(shared_ptr<Actor>(new Health(goodieX, goodieY, this, lifetime)));
		}
		//20% chance to spawn flame powerup
		else if (6 < percentChance && percentChance <= 8)
		{
			m_Actors.push_back(shared_ptr<Actor>(new FlameGoodie(goodieX, goodieY, this, lifetime)));
		}
		//20% chance to spawn lives powerup
		else if (percentChance > 8)
		{
			m_Actors.push_back(shared_ptr<Actor>(new LivesGoodie(goodieX, goodieY, this, lifetime)));
		}
	}

	//update status text with game stats
	updateStatusText();

	//return continue game status
	return GWSTATUS_CONTINUE_GAME;


}

void StudentWorld::cleanUp()
{
	//iterate through actors list and erase each item
	list<shared_ptr<Actor>>::iterator clean = m_Actors.begin();
	for (; clean != m_Actors.end();)
	{
		clean = m_Actors.erase(clean);
	}
}
