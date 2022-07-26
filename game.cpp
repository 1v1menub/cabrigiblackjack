#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>

using namespace std;


class Game {
private:
    vector<Player*> players;
    vector<string> deck;
    Dealer* dealer;
    stack<Hand> closedHands;
public:

    Game() {
        dealer = new Dealer();
    }

    void PlayRound() {
        cout << "Placing bets" << endl;
        dealer->join();
        for(int i = 0; i < players.size(); i++) {
            players[i]->join();
        }
        cout << "The round has begun" << endl;
        Shuffle();
        for(int i = 0; i < players.size(); i++) {
            Deal(players[i]);
        }
        Deal(dealer);
        for(int i = 0; i < players.size(); i++) {
            Deal(players[i]);
        }
        DealFD(dealer);
        for(int i = 0; i < players.size(); i++) {
            while(!(players[i]->hands.empty())) {
                cout << players[i]->GetName() << " to play: " << endl << "Press [0] to hit" << "  " << "Press [1] to stand" << "  " << "Press [2] to surrender";
                if(players[i]->livedouble) {
                    cout << "  " << "Press [3] to double" << "  ";
                }
                if(players[i]->livesplit) {
                    cout << "  " << "Press [4] to split";
                }
                int a;
                cin >> a;
                switch (a) {
                    // case 1:
                    //     return 1;
                    // case 2:
                    //     sentencias_2
                    // //Resto de bloques case
                    // case 3:
                    //     sentencias_n
                    // default:
                    //     sentencias_default;
                }
            }
        }
    } 

    void RegisterPlayer() {
        cout << "Enter player name: ";
        string name;
        cin >> name;
        Player* p = new Player(name);
        players.push_back(p);
        cout << "Player" << p->GetName() << " successfully registered" << endl;
    }

    void Shuffle() {
        deck = {"AS", "2S", "3S", "4S", "5S", "6S", "7S", "8S", "9S", "TS", "JS", "QS", "KS", "AD", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "TD", "JD", "QD", "KD", "KC", "QC", "JC", "TC", "9C", "8C", "7C", "6C", "5C", "4C", "3C", "2C", "AC", "KH", "QH", "JH", "TH", "9H", "8H", "7H", "6H", "5H", "4H", "3H", "2H", "AH"};
        srand(time(NULL));
        for(int i = 0; i < 52; i++) {
            int x = rand() % 52;
            swap(deck[i], deck[x]);
        }
    }

    string Deal(Entity* p) {
        string card = deck.back();
        deck.pop_back();
        cout << card << " is dealt " << p->IsDealt() << endl;
        p->hit(card);
        return card;
    }

    string DealFD(Dealer* p) {
        string card = deck.back();
        deck.pop_back();
        cout << "A card is dealt face down to the dealer" << endl;
        p->hit(card);
        return card;
    }
};

struct Hand {
    Entity* entity;
    int amount;
    vector<string> cards;

    Hand(Entity* e) {
        entity = e;
    }

    Hand(Entity* e, int a) {
        entity = e;
        amount = a;
    }

    int ReturnTotal() {
        int result = 0;
        for(int i = 0; i < cards.size(); i++) {
            if((int)(cards[i][0]) < 10) {
                result += (int)(cards[i][0]);
            }
            else if(cards[i][0] == 'T' || cards[i][0] == 'J' || cards[i][0] == 'Q' || cards[i][0] == 'K') {
                result += 10;
            }
            else {
                if(result <= 10) {
                    result += 11;
                }
                else {
                    result += 1;
                }
            }
        }
        return result;
    }

};


class Entity {
public:
    stack<Hand*> hands;
    string name;
    bool livesplit;
    bool livedouble;
    bool liveplay;
    
    void hit(string card) {
        hands.top()->cards.push_back(card);
    }

    string GetName() {
        return name;
    }

    void ShowHand() {
        for(int i = 0; i < hands.top()->cards.size(); i++) {
            cout << hands.top()->cards[i] << " ";
        }
        cout << endl;
    }

    virtual string IsDealt() = 0;
};

class Dealer : public Entity {
public:
    Dealer() {
        name = "Dealer";
    }

    void join() {
        Hand* h = new Hand(this);
        hands.push(h);
    }

    string IsDealt() {
        return "to the dealer";
    }

};

class Player : public Entity{
private:
    int balance;

public:
    Player(string n) {
        name = n;
        balance = 1000;
    }

    void join() {
        cout << "Player " << name << " please enter bet amount: ";
        int amount;
        cin  >> amount;
        Hand* h = new Hand(this, amount);
        hands.push(h);
    }

    string IsDealt() {
        string x = "to the player " + name;
        return x;
    }

    // void checksplit() {
    //     if(livehand->size() == 2) {
    //         if(livehand[0][0] == livehand[1][0]) {
    //             livesplit = true;
    //         }
    //     }
    // }

    // bool split() {
    //     if(checksplit) {
    //         cout << "Player splits " << livehand[0][0] << 's' <<endl;
    //         string scard = livehand->back();
    //         livehand->pop_back();

    //         return true;
    //     }
    //     else {
    //         cout << "Player cannot split" << endl;
    //         return false;
    //     }
    // }
};


void game() {
}

int main() {
}

