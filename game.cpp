#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>

using namespace std;

class Entity;

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

    Hand(Hand* ph) {
        entity = ph->entity;
        amount = ph->amount;
        cards.push_back(ph->cards[1]);
    }

    int ReturnTotal() {
        int result = 0;
        for(int i = 0; i < cards.size(); i++) {
            if(((int)cards[i][0] -'0') < 10) {
                result += (int)cards[i][0] -'0';
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

    int GetAmount() {
        return amount;
    }
};

class Entity {
public:
    stack<Hand*> hands;
    string name;
    int balance;
    bool liveplay;
    
    void hit(string card) {
        hands.top()->cards.push_back(card);
    }

    string GetName() {
        return name;
    }

    bool IsBust() {
        if(hands.top()->ReturnTotal() > 21) {
            return true;
        }
        else {
            return false;
        }
    }

    bool CloseHand() {
        if(IsBust()) {
            hands.pop();
            return false;
        }
        else {
            hands.pop();
            return true;
        }
    }


    Hand* Stand() {
        Hand* temp = hands.top();
        CloseHand();
        cout << "Player stands" << endl;
        return temp;
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
    bool livesplit;
    bool livedouble;

public:
    Player(string n) {
        name = n;
        balance = 1000;
    }

    void join() {
        int amount;
        do {
            cout << "Current balance: " << balance  << endl;
            cout << "Player " << name << " please enter valid bet amount: ";
            cin  >> amount;
        } while(amount > balance);
        balance -= amount;
        Hand* h = new Hand(this, amount);
        hands.push(h);
    }

    string IsDealt() {
        string x = "to the player " + name;
        return x;
    }

    void CalcLiveDouble() {
        if((hands.top()->amount) <= balance) {
            if(hands.top()->cards.size() <= 2) {
                livedouble = true;
                return;
            }
        }
        livedouble = false;
    }

    void CalcLiveSplit() {
        if(hands.top()->amount <= balance) {
            if(hands.top()->cards.size() == 2) {
                if(hands.top()->cards[0][0] == hands.top()->cards[1][0]) {
                    livesplit = true;
                    return;
                }
            }
        }
        livesplit = false;
    }

    bool getLiveDouble() {
        return livedouble;
    }

    bool getLiveSplit() {
        return livesplit;
    }

    void Surrender() {
        int change = hands.top()->GetAmount();
        balance += (change / 2);
        CloseHand();
    }

    bool HasBlackjack() {
        if(hands.top()->ReturnTotal() == 21) {
            balance += hands.top()->GetAmount() * 2.5;
            CloseHand();
            return true;
        }
        else {
            return false;
        }
    }

    bool DoubleBet() {
        if(livedouble) {
            balance -= hands.top()->amount;
            hands.top()->amount = hands.top()->amount * 2;
            return true;
        }
        return false;
    }

    bool Split() {
        if(livesplit) {
            balance -= hands.top()->amount;
            Hand* temp = new Hand(hands.top());
            hands.top()->cards.pop_back();
            hands.push(temp);
            return true;
        }
        return false;
    }
};


class Game {
private:
    vector<Player*> players;
    vector<string> deck;
    Dealer* dealer;
    stack<Hand*> closedHands;
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
            players[i]->HasBlackjack();
        }
        for(int i = 0; i < players.size(); i++) {
            while(!(players[i]->hands.empty())) {
                players[i]->CalcLiveDouble();
                players[i]->CalcLiveSplit();
                cout << players[i]->GetName() << " to play: ";
                players[i]->ShowHand();
                cout << "Dealers Hand: ";
                dealer->ShowHand();
                cout << "Press [0] to hit" << "  " << "Press [1] to stand" << "  " << "Press [2] to surrender" << "  ";
                if(players[i]->getLiveDouble()) {
                    cout << "Press [3] to double" << "  ";
                }
                if(players[i]->getLiveSplit()) {
                    cout << "Press [4] to split";
                }
                cout << endl;
                static int a;
                cin >> a;
                switch (a) {
                    case 0:
                        Deal(players[i]);
                        if(players[i]->IsBust()) {
                            cout << "Hand busts" << endl;
                            players[i]->CloseHand();
                        }
                        break;
                    case 1:
                        closedHands.push(players[i]->Stand());
                        break;
                    case 2:
                        players[i]->Surrender();
                        break; 
                    case 3:
                        if(players[i]->DoubleBet()) {
                            Deal(players[i]);
                            if(!players[i]->IsBust()) {
                                closedHands.push(players[i]->Stand());
                            }
                        }
                        break;
                    case 4:
                        players[i]->Split();
                        break;
                    default:
                        cout << "Lol";
                }
            }
        }
        cout << "Facedown card was a: " << dealer->hands.top()->cards[1] << endl;
        if(dealer->hands.top()->ReturnTotal() == 21) {
            cout << "Dealer natural blackjack" << endl;
            return;
        }
        while(dealer->hands.top()->ReturnTotal() < 17) {
            Deal(dealer);
            if(dealer->hands.top()->ReturnTotal() > 21) {
                cout << "Dealer busts" << endl;
                int n = closedHands.size();
                for(int i = 0; i < n; i++) {
                    closedHands.top()->entity->balance += (closedHands.top()->amount * 2);
                    cout << "Player " << closedHands.top()->entity->name << " wins " << closedHands.top()->GetAmount() * 2 << endl;
                    closedHands.pop();
                }
                return;
            }
        }
        int n = closedHands.size();
        for(int i = 0; i < n; i++) {
            if(closedHands.top()->ReturnTotal() < dealer->hands.top()->ReturnTotal()) {
                closedHands.top()->entity->balance += closedHands.top()->amount * 2;
                cout << "Player " << closedHands.top()->entity->name << " wins " << closedHands.top()->GetAmount() * 2 << endl;
            }
            else if(closedHands.top()->ReturnTotal() == dealer->hands.top()->ReturnTotal()) {
                closedHands.top()->entity->balance += closedHands.top()->amount;
                cout << "Player " << closedHands.top()->entity->name << " is pushed " << closedHands.top()->GetAmount() << endl;
            }
            closedHands.pop();
        }
    } 

    void RegisterPlayer() {
        cout << "Enter player name: ";
        string name;
        cin >> name;
        Player* p = new Player(name);
        players.push_back(p);
        cout << "Player " << p->GetName() << " successfully registered" << endl;
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

int main() {
    Game g;
    g.RegisterPlayer();
    g.RegisterPlayer();
    g.PlayRound();
    g.PlayRound();
}