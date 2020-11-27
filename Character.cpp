#include "Character.h"


//creates character with the given attributes
Character::Character(string _name, string _type, int _attack, int _defense, int _remainingHealth, int _nMaxRounds) {
    this->name=_name;
    this->type=_type;
    this->attack=_attack;
    this->defense=_defense;
    this->remainingHealth=_remainingHealth;
    this->nMaxRounds=_nMaxRounds;
    this->nRoundsSinceSpecial=0;
    this->isAlive= true;  
    this->healthHistory = new int[_nMaxRounds+1];
}

//copy constructor
Character::Character(const Character& character) {
    this->attack=character.attack;
    this->defense=character.defense;
    this->isAlive=character.isAlive;
    this->nMaxRounds=character.nMaxRounds;
    this->nRoundsSinceSpecial=character.nRoundsSinceSpecial;
    this->name=character.name;
    this->remainingHealth=character.remainingHealth;
    this->type=character.type;
 
    int n=(character.nMaxRounds+1);
    this->healthHistory = new int[n];
    for (int i = 0; i < n; ++i){
        this->healthHistory[i]=character.healthHistory[i];
    }

}

//= operator overloader Öylesine şimdilik
Character& Character::operator=(const Character& character) {
    if(this==&character) {
        return *this;
    }
    if(this->healthHistory != NULL) {
        delete[] this->healthHistory;
    }
    this->attack=character.attack;
    this->defense=character.defense;
    this->isAlive=character.isAlive;
    this->nMaxRounds=character.nMaxRounds;
    this->nRoundsSinceSpecial=character.nRoundsSinceSpecial;
    this->name=character.name;
    this->remainingHealth=character.remainingHealth;
    this->type=character.type;
    int n=(character.nMaxRounds)+1;
    this->healthHistory = new int[n];
    for (int i = 0; i < n; ++i){
        this->healthHistory[i]=character.healthHistory[i];
        // int m=*(character.healthHistory+i);
        // *(this->healthHistory+i) = m; //DEĞİŞTİRDİN
    }
    return *this;

}


bool Character::operator<(const Character& other) {
    return other.name > this->name;

}

Character::~Character() {
    if(this->healthHistory != NULL)
        delete[] healthHistory;
    /* if (this->healthHistory != NULL) {
         delete[] this->healthHistory;
     }*/
}
