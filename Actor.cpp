#include "Actor.h"
#include "StudentWorld.h"

using namespace std;


//Actor class implmentation

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, int depth,
	int hp, StudentWorld* pointer, int damage) :
	GraphObject(imageID, startX, startY, startDirection, depth)
{
	m_alive = true; //all actors start out alive
	m_StudentWorld = pointer; 
	m_distance = 0; //default travel distance of 0 (not traveled)
	m_damage = damage; 
	m_maxHp = hp; //maximum hp is set to initial hp
	m_hp = hp;
}

StudentWorld* Actor::getPointerToStudentWorld() const
{
	return m_StudentWorld;
}

//The following is___ functions return a boolean based on whether the Actor has the given characteristic
bool Actor::isAlive() const
{
	return m_alive;
}

bool Actor::isEdible() const
{
	return false;
}

bool Actor::isMovementBlocker() const
{
	return false;
}

bool Actor::isWeaponDamageable() const
{
	return false;
}

bool Actor::isHarmful() const
{
	return false;
}

bool Actor::isFoodSeeker() const
{
	return false;
}

bool Actor::isHunterKiller() const
{
	return false;
}

bool Actor::isBooster() const
{
	return false;
}

void Actor::takeDamage(int damage)
{
	//subtract damage from hp, if it dips below 0 set to dead
	m_hp -= damage;
	if (m_hp <= 0)
		m_alive = false;
}

int Actor::getHp() const
{
	return m_hp;
}

int Actor::getDamage() const
{
	return m_damage;
}

void Actor::restoreHp(int number)
{
	//if input is 0, restore to full health
	if (number == 0)
		m_hp = m_maxHp;
	else //for non-zero inputs, restore/subtract that much hp
	{
		m_hp += number;
		if (m_hp > m_maxHp) //if health is restored past max, set hp to max
			m_hp = m_maxHp;
		if (m_hp <= 0) //declare dead upon 0 hp
			m_alive = false;
	}

}

void Actor::fireWeapon(int maxDistance)
{
	if (!isAlive())
		return;
	else
	{
		//if collides with valid, weapon damageable actor, disappear and deal damage to that actor
		if (getPointerToStudentWorld()->hitWithWeapon(getX(), getY(), m_damage))
		{
			takeDamage();
			return;
		}
		//if did not collide, continue to move in same direction until reach maxDistance and disappear
		else
		{
			moveAngle(getDirection(), SPRITE_RADIUS * 2);
			m_distance += (SPRITE_RADIUS * 2.0);
			if (m_distance >= maxDistance)
				takeDamage();
		}
	}
}

void Actor::doSomething()
{
	return; //do nothing, not pure virtual because it would cause replicate implementations
}

//Socrates class implementation

Socrates::Socrates(StudentWorld* pointer)
	:Actor(IID_PLAYER, 0, VIEW_HEIGHT / 2, 0, 0, 100, pointer)
{
	m_positionalAngle = 180; //place player at left, middle of dish
	m_flameCount = 5;
	m_sprayCount = 20;
}

void Socrates::moveToAngle()
{
	const double PI = 4 * atan(1.0);
	//move player to a specific spot relative to the dish's perimeter (move in a circular fashion) using polar coordinates radius(cos(angle)) and radius(sin(angle))
	moveTo(VIEW_WIDTH / 2 + (VIEW_RADIUS) * 1.0 * cos(m_positionalAngle * (2 * PI / 360)),
		VIEW_HEIGHT / 2 + (VIEW_RADIUS) * 1.0 * sin(m_positionalAngle * (2 * PI / 360)));
	setDirection(m_positionalAngle + 180); //make sure player faces the middle of the dish
}

int Socrates::getOrUpdateSprays(int number)
{
	m_sprayCount += number;
	return m_sprayCount;
}

int Socrates::getOrUpdateFlames(int number)
{
	m_flameCount += number;
	return m_flameCount;
}

void Socrates::doSomething()
{
	if (isAlive())
	{
		StudentWorld* world = getPointerToStudentWorld();
		int ch;
		if (world->getKey(ch)) //key ch was pressed
		{
			switch (ch)
			{
			case KEY_PRESS_SPACE:
				//if player has at least one spray
				if (m_sprayCount >= 1)
				{
					int sprayDirection = m_positionalAngle + 180;
					double sprayX;
					double sprayY;
					//Add a spray object 2*SPRITE_RADIUS pixels directly in front of himself (in the same 
					//direction Socrates is facing, 
					//which is always toward the center of the Petri dish).
					getPositionInThisDirection(sprayDirection, (2 * SPRITE_RADIUS), sprayX, sprayY);
					world->initSpray(sprayX, sprayY, sprayDirection);
					//decrement sprays
					m_sprayCount--;
					//play sound SOUND_PLAYER_SPRAY
					world->playSound(SOUND_PLAYER_SPRAY);
				}
				break;
			case KEY_PRESS_ENTER:
				if (m_flameCount >= 1)
				{
					int flameDirection = m_positionalAngle + 180;
					double flameX;
					double flameY;

					//init 16 flame objects evenly spaced around the player
					for (int i = 0; i < 16; i++)
					{
						getPositionInThisDirection(flameDirection, (2 * SPRITE_RADIUS), flameX, flameY);
						world->initFlame(flameX, flameY, flameDirection);
						flameDirection += 22;
					}
					//decrease flame usage counter
					m_flameCount--;
					world->playSound(SOUND_PLAYER_FIRE);
				}
				break;
			case KEY_PRESS_LEFT:
				//move to an angle 5 degrees higher on the circle (0-359 degrees)
				m_positionalAngle += 5;
				moveToAngle();
				break;
			case KEY_PRESS_RIGHT:
				//move to an angle 5 degrees lower on the circle (0-359 degrees)
				m_positionalAngle -= 5;
				moveToAngle();
				break;
			}
		}
		else //no key was pressed
		{
			//regen a spray
			if (m_sprayCount < 20)
			{
				m_sprayCount++;
			}
		}

	}
	return;
}

//Spray class implementation

Spray::Spray(double startX, double startY, int startDirection, StudentWorld* pointer)
	:Actor(IID_SPRAY, startX, startY, startDirection, 1, 1, pointer, 2)
{
}

void Spray::doSomething()
{
	//input 112 for maximum distance
	fireWeapon(112); //function explained elsewhere
}

//Flame class implementation

Flame::Flame(double startX, double startY, int startDirection, StudentWorld* pointer)
	:Actor(IID_FLAME, startX, startY, startDirection, 1, 1, pointer, 5)
{
}

void Flame::doSomething()
{
	//input 32 for maximum distance
	fireWeapon(32); //function explained elsewhere
}

//Pit class implementation

Pit::Pit(double startX, double startY, StudentWorld* pointer)
	: Actor(IID_PIT, startX, startY, 0, 1, 1, pointer)
{
	//each pit contains 5 regularSalmonella, 3 aggressiveSalmonella, and 2 eColi
	m_regularSalmonella = 3;
	m_agressiveSalmonella = 1;
	m_eColi = 1;
}

void Pit::doSomething()
{
	//if emitted all bacteria, die
	if (getTotalBacteria() <= 0)
	{
		takeDamage();
		return;
	}
	int spawnChance = randInt(1, 50);
	//1 in 50 chance of spawning bacteria each tick
	if (spawnChance == 1)
	{

		//each bacteria has an equal chance of being spawned each spawn tick

		double pitX = getX();
		double pitY = getY();
		int totalBacteria = getTotalBacteria();
		StudentWorld* world = getPointerToStudentWorld();

		//if only reg salmonellas remain
		if (totalBacteria == m_regularSalmonella)
		{
			spawnRegularSalmonellaHere();
			return;
		}
		//if only aggresive salmonellas remain
		else if (totalBacteria == m_agressiveSalmonella)
		{
			spawnAggressiveSalmonellaHere();
			return;
		}
		//if only eColis remain
		else if (totalBacteria == m_eColi)
		{
			spawnEColiHere();
			return;
		}
		//if all three types remain
		if (m_regularSalmonella > 0 && m_agressiveSalmonella > 0 && m_eColi > 0)
		{
			int typeChance = randInt(1, 3); //equal chance for all 3
			switch (typeChance)
			{
			case 1:
				spawnRegularSalmonellaHere();
				return;
			case 2:
				spawnAggressiveSalmonellaHere();
				return;
			case 3:
				spawnEColiHere();
				return;
			}
		}
		//if no more ecoli
		if (m_regularSalmonella > 0 && m_agressiveSalmonella > 0 && m_eColi <= 0)
		{
			int typeChance = randInt(1, 2); //equal chance for all 2
			switch (typeChance)
			{
			case 1:
				spawnRegularSalmonellaHere();
				return;
			case 2:
				spawnAggressiveSalmonellaHere();
				return;
			}
		}
		//if no more aggressivesalmonella
		if (m_regularSalmonella > 0 && m_agressiveSalmonella <= 0 && m_eColi > 0)
		{
			int typeChance = randInt(1, 2); //equal chance for all 2
			switch (typeChance)
			{
			case 1:
				spawnRegularSalmonellaHere();
				return;
			case 2:
				spawnEColiHere();
				return;
			}
		}
		//if no more regular salmonella
		if (m_regularSalmonella <= 0 && m_agressiveSalmonella > 0 && m_eColi > 0)
		{
			int typeChance = randInt(1, 2); //equal chance for all 2
			switch (typeChance)
			{
			case 1:
				spawnAggressiveSalmonellaHere();
				return;
			case 2:
				spawnEColiHere();
				return;
			}
		}
	}
}

int Pit::getTotalBacteria() const
{
	return m_regularSalmonella + m_agressiveSalmonella + m_eColi;
}

void Pit::spawnRegularSalmonellaHere()
{
	//spawn this type of enemy using the function in StudentWorld
	StudentWorld* world = getPointerToStudentWorld();
	m_regularSalmonella--;
	world->playSound(SOUND_BACTERIUM_BORN);
	world->initRegularSalmonella(getX(), getY());
}

void Pit::spawnAggressiveSalmonellaHere()
{
	//spawn this type of enemy using the function in StudentWorld
	StudentWorld* world = getPointerToStudentWorld();
	m_agressiveSalmonella--;
	world->playSound(SOUND_BACTERIUM_BORN);
	world->initAggressiveSalmonella(getX(), getY());
}

void Pit::spawnEColiHere()
{
	//spawn this type of enemy using the function in StudentWorld
	StudentWorld* world = getPointerToStudentWorld();
	m_eColi--;
	world->playSound(SOUND_BACTERIUM_BORN);
	world->initEColi(getX(), getY());
}

//Dirt class implementation

Dirt::Dirt(double startX, double startY, StudentWorld* pointer) :
	Actor(IID_DIRT, startX, startY, 0, 1, 1, pointer)
{

}

bool Dirt::isMovementBlocker() const
{
	return true;
}

bool Dirt::isWeaponDamageable() const
{
	return true;
}

//Food class implementation

Food::Food(double startX, double startY, StudentWorld* pointer)
	:Actor(IID_FOOD, startX, startY, 90, 1, 1, pointer)
{

}

bool Food::isEdible() const
{
	return true;
}

//Enemy class implementation

Enemy::Enemy(double startX, double startY, int imgID, StudentWorld* pointer, int hp, int damage)
	:Actor(imgID, startX, startY, 90, 0, hp, pointer, damage)
{
	m_movementPlan = 0;
	m_foodEaten = 0;
	m_huntingPlayer = false;
}

void Enemy::doSomething()
{
	if (!isAlive())
		return;

	double X = getX();
	double Y = getY();

	//performOptionalFunction only does something in aggressiveSalmonellas
	m_huntingPlayer = false;
	performOptionalFunction();

	StudentWorld* world = getPointerToStudentWorld();

	// if colliding with player, damage player for bacteria's damage amount
	if (world->isOverlappingSocrates(getX(), getY()))
	{
		world->updateSocratesHp(-getDamage());
	}
	else
	{
		//if 3 food have been eaten by this bacteria
		if (m_foodEaten == 3)
		{
			double newx;
			double newy;
			//determine coordinates for a new enemy to spawn based on position in petri dish
			if (X < VIEW_WIDTH / 2)
				newx = X + SPRITE_RADIUS;
			else if (X > VIEW_WIDTH / 2) 
				newx = X - SPRITE_RADIUS;
			else if (X == VIEW_WIDTH / 2)
				newx = X;
			if (Y < VIEW_HEIGHT / 2)
				newy = Y + SPRITE_RADIUS;
			else if (Y >= VIEW_HEIGHT / 2)
				newy = Y - SPRITE_RADIUS;
			else if (Y == VIEW_HEIGHT / 2)
				newy = Y;
			spawnEnemy(newx, newy);
			world->playSound(SOUND_BACTERIUM_BORN);
			world->getOrUpdateRemainingEnemies(1); //increase game's enemy count
			m_foodEaten = 0; //reset food count so enemies do not continuously spawn
		}
		else
		{
			//if overlapping with food, eat it and increment food counter
			if (world->consumeFoodIfOverlapping(X, Y))
			{
				m_foodEaten++;
			}
		}
	}

	//if currently hunting player, do not performFunction()
	if (m_huntingPlayer)
		return;

	performFunction();

}

int Enemy::getOrUpdateMovementPlan(int number) 
{
	// 0 to get movement plan
	if (number == 0)
		return m_movementPlan;
	//otherwise, add param to movementPlan distance
	m_movementPlan += number;
	//if exceed limit, set to limit
	if (m_movementPlan >= 10) //max ticks bacteria movement plan 
		m_movementPlan = 10;

	return m_movementPlan;
}

void Enemy::toggleHuntingPlayer()
{
	if (m_huntingPlayer == true)
		m_huntingPlayer = false;
	else
		m_huntingPlayer = true;
}

bool Enemy::isHarmful() const
{
	return true;
}

void Enemy::performOptionalFunction()
{
	//nothing
}

//Salmonella class implementation

Salmonella::Salmonella(double startX, double startY, StudentWorld* pointer, int hp, int damage)
	:Enemy(startX, startY, IID_SALMONELLA, pointer, hp, damage)
{

}

bool Salmonella::isFoodSeeker() const
{
	return true;
}

bool Salmonella::isWeaponDamageable() const
{
	return true;
}

void Salmonella::performFunction()
{
	//common functions between salmonellas go here
	if (getOrUpdateMovementPlan() > 0)
	{
		getOrUpdateMovementPlan(-1); //decrement movementplan
		double newx;
		double newy;
		getPositionInThisDirection(getDirection(), 3, newx, newy);
		//if it can continue to move in the current direction
		if (getPointerToStudentWorld()->isValidEnemyMovement(newx, newy))
		{
			//move 3 pixels in current direction
			moveAngle(getDirection(), 3);
		}
		else //if it cannot continue in the current direction
		{
			goRandomDirection();
		}
		return;
	}
	else
	{
		//get angle of nearest food
		double foodAngle = getPointerToStudentWorld()->getAngleOfNearestFood(getX(), getY());
		if (foodAngle != -999) //if food is nearby
		{
			double newx;
			double newy;
			getPositionInThisDirection(foodAngle, 3, newx, newy);
			//if can move 3 pixels toward the food without colliding with object/exiting the dish
			if (getPointerToStudentWorld()->isValidEnemyMovement(newx, newy))
			{
				//face the food and move 3 pixels toward it
				setDirection(foodAngle);
				moveAngle(foodAngle, 3);
				//set movementplan to 10 so bacteria continues to move in this direction toward the food
				getOrUpdateMovementPlan(10);
			}
			else //if cannot move toward the food
			{
				goRandomDirection();
			}
			return;
		}
		else //food not found
		{
			goRandomDirection();
		}
	}
}

void Salmonella::goRandomDirection()
{
	//pick a random angle
	int newDirection = randInt(0, 359);
	//face that angle
	setDirection(newDirection);
	//set intention to move in that direction
	getOrUpdateMovementPlan(10);
}

//RegularSalmonella class implementation

RegularSalmonella::RegularSalmonella(double startX, double startY, StudentWorld* pointer)
	:Salmonella(startX, startY, pointer, 4, 1)
{
}

void RegularSalmonella::spawnEnemy(double X, double Y) const
{
	getPointerToStudentWorld()->initRegularSalmonella(X, Y);
}

//AggressiveSalmonella class implementation

AggressiveSalmonella::AggressiveSalmonella(double startX, double startY, StudentWorld* pointer)
	:Salmonella(startX, startY, pointer, 10, 2)
{
}

void AggressiveSalmonella::performOptionalFunction()
{
	StudentWorld* world = getPointerToStudentWorld();
	//if within 72 pixels of player
	if (world->getDistanceToSocrates(getX(), getY()) <= 72)
	{
		toggleHuntingPlayer();
		double newx;
		double newy;
		double angleToSocrates = world->getAngleToSocrates(getX(), getY());
		getPositionInThisDirection(angleToSocrates, 3, newx, newy);
		setDirection(angleToSocrates); //face toward player
		//if can move toward player without being blocked or going out of dish
		if (getPointerToStudentWorld()->isValidEnemyMovement(newx, newy))
		{
			//move 3 pixels toward player
			moveAngle(angleToSocrates, 3);
		}
	}
}

void AggressiveSalmonella::spawnEnemy(double X, double Y) const
{
	getPointerToStudentWorld()->initAggressiveSalmonella(X, Y);
}

//EColi class implementation

EColi::EColi(double startX, double startY, StudentWorld* pointer)
	:Enemy(startX, startY, IID_ECOLI, pointer, 5, 4)
{
}

bool EColi::isWeaponDamageable() const
{
	return true;
}

bool EColi::isHunterKiller() const
{
	return true;
}

void EColi::performFunction()
{
	double X = getX();
	double Y = getY();
	StudentWorld* world = getPointerToStudentWorld();
	//if distance to player is less than or equal 256 pixels
	if (world->getDistanceToSocrates(X, Y) <= 256)
	{
		double socAngle = world->getAngleToSocrates(X, Y);
		//attempt up to 10 times
		for (int i = 0; i < 10; i++)
		{
			double newx, newy;
			getPositionInThisDirection(socAngle, 2, newx, newy);
			//face toward player
			setDirection(socAngle);
			//if can move toward player without being blocked or leaving the dish
			if (world->isValidEnemyMovement(newx, newy))
			{
				//move 2 pixels toward player
				moveAngle(socAngle, 2);
				break;
			}
			else //if blocked
			{
				//add 10 to the angle toward player
				socAngle += 10;
				if (socAngle > 360)
					socAngle -= 360;
			}
		}
	}
}

void EColi::spawnEnemy(double X, double Y) const
{
	getPointerToStudentWorld()->initEColi(X, Y);
}

//Collectible class implementation

Collectible::Collectible(double startX, double startY, int imgID, StudentWorld* pointer, double lifeTicks, int points)
	:Actor(imgID, startX, startY, 0, 1, 1, pointer)
{
	m_lifeTicks = lifeTicks;
	m_points = points;
}

void Collectible::doSomething()
{
	if (!isAlive())
		return;
	else
	{
		//if collide with player
		if (getPointerToStudentWorld()->isOverlappingSocrates(getX(), getY()))
		{
			//increase score by repsective amount based on type of powerup
			getPointerToStudentWorld()->increaseScore(m_points);
			//disappear/get removed from game
			takeDamage();
			//if not a harmful collectible, play powerup sound
			if (!isHarmful())
				getPointerToStudentWorld()->playSound(SOUND_GOT_GOODIE);
			//perform function specific to type of powerup
			performFunction();
			return;
		}
		if (m_lifeTicks <= 0) //if lifeticks ran out, destroy goodie
		{
			takeDamage();
		}
		else //if lifeticks still remain, decrement them
		{
			m_lifeTicks--;
		}
	}
}

bool Collectible::isBooster() const
{
	return true;
}

bool Collectible::isWeaponDamageable() const                                                      
{
	return true;
}

//Health class implementation

Health::Health(double startX, double startY, StudentWorld* pointer, double lifeTicks)
	:Collectible(startX, startY, IID_RESTORE_HEALTH_GOODIE, pointer, lifeTicks, 10)
{

}

void Health::performFunction()
{
	//restore player to full health
	getPointerToStudentWorld()->updateSocratesHp();
}

//FlameGoodie class implementation

FlameGoodie::FlameGoodie(double startX, double startY, StudentWorld* pointer, double lifeTicks)
	:Collectible(startX, startY, IID_FLAME_THROWER_GOODIE, pointer, lifeTicks, 10)
{

}

void FlameGoodie::performFunction()
{
	//add 5 flame thrower charges to socrates
	getPointerToStudentWorld()->getOrUpdateFlames(5);
}

//LivesGoodie class implementation
LivesGoodie::LivesGoodie(double startX, double startY, StudentWorld* pointer, double lifeTicks)
	:Collectible(startX, startY, IID_EXTRA_LIFE_GOODIE, pointer, lifeTicks, 10)
{

}

void LivesGoodie::performFunction()
{
	//increase lives by one
	getPointerToStudentWorld()->incLives();
}

//Fungus class implementation

Fungus::Fungus(double startX, double startY, StudentWorld* pointer, double lifeTicks)
	:Collectible(startX, startY, IID_FUNGUS, pointer, lifeTicks, -10)
{

}

bool Fungus::isHarmful() const
{
	return true;
}

void Fungus::performFunction()
{
	//hurt player by 20 hp
	getPointerToStudentWorld()->updateSocratesHp(-10);
}



