// Text adventure game

#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <time.h>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

// The size of the maze
constexpr int kGridRows = 10;
constexpr int kGridCols = 10;

// Full map
char map[kGridRows][kGridCols];

// Blank map that is used to show the player where they have been
char searchMap[kGridRows][kGridCols];

int playerRow;
int playerCol;
int playerHealth = 10;
int playerMaxHealth = 10;
int playerAttack = 1;
int playerDefense = 1;
int potions = 0;
int enemiesDefeated = 0;
int goalsFound = 0;
int potionsTaken = 0;
int finalScore = 0;

void loadMap(char map[kGridRows][kGridCols], string fileName)
{
	ifstream input;
	input.open(fileName);

	for (int row = 0; row < kGridRows; row++)
	{
		for (int col = 0; col < kGridCols; col++)
		{
			input >> map[row][col];
		}
	}
}

void clearScreen()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	system("CLS");
#else
	cout << "\033[2J\033[1;1H"; cout << "\033[1;31m";
#endif
}

// Find the coordinate of the player
void findPlayer()
{
	for (int col = 0; col < kGridCols; col++)
	{
		for (int row = 0; row < kGridRows; row++)
		{
			if (map[row][col] == 'P')
			{
				playerRow = row;
				playerCol = col;
				return;
			}
		}
	}
}

// Prints empty map and fills it in as the player goes
void printMap()
{
	// to print player stats alongside map. stringstream is so I can concatenate the strings and ints
	stringstream statHealth;
	stringstream statAttack;
	stringstream statDefense;
	stringstream statPotions;
	stringstream statEnemies;
	stringstream statGoals;
	stringstream statPotionsTaken;
	statHealth << "Health: " << playerHealth << "/" << playerMaxHealth;
	statAttack << "Attack: " << playerAttack;
	statDefense << "Defense: " << playerDefense;
	statPotions << "Potions: " << potions;
	statEnemies << "Enemies Defeated: " << enemiesDefeated;
	statGoals << "Goals Found: " << goalsFound;
	statPotionsTaken << "Potions Taken: " << potionsTaken;
	string stats[8]{ "Stats: ", statHealth.str(), statAttack.str(), statDefense.str(), statPotions.str(), statEnemies.str(),
		statGoals.str(), statPotionsTaken.str() };

	cout << endl;
	for (int row = 0; row < kGridRows; row++)
	{
		for (int col = 0; col < kGridCols; col++)
		{
			if (searchMap[row][col] == '.')
				cout << " " << ", ";
			else
				cout << searchMap[row][col] << ", ";
		}
		if (row < 8)
		{
			cout << "		" << stats[row];
		}
		cout << endl;
	}
}

//DEBUG -- prints full map
void printFullMap()
{
	cout << endl;
	for (int row = 0; row < kGridRows; row++)
	{
		for (int col = 0; col < kGridCols; col++)
		{
			cout << map[row][col] << ", ";
		}
		cout << endl;
	}
}

// used when the player tries to walk into a wall
void wallText()
{
	cout << "There's a wall there, you can't move." << endl;
}

// used for the look function in-game
void lookWallText(string dir)
{
	cout << "To the " << dir << " you see a wall.";
}

//used for the look function in-game
void lookCorridorText(string dir)
{
	cout << "To the " << dir << " you see an empty corridor.";
}

void findRandomCell(char c)
{
	int randRow;
	int randCol;
	srand(time(NULL));
	do
	{
		randRow = rand() % kGridRows;
		randCol = rand() % kGridCols;
	} while (map[randRow][randCol] != '.'); // makes sure the cell is empty

	map[randRow][randCol] = c; // places char on that cell
}

void goalFound()
{
	printMap();
	cout << endl << "Congratulations! You've reached a goal." << endl;
	goalsFound++;
	bool statIncreased = false;
	char goalOption;

	while (statIncreased == false)
	{
		cout << "What would you like to train? Your [S]trength, [R]esilience or [V]itality? ";

		cin >> goalOption;
		goalOption = tolower(goalOption);

		if (goalOption == 's')
		{
			playerAttack += 2;
			statIncreased = true;
		}
		else if (goalOption == 'r')
		{
			playerDefense += 2;
			statIncreased = true;
		}
		else if (goalOption == 'v')
		{
			playerMaxHealth += 2;
			playerHealth += 2;
			statIncreased = true;
		}
	}

	findRandomCell('G');

	clearScreen();
}

struct highScore
{
	string name;
	int score;
};

vector<highScore> highScores;

void loadScores()
{
	ifstream scoresFile;

	scoresFile.open("saveScores.txt");

	for (int i = 0; i < 10; i++)
	{
		highScores.push_back(highScore());
		scoresFile >> highScores[i].name >> highScores[i].score;
	}

	scoresFile.close();
}

void saveScores()
{
	ofstream scoresFile;

	scoresFile.open("saveScores.txt");

	for (int i = 0; i < 10; i++)
	{
		scoresFile << highScores[i].name << " " << highScores[i].score << " ";
	}

	scoresFile.close();
}

void printHighScores()
{
	loadScores();

	for (int i = 0; i < 10; i++)
	{
		if (finalScore > highScores[i].score)
		{
			cout << "You got a high score!" << endl << "What's your name?" << endl;
			string name;
			do
			{
				cout << "Please enter 3 characters." << endl;
				cin >> name;
			} while (name.size() != 3);

			highScore newScore = { name, finalScore };
			highScores.insert(highScores.begin() + i, newScore);
			break;
		}
	}

	cout << "High Scores: " << endl;

	for (int i = 0; i < 10; i++)
	{
		cout << i + 1 << ": " << highScores[i].name << " - " << highScores[i].score << endl;
	}

	saveScores();
}

void endGame()
{
	finalScore = (enemiesDefeated * 2) + (goalsFound * 5);
	cout << endl << "Agh! You were defeated." << endl << "Final stats: " << endl;
	cout << "Goals reached: " << goalsFound << endl << "Enemies defeated: " << enemiesDefeated << endl;
	cout << "Attack: " << playerAttack << endl << "Defense: " << playerDefense << endl << "Potions taken: " << potionsTaken << endl;
	cout << endl << "Final Score: " << finalScore << endl;
	cout << "Thanks for playing!" << endl;
	printHighScores();
	char end;
	cin >> end;
	exit(0);
}

void takePotion()
{
	if (potions > 0)
	{
		int healthRecovered = 10 + (pow(enemiesDefeated, 0.7));
		playerHealth += healthRecovered;
		if (playerHealth > playerMaxHealth)
		{
			playerHealth = playerMaxHealth;
		}
		cout << "You used a potion and recovered " << healthRecovered << " health." << endl;
		potions--;
		potionsTaken++;
	}
	else
	{
		cout << "You don't have any potions!" << endl;
	}
}

void calculateDamageReceived(string enemy, int enemyAttack)
{
	int damageReceived = 0;
	if (rand() % 16 == 1)
	{
		damageReceived = (enemyAttack / playerDefense) * 3;
		cout << "The " << enemy << " attacks and deals " << damageReceived << " damage!" << endl;
		cout << "It was a critical hit!" << endl;
	}
	else
	{
		damageReceived = enemyAttack / playerDefense;
		cout << "The " << enemy << " attacks and deals " << damageReceived << " damage!" << endl;
	}
	playerHealth -= damageReceived;
}

void enemyBattle(string enemy, float attackMult, float defenseMult, float HealthMult)
{
	srand(time(NULL));

	int enemyHealth = HealthMult * (1 + enemiesDefeated);
	float enemyDefense = defenseMult * (1 + (enemiesDefeated / 2));
	float enemyAttack = attackMult * (1 + (enemiesDefeated / 2));
	bool isDefeated = false;
	int usedPotion = 0;

	while (isDefeated == false)
	{
		printMap();
		cout << endl << "Enemy Health: " << enemyHealth << endl;
		cout << "A " << enemy << " approaches! What will you do?" << endl << "[F]ight, [D]efend, use a [P]otion or [R]un? ";
		char enemyOption;
		cin >> enemyOption;
		enemyOption = tolower(enemyOption); // so I only need to put one character into the ifs

		clearScreen();

		int damageDealt; //damage dealt to enemy
		int damageReceived; //damage player receives from enemy

		if (enemyOption == 'f')
		{
			if (rand() % 16 == 1)
			{
				damageDealt = ((playerAttack / enemyDefense) + 1) * 3;
				cout << "You attack the " << enemy << " and deal " << damageDealt << " damage!" << endl;
				cout << "It was a critical hit!" << endl;
			}
			else
			{
				damageDealt = (playerAttack / enemyDefense) + 1;
				cout << "You attack the " << enemy << " and deal " << damageDealt << " damage!" << endl;
			}
			enemyHealth -= damageDealt;

			if (enemyHealth > 0)
			{
				calculateDamageReceived(enemy, enemyAttack);
			}
		}
		else if (enemyOption == 'd')
		{
			damageReceived = enemyAttack / (playerDefense * 3);
			playerHealth -= damageReceived;
			cout << "You tightened up your defenses!" << endl;
			cout << "The " << enemy << " attacks and deals " << damageReceived << " damage!" << endl;
		}
		else if (enemyOption == 'r')
		{
			cout << "You tried to run away!" << endl;
			if (rand() % 2 == 1)
			{
				cout << "You couldn't get away!" << endl;
				damageReceived = enemyAttack / playerDefense;
				playerHealth -= damageReceived;
				cout << "The " << enemy << " attacked and dealt " << damageReceived << " damage!" << endl;
			}
			else
			{
				cout << "You got away!" << endl;
				findRandomCell('E');
				break; //breaks out of enemy encounter
			}
		}
		else if (enemyOption == 'p')
		{
			//TODO: fix if no potion
			if (potions <= 0)
				cout << "You have no potions to take!" << endl;
			else
			{
				cout << "You swigged a potion! It will take another turn to set in!" << endl;
				usedPotion++;
			}
			calculateDamageReceived(enemy, enemyAttack);
		}

		int healthRecovered = 10 + (pow(enemiesDefeated, 0.7)); //calculation for health recovered in case of potion use
		if (enemyHealth <= 0 && playerHealth > 0)
		{
			cout << "Congratulations! You defeated the " << enemy << "." << endl;
			enemiesDefeated++;
			isDefeated = true;
			if (enemy == "Goblin" && rand() % 3 == 1)
			{
				cout << "Wow! You gained some experience! You feel your strength growing!" << endl;
				playerAttack++;
			}
			if (enemy == "Troll" && rand() % 3 == 1)
			{
				cout << "Wow! You gained some experience! You feel your vitality growing!" << endl;
				playerMaxHealth++;
				playerHealth++;
			}
			if (enemy == "Slime" && rand() % 3 == 1)
			{
				cout << "Wow! You gained some experience! You feel your resilience growing!" << endl;
				playerDefense++;
			}
			if (rand() % 10 == 1)
			{
				cout << "Woah! The " << enemy << " was holding a potion! You put it in your inventory." << endl;
				potions++;
			}
			findRandomCell('E');
		}
		if (playerHealth <= 0)
		{
			endGame();
		}

		if (usedPotion == 1)
		{
			usedPotion++;
		}
		else if (usedPotion == 2)
		{
			takePotion();
			usedPotion = 0;
		}
	}
}

void enemyEncounter()
{
	srand(time(NULL));

	int randEnemy = rand() % 3;

	if (randEnemy == 0)
	{
		enemyBattle("Goblin", 1.7, 0.8, 1.3);
	}
	else if (randEnemy == 1)
	{
		enemyBattle("Troll", 1.3, 1.3, 1.7);
	}
	else
	{
		enemyBattle("Slime", 0.8, 1.7, 1);
	}
}

void enemyMove()
{
	srand(time(NULL));

	vector<int> enemyRows(4, 0); // holds every enemy's position
	vector<int> enemyCols(4, 0);

	for (int col = 0; col < kGridCols; col++)
	{
		for (int row = 0; row < kGridRows; row++)
		{
			for (int i = 0; i < 3; i++)
			{
				if (map[row][col] == 'E' && map[row][col] != map[enemyRows[i]][enemyCols[i]])
				{
					int randNo = rand() % 4;

					enemyRows[i] = row;
					enemyCols[i] = col;

					if (randNo == 0 && map[row - 1][col] == '.')
					{
						map[row - 1][col] = 'E'; //change next spot to E
						map[row][col] = '.'; // change previous spot to blank
						enemyRows[i]--;
					}
					else if (randNo == 1 && map[row + 1][col] == '.')
					{
						map[row + 1][col] = 'E';
						map[row][col] = '.'; //needed on each else in case it doesn't move at all
						enemyRows[i]++;
					}
					else if (randNo == 2 && map[row][col + 1] == '.')
					{
						map[row][col + 1] = 'E';
						map[row][col] = '.';
						enemyCols[i]++;
					}
					else if (randNo == 3 && map[row][col - 1] == '.')
					{
						map[row][col - 1] = 'E';
						map[row][col] = '.';
						enemyCols[i]--;
					}
				}
			}
		}
	}
}

void chestFound()
{
	srand(time(NULL));
	cout << "You found a chest on the ground! ";

	int randNo = rand() % 20;

	if (randNo == 0)
	{
		cout << "A mysterious power flows into you. You feel your strength growing!" << endl;
		playerAttack++;
	}
	else if (randNo == 1)
	{
		cout << "A mysterious power flows into you. You feel your vitality growing!" << endl;
		playerMaxHealth++;
		playerHealth++;
	}
	else if (randNo == 2)
	{
		cout << "A mysterious power flows into you. You feel your resilience growing!" << endl;
		playerDefense++;
	}
	else if (randNo > 2 && randNo <= 10)
	{
		cout << "Woah! There's a potion inside! You put it in your inventory." << endl;
		potions++;
	}
	else
	{
		cout << "It turned out to be a mimic! The mimic attacks!" << endl;
		enemyBattle("Mimic", 1.5, 1.5, 1.5);
	}

	findRandomCell('C');
}

int main()
{
	loadMap(map, "Maze1.txt");
	findPlayer();

	while (true)
	{
		//just to shorten when I use these
		char playerPos = map[playerRow][playerCol];
		char north = map[playerRow - 1][playerCol];
		char east = map[playerRow][playerCol + 1];
		char south = map[playerRow + 1][playerCol];
		char west = map[playerRow][playerCol - 1];

		//updates the blank map
		searchMap[playerRow][playerCol] = playerPos;
		searchMap[playerRow - 1][playerCol] = north;
		searchMap[playerRow][playerCol + 1] = east;
		searchMap[playerRow + 1][playerCol] = south;
		searchMap[playerRow][playerCol - 1] = west;
		searchMap[playerRow + 1][playerCol + 1] = map[playerRow + 1][playerCol + 1];
		searchMap[playerRow + 1][playerCol - 1] = map[playerRow + 1][playerCol - 1];
		searchMap[playerRow - 1][playerCol + 1] = map[playerRow - 1][playerCol + 1];
		searchMap[playerRow - 1][playerCol - 1] = map[playerRow - 1][playerCol - 1];

		// printFullMap(); //DEBUG -- prints full map
		printMap();

		cout << endl << "What would you like to do?" << endl;

		cout << "[Q]uit, [N]orth, [E]ast, [S]outh, [W]est, [P]otion, [L]ook: ";

		// Read in a character representing the user command
		char option;
		cin >> option;
		option = tolower(option);

		clearScreen(); //clears text on command line

		if (option == 'n')
		{
			if (north == 'W') // if north of player is a wall, print wall text
				wallText();
			else
			{
				if (north == 'G') // if north of player is a goal...
					goalFound();

				else if (north == 'E') // if north of player is an enemy...
					enemyEncounter();

				else if (north == 'C')
				{
					chestFound();
				}

				map[playerRow - 1][playerCol] = 'P'; //change next spot to P
				map[playerRow][playerCol] = '.'; // change previous spot to blank
				playerRow--;
			}
		}
		else if (option == 'e')
		{
			if (east == 'W')
				wallText();
			else
			{
				if (east == 'G')
					goalFound();

				else if (east == 'E')
					enemyEncounter();

				else if (east == 'C')
				{
					chestFound();
				}

				map[playerRow][playerCol + 1] = 'P';
				map[playerRow][playerCol] = '.';
				playerCol++;
			}
		}
		else if (option == 's')
		{
			if (south == 'W')
				wallText();
			else
			{
				if (south == 'G')
					goalFound();

				else if (south == 'E')
					enemyEncounter();

				else if (south == 'C')
				{
					chestFound();
				}
				map[playerRow + 1][playerCol] = 'P';
				map[playerRow][playerCol] = '.';
				playerRow++;
			}
		}
		else if (option == 'w')
		{
			if (west == 'W')
				wallText();
			else
			{
				if (west == 'G')
					goalFound();

				else if (west == 'E')
					enemyEncounter();

				else if (west == 'C')
				{
					chestFound();
				}

				map[playerRow][playerCol - 1] = 'P';
				map[playerRow][playerCol] = '.';
				playerCol--;
			}
		}
		else if (option == 'l') //look
		{
			if (north == 'W')
				lookWallText("north");
			else
				lookCorridorText("north");
			if (east == 'W')
				lookWallText("east");
			else
				lookCorridorText("east");
			if (south == 'W')
				lookWallText("south");
			else
				lookCorridorText("south");
			if (west == 'W')
				lookWallText("west");
			else
				lookCorridorText("west");
			cout << endl;
		}
		else if (option == 'p')
		{
			takePotion();
		}
		else if (option == 'q')
		{
			endGame();
		}

		enemyMove();
	}
}