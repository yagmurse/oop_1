//Yagmur SELEK
//Id: 2017400273
#include "Character.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>



using namespace std;
int maxRoundGiven; //max round according to input.
int currentRound;
int elves_special=10;
int dwarfs_special=20;
int wizards_special=50;
vector<Character> community_1; //holds Characters for community 1
vector<Character> community_2; //holds Characters for community2
queue < pair < pair < string,string > ,string > > roundInfo; //pair<(attacker_name,defender_name),Ifspecial>, round informations given
void special_wizards(vector<Character> &community,Character &character);
void special_elves(vector<Character> &community,Character &chr);
bool special_dwarfs(Character &character);
bool is_war_end(ofstream& outFile); //check if one is the winner
void one_round(Character &attacker,Character &defender,vector<Character> &teamA, vector<Character> &teamD,string &ifSpecial); //play 1 round
void war_begin(ofstream& outFile);
bool turn_1; //does first community attacks, is it first's turn
bool sort_for_char(Character &c1,Character &c2);
vector<Character> sort_alph_order(vector<Character> &vec);

stack<Character> lastKilled_1; //last killed chars for community1
stack<Character> lastKilled_2; //last killed chars for community2
void deneme_print(ofstream& outFile, queue<string> com1, queue<string> com2); //prints the output to the out file



int main(int argc, char* argv[]) {
    queue<string> com_1_given_order;
    queue<string> com_2_given_order;


    if (argc != 3) {
        // cerr is used for errors
        cerr << "Run the code with the following command: g++ main.cpp Character.h Character.cpp -std=c++11 -o project1.out ./project1.out inputFile outputFile" << endl;
        return 1;
    }
    // Open the input and output files, check for failures
    ifstream inputFile(argv[1]);
    if (!inputFile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem opening \"" << argv[1] << "\" as input file" << endl;
        return 1;
    }

    ofstream outputFile(argv[2]);
    // outputFile.open("output.txt");
    if (!outputFile) {
        cerr << "There was a problem opening \"" << argv[2] << "\" as output file" << endl;
        return 1;
    }


    vector< vector< string > > all_input;
    string input;
    int currentlineNumber=0;

    //store input line by line to global vector all_input
    if (inputFile.is_open())
    {
        while (getline (inputFile,input))
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

        inputFile.close();
    }
    else { cout << "Unable to open input file"; }

    //place inputs into communities
    for(int i=0;i<10;i++) {
        vector<string> tmp=all_input[i];
        Character tmpChar=*new Character(tmp[0],tmp[1],stoi(tmp[2]),stoi(tmp[3]),stoi(tmp[4]),maxRoundGiven);
        if (i<5) {
            community_1.push_back(tmpChar);
            com_1_given_order.push(tmp[0]);
        }
        else {
            community_2.push_back(tmpChar);
            com_2_given_order.push(tmp[0]);
        }
    }

    //record attacks into attackInfo
    int remaining=all_input.size();
    for(int i=10;i<remaining;i++) {
        pair<string,string> competitors=make_pair(all_input[i][0],all_input[i][1]);
        pair<pair<string,string>,string> round=make_pair(competitors,all_input[i][2]);
        roundInfo.push(round);
    }
    //sort communities in alphabetical order
    community_1=sort_alph_order(community_1);
    community_2=sort_alph_order(community_2);


    war_begin(outputFile); //begin the war
    deneme_print(outputFile,com_1_given_order,com_2_given_order); //print the result to the outfile
    return 0;
}

void war_begin(ofstream& outFile) {
    currentRound=0;
    for(int i=0;i<5;i++) {
        community_1[i].healthHistory[0]=community_1[i].remainingHealth;
        community_2[i].healthHistory[0]=community_2[i].remainingHealth;
    }
    turn_1=true;
    while(!(is_war_end(outFile)) && ! roundInfo.empty() ) {
        currentRound++;
        turn_1=currentRound%2;
        pair<pair<string,string>,string> currentPair=roundInfo.front();
        roundInfo.pop();

        string attackerStr=currentPair.first.first;
        string defenderStr=currentPair.first.second;
        string special=currentPair.second;
        int com1int=0;  //decides the player index for community 1
        int com2int=0;  //decides the player index for community 2
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
        //change the player if given player is not alive for community 1 and 2 respectively
        if(!community_1[com1int].isAlive) {
            for(int i=1;i<5;i++) {
                int next=com1int+i;
                (next==5) ? (next=5) : (next=next%5);
                if(community_1[next].isAlive) {
                    com1int=next;
                    break;
                }
            }
        }
        if(!community_2[com2int].isAlive) {
            for(int i=1;i<5;i++) {
                int next=com2int+i;
                (next==5) ? (next=5) : (next=next%5);
                if(community_2[next].isAlive) {
                    com2int=next;
                    break;
                }
            }
        }
        if(turn_1) {
            //if community 1 attacks
            one_round(community_1[com1int],community_2[com2int],community_1,community_2,special);
        }
        else {
            //if community 2 attacks
            one_round(community_2[com2int],community_1[com1int],community_2,community_1,special);
        }
    }

}

void one_round(Character &attacker, Character &defender, vector<Character> &teamA, vector<Character> &teamD, string &ifSpecial) {
    if(ifSpecial=="SPECIAL" && attacker.type != "Dwarfs") {
        if(attacker.type=="Elves") {
            special_elves(teamA,attacker); }
        else if(attacker.type=="Wizards") {
            special_wizards(teamA,attacker);
        }
        if(defender.defense<attacker.attack) {
            defender.remainingHealth-=attacker.attack-defender.defense;
        }
    }
    else if (ifSpecial=="SPECIAL" && special_dwarfs(attacker) && defender.defense<attacker.attack) {
        defender.remainingHealth-= 2*(attacker.attack - defender.defense);
    }
    else {
        if(defender.defense<attacker.attack) {
            defender.remainingHealth-=attacker.attack-defender.defense;
        }
    }
    if(defender.remainingHealth<=0) {
        (turn_1) ? (lastKilled_2.push(defender)) : (lastKilled_1.push(defender));
        defender.isAlive=false;
        defender.remainingHealth=0;
        defender.nRoundsSinceSpecial=0;
    }

    for(int i=0;i<5;i++) {
        community_1[i].healthHistory[currentRound]=community_1[i].remainingHealth;
        community_2[i].healthHistory[currentRound]=community_2[i].remainingHealth;
        community_1[i].nRoundsSinceSpecial++;
        community_2[i].nRoundsSinceSpecial++;
    }

}
//Elves convey half of its life to Hobbits if conditions satisfied
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

//wizard gives the initial health for lastly killed member of its community
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

//check if hobbit is death or all other members are death for each community
bool is_war_end(ofstream& outFile) {
    bool result=false;
    int deathNotHobbit1=0;
    int deathNotHobbit2=0;
    bool isHobbitDeath1=false;
    bool isHobbitDeath2=false;
    for (int i = 0; i < 5; i++) {
        if (community_2[i].type=="Hobbits") {
            if( !community_2[i].isAlive)
                isHobbitDeath2=true;
        }
        else if(!community_2[i].isAlive)
            deathNotHobbit2++;
    }
    if(isHobbitDeath2 || deathNotHobbit2==4) {
        outFile << "Community-1" <<endl;
        outFile << currentRound <<endl;
        result = true;
    }

    for (int i = 0; i < 5; i++) {
        if (community_1[i].type=="Hobbits") {
            if( !community_1[i].isAlive)
                isHobbitDeath1=true;
        }
        else if(!community_1[i].isAlive)
            deathNotHobbit1++;
    }
    if(isHobbitDeath1 || deathNotHobbit1==4) {
        outFile << "Community-2" <<endl;
        outFile << currentRound <<endl;
        result=true;
    }

    if(currentRound==maxRoundGiven) {
        outFile << "Draw" <<endl;
        outFile << currentRound <<endl;
        result=true;
    }
    if(result) {
        int totalDied=deathNotHobbit1+deathNotHobbit2;
        if(isHobbitDeath1) {
            totalDied++;
        }
        if(isHobbitDeath2) {
            totalDied++;
        }
        outFile << totalDied <<endl;
    }

    return result;

}


bool sort_for_char(Character &c1,Character &c2) {
    return c1<c2;
}

vector<Character> sort_alph_order(vector<Character> &vec){
    int size=vec.size();
    sort(vec.begin(),vec.end(),sort_for_char);
    return vec;
}

void deneme_print(ofstream& outFile,queue<string> com1, queue<string> com2) {
    //do modifications to print the output in given name order and print
    for(int i=0;i<5;i++) {
        string s=com1.front();
        com1.pop();
        int number=0;
        for(int m=0;m<5;m++) {
            if(community_1[m].name==s) {
                number=m;
                break;
            }
        }
        outFile << community_1[number].name << " ";
        for(int n=0;n<currentRound+1;n++) {
            outFile <<  community_1[number].healthHistory[n] << " ";
        }
        outFile <<endl;

    }
    for(int i=0;i<5;i++) {
        string s=com2.front();
        com2.pop();
        int number=0;
        for(int m=0;m<5;m++) {
            if(community_2[m].name==s) {
                number=m;
                break;
            }
        }
        outFile << community_2[number].name << " ";
        for(int n=0;n<currentRound+1;n++) {
            outFile <<  community_2[number].healthHistory[n] << " ";
        }
        outFile <<endl;

    }


}