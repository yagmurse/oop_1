#include "Character.h"


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>


using namespace std;
int maxRoundGiven; //max round according to input
int currentRound;
int elves_special=10;
int dwarfs_special=20;
int wizards_special=50;
vector<Character> community_1;
vector<Character> community_2;
queue < pair < pair < string,string > ,string > > roundInfo; //pair<(attacker_name,defender_name),Ifspecial>
vector< vector< string > > all_input;
void place_inputs();
void special_wizards(vector<Character> &community,Character &character);
void special_elves(vector<Character> &community,Character &character);
bool special_dwarfs(Character &character);
bool is_war_end();
void one_round(Character &attacker,Character &defender,vector<Character> &teamA, vector<Character> &teamD,const string &ifSpecial);
void war_begin();
bool turn_1;
stack<Character> lastKilled_1;
stack<Character> lastKilled_2;


void deneme_print() {
    for(int i=0;i<5;i++) {
        cout << community_1[i].name << " ";
        for(int n=0;n<currentRound+1;n++) {
            cout <<  community_1[i].healthHistory[n] << " ";
        }
        cout <<endl;

    }
    for(int i=0;i<5;i++) {
        cout << community_2[i].name << " ";
        for(int n=0;n<currentRound+1;n++) {
            cout << community_2[i].healthHistory[n] << " ";
        }
        cout <<endl;

    }

}

int main(int argc, char* argv[]) {
    string input;
    int currentlineNumber=0;
    ifstream inputfile ("/Users/yselek14/Desktop/project1-yagmurse-master/testcases/inputs/input_7.txt");
    //store input line by line to global vector all_input
    if (inputfile.is_open())
    {
        while (getline (inputfile,input))
        {
            vector<string> currentLine;
            istringstream iss(input);
            do
            {
                string tmpStr;
                iss >> tmpStr;
                currentLine.push_back(tmpStr);
            }
            while (iss);
            all_input.push_back(currentLine);
            if(currentlineNumber==0) {
                maxRoundGiven=stoi(currentLine[0]);
                currentlineNumber++;
                all_input.pop_back();
            }
        }

        inputfile.close();
    }
    else { cout << "Unable to open input file"; }
    place_inputs();
    war_begin();
    deneme_print();
    return 0;
}

void war_begin() {
    bool _is_war_end;
    currentRound=0;
    for(int i=0;i<5;i++) {
        community_1[i].healthHistory[0]=community_1[i].remainingHealth;
        community_2[i].healthHistory[0]=community_2[i].remainingHealth;
    }
    while(!(is_war_end()) && ! roundInfo.empty() ) {
        cout << currentRound <<endl;
        pair<pair<string,string>,string> currentPair=roundInfo.front();
        roundInfo.pop();
        currentRound++;
        turn_1=currentRound%2;
        string attackerStr=currentPair.first.first;
        string defenderStr=currentPair.first.second;
        string special=currentPair.second;
        int com1int=0;
        int com2int=0;
        for(int i=0;i<5;i++) {
            if(com1int != 0 && com2int != 0)
                break;
            if(community_1[i].name==attackerStr)
                com1int=i;
            if(community_1[i].name==defenderStr)
                com1int=i;
            if(community_2[i].name==attackerStr)
                com2int=i;
            if(community_2[i].name==defenderStr)
                com2int=i;
        }
        if(turn_1) {
            one_round(community_1[com1int],community_2[com2int],community_1,community_2,special);
        }
        else {
            one_round(community_2[com2int],community_1[com1int],community_2,community_1,special);
        }
    }

}

void one_round(Character &attacker, Character &defender, vector<Character> &teamA, vector<Character> &teamD, const string &ifSpecial) {
    if(ifSpecial=="SPECIAL" && attacker.type != "Dwarfs") {
        if(attacker.type=="Elves") {
            special_elves(teamA,attacker); }
        else if(attacker.type=="Wizards") {
            special_wizards(teamA,attacker);
        }
        if(defender.operator<(attacker)) {
            defender.remainingHealth-=attacker.attack-defender.defense;
        }
    }
    else if (ifSpecial=="SPECIAL" && special_dwarfs(attacker) && defender.defense<2*attacker.attack) {
        defender.remainingHealth-= (2 * attacker.attack) -defender.defense;
    }
    else {
        if(defender.operator<(attacker)) {
            defender.remainingHealth-=attacker.attack-defender.defense;
        }
    }
    if(defender.remainingHealth<=0) {
        (turn_1) ? (lastKilled_2.push(defender)) : (lastKilled_1.push(defender));
        defender.isAlive=false;
        defender.remainingHealth=0;
    }

    for(int i=0;i<5;i++) {
        community_1[i].healthHistory[currentRound]=community_1[i].remainingHealth;
        community_2[i].healthHistory[currentRound]=community_2[i].remainingHealth;
        community_1[i].nRoundsSinceSpecial++;
        community_2[i].nRoundsSinceSpecial++;
    }

}
void special_elves(vector<Character> &community,Character &chr) {
    if(chr.type=="Elves" && chr.nRoundsSinceSpecial>=elves_special) {
        chr.nRoundsSinceSpecial=0;
        int conveyHealth=chr.remainingHealth;
        conveyHealth=conveyHealth/2;
        chr.remainingHealth=chr.remainingHealth-conveyHealth;
        bool findHobbit=true; int tmp=0;      //find hobbit to convey half of health
        while(findHobbit) {
            if(community[tmp].type=="Hobbit") {
                community[tmp].remainingHealth+=conveyHealth;
                findHobbit=false; }
            else {
                tmp++;}
        }
    }
}
bool special_dwarfs(Character &character) {
    if(character.type=="Dwarfs" && (character.nRoundsSinceSpecial>=dwarfs_special)) {
        character.nRoundsSinceSpecial=0;
        return true;
    }
    else {
        return false;
    }
}

void special_wizards(vector<Character> &community,Character &character) {
    if(character.type=="Wizards" && character.nRoundsSinceSpecial>=wizards_special ) {
        string lastDied="";
        if(turn_1) {
            if(!lastKilled_1.empty())
                lastDied=lastKilled_1.top().name;
        } else {
            if(!lastKilled_2.empty())
                lastDied=lastKilled_2.top().name;
        }
        bool findDead=true; int tmp=0;
        while(findDead && lastDied != "" && tmp <=5) {
            if(community[tmp].name==lastDied) {
                int initialHealth=community[tmp].healthHistory[0];
                community[tmp].remainingHealth=initialHealth;
                community[tmp].isAlive=true;
                character.nRoundsSinceSpecial=0;
                findDead=false;
            }
            tmp++;
        }
    }
}

bool is_war_end() {
    int deathNotHobbit=0;
    bool isHobbitDeath=false;
    if(turn_1) {
        for (int i = 0; i < 5; i++) {
            if (community_2[i].type=="Hobbits") {
                if( !community_2[i].isAlive)
                    isHobbitDeath=true;
            }
            else if(!community_2[i].isAlive)
                deathNotHobbit++;
        }
        if(isHobbitDeath || deathNotHobbit==4) {
            cout << "community 1 won" <<endl;
            return true;
        }
    }
    else {
        for (int i = 0; i < 5; i++) {
            if (community_1[i].type=="Hobbits") {
                if( !community_1[i].isAlive)
                    isHobbitDeath=true;
            }
            else if(!community_1[i].isAlive)
                deathNotHobbit++;
        }
        if(isHobbitDeath || deathNotHobbit==4) {
            cout << "community 2 won" <<endl;
            return true;
        }
    }
    if(currentRound==maxRoundGiven) {
        cout << "Draw" <<endl;
        cout << currentRound <<endl;
        return true;
    }


    return false;

}

void place_inputs() {
    //record 5 chars into community_1 and following 5 chars into community_2
    Character tmpChar=*new Character("","",0,0,0,0);
    for(int i=0;i<10;i++) {
        vector<string> tmp=all_input[i];
        Character toAssign=*new Character(tmp[0],tmp[1],stoi(tmp[2]),stoi(tmp[3]),stoi(tmp[4]),maxRoundGiven);
        tmpChar=toAssign;
        if (i<5)
            community_1.push_back(tmpChar);
        else
            community_2.push_back(tmpChar);
    }
    //record attacks into attackInfo
    int remaining=all_input.size();
    for(int i=10;i<remaining;i++) {
        string attacker=all_input[i][0];
        string defender=all_input[i][1];
        string special=all_input[i][2];
        pair<string,string> competitors=make_pair(attacker,defender);
        pair<pair<string,string>,string> round=make_pair(competitors,special);
        roundInfo.push(round);
    }
}














