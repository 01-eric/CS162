#include "Item.h"
#include "Room.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <climits>
#include <vector>
#include <map>
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::flush;

void init(vector<Room*> &rooms);
int indexOf(char c, char cstr[], int occurrence);
char* substr(char cstr[], int start, int end);
vector<char*> split(char cstr[], char delim);
void empty(vector<char*> &v); // empties and dealloc all contents
void empty(vector<Room*> &v);
void empty(vector<Item*> &v);

int main() {
    vector<Room*> rooms;
    init(rooms);
    Room* location = rooms.front();
    char* name;
    vector<Item*> inventory;
    int health = 10;
    const int WIN_COND = 5;

    // welcome to Zuul introduction
    printf("Welcome to Zuul, a text based adventure game.\n");
    printf("In this map, there are %d rooms. Each room contains a healing potion and potentially an enemy.\n", rooms.size());
    printf("To win, you must bring %d enemy trophies to TABUU'S LAIR, and then defeat him in battle.\n", WIN_COND);
    printf("If you lose to any enemy in battle, the game is over.\nIf you beat an enemy, they turn into a grabbable trophy. \nBosses do not turn into trophies but instead just disappear.\n");
    cout << "Choose your player name: " << flush;
    char input[SHRT_MAX];
    cin.getline(input, SHRT_MAX);
    name = new char[strlen(input) + 1];
    strcpy(name, input);
    printf("Hello, %s, welcome to Zuul. Below are the game instructions.\n\n", name);
    printf("Keywords: ENTER - Go to the specified room. Type the room name after the command (e.g. ENTER Room 2, ENTER Skyworld, or ENTER Room 2: Skyworld)\n");
    printf("          STATUS - Get the status of your player; location, health, and inventory\n");
    printf("          USE - Apply a consumable item to yourself (e.g. USE Base Healer)\n");
    printf("          FIGHT - Fight the specified enemy or boss. Make sure you have higher health than the enemy! (e.g. FIGHT Kirby)\n");
    printf("          GRAB - Pick up the specified item. Can only do this to non-consumable non-enemy items. Enemies turn into grabbable trophies after battle (e.g. GRAB Link Trophy)\n");
    printf("          DROP - Dispense an item from your inventory into the current room (e.g. DROP Ike Trophy)\n");
    printf("          SUICIDE - Exit the game\n");
    location->print();

    while (true) { // break when quit or loss

        char command[SHRT_MAX];
        printf("P1-%s@Room-%c> ", name, location->getName()[5]); // accepting command
        cin.getline(command, SHRT_MAX);
        vector<char*> words = split(command, ' ');
        bool toBreak = false;

        // get keyword by words.front()
        if (strcmp(words.front(), "ENTER") == 0) { // how can I pass in string for const char*
            char* toEnter = substr(command, indexOf(' ', command, 1) + 1, strlen(command)); // command without keyword
            char* toEnterSubstr = substr(toEnter, 0, indexOf(' ', toEnter, 1)); // get first word in room to enter
            // access all exits in current room, check to see room is valid
            for (int i = 0; i < location->getExits().size(); i++) { // something wrong with iterator
                // cout << location->getExits().at(i) << std::endl;
                if (strcmp(toEnterSubstr, "Room") == 0) {
                    if (words.size() == 3) { // ENTER Room 1
                        char* toCompare = substr(location->getExits().at(i)->getName(), 0, indexOf(' ', location->getExits().at(i)->getName(), 2) - 1);
                        if (strcmp(toEnter, toCompare) == 0) {
                            location = location->getExits().at(i);
                            location->print(); // updated location
                            delete [] toCompare; // otherwise break skips next one
                            break;
                        } delete [] toCompare;
                    } else if (strcmp(toEnter, location->getExits().at(i)->getName()) == 0) { // ENTER Room 2: Skyworld
                        location = location->getExits().at(i);
                        location->print(); // updated location
                        break;
                    }
                } else { // ENTER Rayquaza's Lagoon
                    char* toCompare = substr(location->getExits().at(i)->getName(), indexOf(' ', location->getExits().at(i)->getName(), 2) + 1, strlen(location->getExits().at(i)->getName()));
                    if (strcmp(toEnter, toCompare) == 0) {
                        location = location->getExits().at(i);
                        location->print(); // updated location
                        delete [] toCompare;
                        break;
                    } delete [] toCompare;
                } // if reached here on last iteration exit does not exist
                if (i == location->getExits().size() - 1) cout << "\nRoom not found.\n" << endl;
            } delete [] toEnter;
            delete [] toEnterSubstr;
        }

        else if (strcmp(words.front(), "STATUS") == 0) {
            printf("\nLocation: %s\nHealth: %d\nInventory: ", location->getName(), health);
            if (inventory.size() == 0) cout << "Empty\n";
            else for (vector<Item*>::iterator i = inventory.begin(); i != inventory.end(); i++) cout << (*i)->getName() << (i == inventory.end() - 1 ? "\n" : ", ");
            cout << endl;
        }

        else if (strcmp(words.front(), "USE") == 0 || strcmp(words.front(), "FIGHT") == 0 || strcmp(words.front(), "GRAB") == 0) { // one of the use fight grab commands with items
            char* item = substr(command, indexOf(' ', command, 1) + 1, strlen(command)); // command without keyword
            for (vector<Item*>::iterator i = location->getItems()->begin(); i != location->getItems()->end(); i++) { // iterate through all items in room
                if (strcmp((*i)->getName(), item) == 0) { // if item found check item type compare to keyword
                    if (strcmp(words.front(), "USE") == 0 && (*i)->getType() == Item::CONSUM) { // if item is consumable
                        health += (*i)->getBoost(); // add health do not pick up
                        printf("\nUsed %s.\nHealth: %d.\n\n", item, health);
                    } else if (strcmp(words.front(), "FIGHT") == 0 && ((*i)->getType() == Item::ENEMY || (*i)->getType() == Item::BOSS)) {
                        health -= (*i)->getStrength(); // player loses health
                        if (health > 0) { // if won battle either delete boss or turn enemy into trophy
                            printf("\nYou won the battle against %s with %d remaining hp.\n", item, health);
                            if ((*i)->getType() == Item::ENEMY) { // enemy will turn into trophy
                                (*i)->switchType();
                                printf("You can now pick up %s.\n\n", (*i)->getName());
                            } else { // BOSS will disappear
                                if (strcmp(item, "TABUU") == 0) { // if you beat TABUU in battle, you win. Set break = true
                                    cout << "The dark energy holding the worlds together begins to fade." << endl;
                                    cout << "TABUU'S Lair shatters beneath your feet as you float in midair," << endl;
                                    cout << "wacthing all the worlds return to their original state." << endl;
                                    cout << "Congratulations, you've saved all the parallel worlds from destruction!" << endl;
                                    toBreak = true;
                                } printf("%s has disappeared.\n%s", item, (toBreak ? "\0" : "\n"));
                                delete *i; // delete item from room, boss disappears
                                location->getItems()->erase(i); // delete pointer
                            }
                        } else { // if you lost then go to quit prompti
                            printf("\nYou lost the battle against %s.\n", item);
                            toBreak = true;
                        } // to pick up an item remove it from the room and add to inventory
                    } else if (strcmp(words.front(), "GRAB") == 0 && (*i)->getType() == Item::DEF) {
                        inventory.push_back(*i); // add to inventory
                        printf("\n%s added to inventory.\n", (*i)->getName());
                        location->getItems()->erase(i); // delete from room BUT DO NOT delete *i BECAUSE INVENTORY STILL USING THAT MEMORY
                        cout << "Items in inventory: " << flush;
                        for (vector<Item*>::iterator i = inventory.begin(); i != inventory.end(); i++) cout << (*i)->getName() << (i == inventory.end() - 1 ? "\n" : ", ");
                        cout << endl; 
                    } else cout << "\nWrong item type, try a different keyword.\n" << endl; // if item found but no keyword matched
                    break; // if found item should break loop no matter what
                } // if reached here on last iteration item does not exist
                if (i == location->getItems()->end() - 1) printf("\n%s not found in this room.\n\n", item); // otherwise item not in this room
            } delete [] item;
        }

        else if (strcmp(words.front(), "DROP") == 0) {
            char* item = substr(command, indexOf(' ', command, 1) + 1, strlen(command)); // command without keyword
            for (vector<Item*>::iterator i = inventory.begin(); i != inventory.end(); i++) { // iterate through inventory
                if (strcmp((*i)->getName(), item) == 0) { // if found item drop in room
                    location->getItems()->push_back(*i); // drop in room
                    printf("\nDropped %s in %s.\n", item, location->getName());
                    inventory.erase(i); // remove from inventory and DON'T delete *i pointer is still being used
                    if (location == rooms.at(17) && location->getItems()->size() == 5) {
                        bool tabuuDNE = true; // checks to see if tabuu already exists
                        for (vector<Item*>::iterator j = location->getItems()->begin(); j != location->getItems()->end(); j++) if (strcmp((*j)->getName(), "TABUU") == 0) tabuuDNE = false;
                        if (tabuuDNE) location->getItems()->push_back(new Item(Item::BOSS, new char[6]{'T','A','B','U','U','\0'}, 0, 500));                        
                        cout << "The doors to hell creak open as you sense a supernatural presence." << endl;
                    } cout << "Items in the room: " << flush;
                    for (vector<Item*>::iterator j = location->getItems()->begin(); j != location->getItems()->end(); j++) cout << (*j)->getName() << (j == location->getItems()->end() - 1 ? "\n" : ", ");
                    cout << endl; 
                    break;
                } if (i == inventory.end() - 1) printf("\n%s not found in inventory.\n\n", item); // if reached here then item not found
            } delete [] item;
        }

        else if (strcmp(words.front(), "SUICIDE") == 0) {
            toBreak = true;
        }

        empty(words); // dealloc split vector
        if (toBreak == true) break;

    }
    
    // dealloc all rooms, inventory, player name at end
    empty(rooms);
    empty(inventory);
    delete [] name;
    cout << "Game over." << endl;
}

// returns nth occurence of c in cstr
int indexOf(char c, char cstr[], int occurrence) {
    int count = 0;
    for (int i = 0; i < strlen(cstr); i++) if (cstr[i] == c && ++count == occurrence) return i;
    return -1;
}

// remember to dealloc when calling this method
char* substr(char cstr[], int start, int end) {
    char* substr = new char[end - start + 1]; // allocate new cstring of 1 more than end - start
    for (int i = 0; i < end - start; i++) substr[i] = cstr[start + i]; // copy character by character
    substr[end - start] = '\0'; // why do I have to do this
    return substr;
}

// splits cstr[] by delim and returns array of cstring
// must dealloc all elements of vector after calling
vector<char*> split(char cstr[], char delim) {
    vector<char*> tokens;
    int count = 0;
    do { // use do while because first one returns -1
        int nextIndex = indexOf(delim, cstr, count + 1);
        tokens.push_back(substr(cstr, indexOf(delim, cstr, count) + 1, nextIndex == -1 ? strlen(cstr) : nextIndex)); 
    } while (indexOf(delim, cstr, ++count) != -1);
    return tokens;
}

// empty vector in similar manner
// to delete [], deletes all allocated
// memory for each element in vector
void empty(vector<char*> &v) {
    while(v.size() > 0) {
        delete [] v.front();
        v.erase(v.begin());
    }
}

void empty(vector<Room*> &v) { 
    while(v.size() > 0) {
        delete v.front();
        v.erase(v.begin());
    }
}

void empty(vector<Item*> &v) {
    while(v.size() > 0) {
        delete v.front();
        v.erase(v.begin());
    }
}

// map of the Great Maze in Brawl
// all rooms contain healing potion except TABUU'S LAIR
// ENEMIES and BOSSES can only be fought with sufficient health
// ENEMIES turn into trophies that you can pick up after fought
// Defeat 1 boss and bring at least 5 trophies and drop them in TABUU'S LAIR to awaken TABUU
// FIGHT TABUU AND WIN to win, FIGHT ANY ENEMY AND LOSE to lose.
void init(vector<Room*> &rooms) {
    char name[SHRT_MAX], description[SHRT_MAX], item[SHRT_MAX];

    strcpy(name, "Room 1: the Base");
    strcpy(description, "The strongest healing item is here. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Base Healer");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 250, 0));

    strcpy(name, "Room 2: Skyworld");
    strcpy(description, "The ground seems to be made of clouds strong enough to walk on! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 3: Pit's Chamber");
    strcpy(description, "An enemy awaits you high up in the sky. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Pit");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 25));

    strcpy(name, "Room 4: Kirby & Link's Chamber");
    strcpy(description, "What, two enemies? ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Kirby");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 25));
    strcpy(item, "Link");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 15));

    strcpy(name, "Room 5: The Forest Area 1");
    strcpy(description, "It's awfully easy to get lost in here. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 6: The Forest Area 2");
    strcpy(description, "It's awfully easy to get lost in here. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 7: Diddy Kong's Jungle");
    strcpy(description, "Who's littering with all these banana peels? ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Diddy Kong");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 30));

    strcpy(name, "Room 8: Duon's Chamber");
    strcpy(description, "2 on 1? Who designed this game? "); 
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Duon");
    rooms.back()->addItem(new Item(Item::BOSS, item, 0, 60));

    strcpy(name, "Room 9: Meta Knight's Ship");
    strcpy(description, "\"This is your captain speaking... You're dead.\" ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Meta Knight");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 40));

    strcpy(name, "Room 10: Snake's Secret Room");
    strcpy(description, "It's showtime. ");
    rooms.push_back(new Room(name, description));    
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Snake");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 35));

    strcpy(name, "Room 11: Game & Watch's Flat World");
    strcpy(description, "What happened to the third dimension? ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Mr. Game & Watch");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 30));

    strcpy(name, "Room 12: Ness's Hideout");
    strcpy(description, "This place is a wasteland! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Ness");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 15));

    strcpy(name, "Room 13: The Ruined Zoo");
    strcpy(description, "There seems to be no trace of life in here. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 14: Porky's New Pork City");
    strcpy(description, "Did you remember your healing potions? ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Porky");
    rooms.back()->addItem(new Item(Item::BOSS, item, 0, 60));

    strcpy(name, "Room 15: Captain Falcon's Crash Site");
    strcpy(description, "Someone's a little mad that they crashed their favorite racing machine. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Captain Falcon");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 15));

    strcpy(name, "Room 16: Olimar's Planet");
    strcpy(description, "It's humid in here. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Olimar");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 30));

    strcpy(name, "Room 17: Donkey Kong's Jungle");
    strcpy(description, "Ride the updraft! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Donkey Kong");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 20));
    strcpy(item, "Samus");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 15));

    strcpy(name, "Room 18: TABUU'S LAIR");
    strcpy(description, "A supernatural force lies dormant. ");
    rooms.push_back(new Room(name, description));

    strcpy(name, "Room 19: Pikachu's Cage");
    strcpy(description, "He's been tortured for long enough, now he wants revenge. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Pikachu");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 35));

    strcpy(name, "Room 20: Ridley's Lair");
    strcpy(description, "\"Havent I killed you like... twice already?\" ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Ridley");
    rooms.back()->addItem(new Item(Item::BOSS, item, 0, 60));
    strcpy(item, "Meta Ridley");
    rooms.back()->addItem(new Item(Item::BOSS, item, 0, 80));

    strcpy(name, "Room 21: R.O.B's Lab");
    strcpy(description, "Beware the laser attacks! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "R.O.B");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 30));

    strcpy(name, "Room 22: The Research Facility");
    strcpy(description, "Look at all these failed experiments! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 23: Mario World");
    strcpy(description, "This place looks oddly familiar. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Mario");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 15));

    strcpy(name, "Room 24: Bowser's Castle");
    strcpy(description, "Home to the greediest of all kings.");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Bowser");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 20));

    strcpy(name, "Room 25: King Dedede's Castle");
    strcpy(description, "Bowser who? ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "King Dedede");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 25));

    strcpy(name, "Room 26: Falco's Escape");
    strcpy(description, "Where did he go? ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Falco");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 35));

    strcpy(name, "Room 27: The Lake");
    strcpy(description, "The piranhas are dangerous. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 28: Fox's Escape");
    strcpy(description, "Catch him if you can! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Fox");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 35));

    strcpy(name, "Room 29: The Cave Area 1");
    strcpy(description, "It's really dark! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 30: The Cave Area 2");
    strcpy(description, "It's really dark! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 31: Galleom's Cave");
    strcpy(description, "An enemy that must stay dead. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Galleom");
    rooms.back()->addItem(new Item(Item::BOSS, item, 0, 60));

    strcpy(name, "Room 32: Wario's Warehouse");
    strcpy(description, "Wait, is Lucas in here too? ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Wario");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 30));
    strcpy(item, "Lucas");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 15));

    strcpy(name, "Room 33: Pokemon Stadium & Zelda's Temple");
    strcpy(description, "It's time to catch 'em all! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Pokemon Trainer");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 20));
    strcpy(item, "Zelda");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 15));

    strcpy(name, "Room 34: Peach's Lakeshore");
    strcpy(description, "The breeze feels kind of nice.");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Peach");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 20));

    strcpy(name, "Room 35: The Path to the Ruins");
    strcpy(description, "Two roads diverged in a yellow wood... ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));

    strcpy(name, "Room 36: Luigi's Mansion");
    strcpy(description, "Spooky... ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Luigi");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 20));

    strcpy(name, "Room 37: Rayquaza's Lagoon");
    strcpy(description, "Wait, Pokemon Stadium wasn't enough? ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Rayquaza");
    rooms.back()->addItem(new Item(Item::BOSS, item, 0, 35));

    strcpy(name, "Room 38: Ice Climbers' Cliff");
    strcpy(description, "Lucario stands at the top giving you the death stare. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Ice Climbers");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 35));
    strcpy(item, "Lucario");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 25));

    strcpy(name, "Room 39: Ike's Battlefield");
    strcpy(description, "Oh shit. Run. ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Ike");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 100));

    strcpy(name, "Room 40: Marth & Ganondorf's Castle");
    strcpy(description, "Grab them from the siege! ");
    rooms.push_back(new Room(name, description));
    strcpy(item, "Healing Potion");
    rooms.back()->addItem(new Item(Item::CONSUM, item, 10, 0));
    strcpy(item, "Marth");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 35));
    strcpy(item, "Ganondorf");
    rooms.back()->addItem(new Item(Item::ENEMY, item, 0, 10));

    // add exits at the end because need full map made first
    rooms.at(0)->addExit(rooms.at(1));
    rooms.at(0)->addExit(rooms.at(36));
    rooms.at(0)->addExit(rooms.at(37));
    rooms.at(1)->addExit(rooms.at(0));
    rooms.at(1)->addExit(rooms.at(2));
    rooms.at(1)->addExit(rooms.at(3));
    rooms.at(2)->addExit(rooms.at(1));
    rooms.at(3)->addExit(rooms.at(1));
    rooms.at(3)->addExit(rooms.at(4));
    rooms.at(4)->addExit(rooms.at(3));
    rooms.at(4)->addExit(rooms.at(5));
    rooms.at(5)->addExit(rooms.at(4));
    rooms.at(5)->addExit(rooms.at(6));
    rooms.at(6)->addExit(rooms.at(5));
    rooms.at(6)->addExit(rooms.at(7));
    rooms.at(6)->addExit(rooms.at(13));
    rooms.at(7)->addExit(rooms.at(6));
    rooms.at(7)->addExit(rooms.at(8));
    rooms.at(7)->addExit(rooms.at(14));
    rooms.at(7)->addExit(rooms.at(16));
    rooms.at(8)->addExit(rooms.at(7));
    rooms.at(8)->addExit(rooms.at(9));
    rooms.at(8)->addExit(rooms.at(11));
    rooms.at(9)->addExit(rooms.at(8));
    rooms.at(9)->addExit(rooms.at(10));
    rooms.at(10)->addExit(rooms.at(9));
    rooms.at(11)->addExit(rooms.at(8));
    rooms.at(11)->addExit(rooms.at(12));
    rooms.at(12)->addExit(rooms.at(11));
    rooms.at(12)->addExit(rooms.at(13));
    rooms.at(13)->addExit(rooms.at(6));
    rooms.at(13)->addExit(rooms.at(12));
    rooms.at(14)->addExit(rooms.at(7));
    rooms.at(14)->addExit(rooms.at(15));
    rooms.at(15)->addExit(rooms.at(14));
    rooms.at(16)->addExit(rooms.at(7));
    rooms.at(16)->addExit(rooms.at(17));
    rooms.at(16)->addExit(rooms.at(18));
    rooms.at(17)->addExit(rooms.at(16));
    rooms.at(17)->addExit(rooms.at(39));
    rooms.at(18)->addExit(rooms.at(16));
    rooms.at(18)->addExit(rooms.at(19));
    rooms.at(19)->addExit(rooms.at(18));
    rooms.at(19)->addExit(rooms.at(20));
    rooms.at(19)->addExit(rooms.at(21));
    rooms.at(19)->addExit(rooms.at(22));
    rooms.at(20)->addExit(rooms.at(19));
    rooms.at(21)->addExit(rooms.at(19));
    rooms.at(22)->addExit(rooms.at(19));
    rooms.at(22)->addExit(rooms.at(23));
    rooms.at(22)->addExit(rooms.at(27));
    rooms.at(22)->addExit(rooms.at(28));
    rooms.at(23)->addExit(rooms.at(22));
    rooms.at(23)->addExit(rooms.at(24));
    rooms.at(24)->addExit(rooms.at(23));
    rooms.at(24)->addExit(rooms.at(25));
    rooms.at(25)->addExit(rooms.at(24));
    rooms.at(25)->addExit(rooms.at(26));
    rooms.at(25)->addExit(rooms.at(27));
    rooms.at(26)->addExit(rooms.at(25));
    rooms.at(27)->addExit(rooms.at(22));
    rooms.at(27)->addExit(rooms.at(25));
    rooms.at(28)->addExit(rooms.at(22));
    rooms.at(28)->addExit(rooms.at(29));
    rooms.at(29)->addExit(rooms.at(28));
    rooms.at(29)->addExit(rooms.at(30));
    rooms.at(30)->addExit(rooms.at(29));
    rooms.at(30)->addExit(rooms.at(31));
    rooms.at(31)->addExit(rooms.at(30));
    rooms.at(31)->addExit(rooms.at(32));
    rooms.at(31)->addExit(rooms.at(34));
    rooms.at(31)->addExit(rooms.at(35));
    rooms.at(31)->addExit(rooms.at(37));
    rooms.at(32)->addExit(rooms.at(31));
    rooms.at(32)->addExit(rooms.at(33));
    rooms.at(32)->addExit(rooms.at(34));
    rooms.at(33)->addExit(rooms.at(32));
    rooms.at(34)->addExit(rooms.at(31));
    rooms.at(34)->addExit(rooms.at(32));
    rooms.at(35)->addExit(rooms.at(31));
    rooms.at(35)->addExit(rooms.at(36));
    rooms.at(36)->addExit(rooms.at(0));
    rooms.at(36)->addExit(rooms.at(35));
    rooms.at(37)->addExit(rooms.at(0));
    rooms.at(37)->addExit(rooms.at(31));
    rooms.at(37)->addExit(rooms.at(38));
    rooms.at(38)->addExit(rooms.at(37));
    rooms.at(38)->addExit(rooms.at(39));
    rooms.at(39)->addExit(rooms.at(17));
    rooms.at(39)->addExit(rooms.at(38));

}
