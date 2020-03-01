#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

#include <string>
#include <memory>

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, Direction startDirection, int depth, 
		int hp, StudentWorld* pointer, int damage = 1);

	StudentWorld* getPointerToStudentWorld() const;

	//identifiers
	bool isAlive() const;
	virtual bool isEdible() const;
	virtual bool isMovementBlocker() const;
	virtual bool isWeaponDamageable() const;
	virtual bool isHarmful() const;
	virtual bool isFoodSeeker() const;
	virtual bool isHunterKiller() const;
	virtual bool isBooster() const;

	void takeDamage(int damage = 1);

	void restoreHp(int number = 0);

	int getHp() const;

	int getDamage() const;

	void fireWeapon(int maxDistance);

	virtual void doSomething();

private:
	bool m_alive;
	int m_hp;
	int m_maxHp;
	StudentWorld* m_StudentWorld;
	int m_distance;
	int m_damage;
};

class Socrates : public Actor
{
public:
	Socrates(StudentWorld* pointer);
	int getOrUpdateSprays(int number = 0);
	int getOrUpdateFlames(int number = 0);
	virtual void doSomething();
private:
	void moveToAngle();
	int m_sprayCount;
	int m_flameCount;
	int m_positionalAngle;
};

class Spray : public Actor
{
public:
	Spray(double startX, double startY, int startDirection, StudentWorld* pointer);
	virtual void doSomething();
};

class Flame : public Actor
{
public:
	Flame(double startX, double startY, int startDirection, StudentWorld* pointer);
	virtual void doSomething();
};

class Pit : public Actor
{
public:
	Pit(double startX, double startY, StudentWorld* pointer);
	virtual void doSomething();
private:
	int getTotalBacteria() const;
	void spawnRegularSalmonellaHere();
	void spawnAggressiveSalmonellaHere();
	void spawnEColiHere();
	int m_regularSalmonella;
	int m_agressiveSalmonella;
	int m_eColi;
};

class Dirt : public Actor
{
public:
	Dirt(double startX, double startY, StudentWorld* pointer);
	virtual bool isMovementBlocker() const;
	virtual bool isWeaponDamageable() const;
};

class Food : public Actor
{
public:
	Food(double startX, double startY, StudentWorld* pointer);
	virtual bool isEdible() const;
};

class Enemy : public Actor
{
public:
	Enemy(double startX, double startY, int imgID, StudentWorld* pointer, int hp, int damage);
	virtual void doSomething();
	int getOrUpdateMovementPlan(int number = 0);
	void toggleHuntingPlayer();
	virtual bool isHarmful() const;
private:
	int m_movementPlan;
	int m_foodEaten;
	bool m_huntingPlayer;
	virtual void performFunction() = 0;
	virtual void performOptionalFunction(); 
	virtual void spawnEnemy(double X, double Y) const = 0;
};

class Salmonella : public Enemy
{
public:
	Salmonella(double startX, double startY, StudentWorld* pointer, int hp, int damage);
	virtual bool isFoodSeeker() const;
	virtual bool isWeaponDamageable() const;
private:
	virtual void performFunction();
	void goRandomDirection();
};

class RegularSalmonella : public Salmonella
{
public:
	RegularSalmonella(double startX, double startY, StudentWorld* pointer);
private:
	virtual void spawnEnemy(double X, double Y) const;
};

class AggressiveSalmonella : public Salmonella
{
public:
	AggressiveSalmonella(double startX, double startY, StudentWorld* pointer);
private:
	virtual void performOptionalFunction();
	virtual void spawnEnemy(double X, double Y) const;
};

class EColi : public Enemy
{
public:
	EColi(double startX, double startY, StudentWorld* pointer);
	virtual bool isWeaponDamageable() const;
	virtual bool isHunterKiller() const;
private:
	virtual void performFunction();
	virtual void spawnEnemy(double X, double Y) const;
};

class Collectible : public Actor
{
public:
	//lifeticks must be determined in studentworld because depends on number of levels
	Collectible(double startX, double startY, int imgID, StudentWorld* pointer, double lifeTicks, int points); 
	virtual void doSomething();
	virtual bool isBooster() const;
	virtual bool isWeaponDamageable() const;
private:
	int m_lifeTicks;
	int m_points;
	virtual void performFunction() = 0;
};

class Health : public Collectible
{
public:
	Health(double startX, double startY, StudentWorld* pointer, double lifeTicks);
private:
	virtual void performFunction();
};

class FlameGoodie : public Collectible
{
public:
	FlameGoodie(double startX, double startY, StudentWorld* pointer, double lifeTicks);
private:
	virtual void performFunction();
};

class LivesGoodie : public Collectible
{
public:
	LivesGoodie(double startX, double startY, StudentWorld* pointer, double lifeTicks);
private:
	virtual void performFunction();
};

class Fungus : public Collectible
{
public:
	Fungus(double startX, double startY, StudentWorld* pointer, double lifeTicks);
	virtual bool isHarmful() const;
private:
	virtual void performFunction();
};

#endif // ACTOR_H_
