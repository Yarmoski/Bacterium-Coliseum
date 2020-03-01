#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
class Actor;

#include <string>
#include <list>
#include <memory>

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
  
    void initFlame(double X, double Y, int direction);
    void initSpray(double X, double Y, int direction);
    void initRegularSalmonella(double X, double Y);
    void initAggressiveSalmonella(double X, double Y);
    void initEColi(double X, double Y);
    int getOrUpdateRemainingEnemies(int number = 0);
    bool hitWithWeapon(double X, double Y, int damage);
    bool isOverlappingSocrates(double X, double Y);
    double getDistanceToSocrates(double X, double Y);
    double getAngleToSocrates(double X, double Y);
    double getAngleOfNearestFood(double X, double Y);
    bool isValidEnemyMovement(double X, double Y);
    bool consumeFoodIfOverlapping(double X, double Y);
    void updateSocratesHp(int number = 0);
    int getOrUpdateSprays(int number = 0);
    int getOrUpdateFlames(int number = 0);

private:
    void initHelper(double& X, double& Y);
    void initDirt(double X, double Y);
    void initPit(double X, double Y);
    void initFood(double X, double Y);
    void updateStatusText();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    //list of smart pointers to actors, lessens memory management requirements (not forbidden in spec, used in industry)
    std::list<std::shared_ptr<Actor>> m_Actors;
    //smrat pointer to player
    std::shared_ptr<Actor> m_Socrates;
    int m_remainingEnemyCount;
};

#endif // STUDENTWORLD_H_
