#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <filesystem>
#include <windows.h>
#include <map>

using namespace std;


enum GameState { MENU, DIFFICULTY_SELECT, IN_GAME, GAME_OVER, HIGH_SCORES };


class Player {
public:
    int score = 0;
    int correctGuesses = 0;
    int incorrectGuesses = 0;
    vector<char> guessedChars;
    string name;

    void addCorrectGuess() {
        correctGuesses++;
        score += 10;
    }

    void addIncorrectGuess() {
        incorrectGuesses++;
        score = max(0, score - 5);
    }

    void reset() {
        correctGuesses = 0;
        incorrectGuesses = 0;
        guessedChars.clear();
    }

    bool hasGuessed(char c) {
        return find(guessedChars.begin(), guessedChars.end(), c) != guessedChars.end();
    }
};


string getRandomWord(const vector<string>& words) {
    int index = rand() % words.size();
    return words[index];
}

bool containsChar(const string& word, char guess) {
    return word.find(tolower(guess)) != string::npos;
}


string drawHangman(int incorrectGuesses, int maxAttempts) {
   
    int stage = 0;
    if (incorrectGuesses > 0) {
        stage = (incorrectGuesses * 7 + maxAttempts - 1) / maxAttempts; 
        stage = min(stage, 7); 
    }

    string hangman;
    if (stage >= 1) {
        hangman = "  ____\n |    |\n |\n |\n |\n_|_\n";
    }
    if (stage >= 2) {
        hangman = "  ____\n |    |\n |    O\n |\n |\n_|_\n";
    }
    if (stage >= 3) {
        hangman = "  ____\n |    |\n |    O\n |    |\n |\n_|_\n";
    }
    if (stage >= 4) {
        hangman = "  ____\n |    |\n |    O\n |   /|\n |\n_|_\n";
    }
    if (stage >= 5) {
        hangman = "  ____\n |    |\n |    O\n |   /|\\\n |\n_|_\n";
    }
    if (stage >= 6) {
        hangman = "  ____\n |    |\n |    O\n |   /|\\\n |   /\n_|_\n";
    }
    if (stage >= 7) {
        hangman = "  ____\n |    |\n |    O\n |   /|\\\n |   / \\\n_|_\n";
    }
    return hangman;
}

void displayWord(const string& word, const vector<char>& guessedChars) {
    for (char letter : word) {
        if (find(guessedChars.begin(), guessedChars.end(), tolower(letter)) != guessedChars.end()) {
            cout << static_cast<char>(tolower(letter)) << " ";  
        }
        else {
            cout << "_ ";
        }
    }
    cout << endl;
}

void displayMenu(GameState& state) {
    system("cls");
    cout << "========================================\n";
    cout << "      VITAJ V HRE HANGMAN (SIBENICA)\n";
    cout << "========================================\n\n";
    cout << "1. Nova hra\n";
    cout << "2. Najlepsie vysledky\n";
    cout << "3. Koniec\n\n";
    cout << "Vyber moznost (1-3): ";

    int choice;
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        state = DIFFICULTY_SELECT;
        break;
    case 2:
        state = HIGH_SCORES;
        break;
    case 3:
        exit(0);
    default:
        cout << "Nespravna volba!\n";
        cin.get();
        state = MENU;
    }
}

void displayDifficultyMenu(int& maxAttempts) {
    system("cls");
    cout << "========================================\n";
    cout << "        VYBERTE UROVEN OBTIAZNOSTI\n";
    cout << "========================================\n\n";
    cout << "1. LAHKA (7 pokusov)\n";
    cout << "2. STREDNA (5 pokusov)\n";
    cout << "3. TAZKA (3 pokusov)\n\n";
    cout << "Vyber uroven (1-3): ";

    int choice;
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        maxAttempts = 7;
        break;
    case 2:
        maxAttempts = 5;
        break;
    case 3:
        maxAttempts = 3;
        break;
    default:
        cout << "Nespravna volba! Vybera sa stredna uroven.\n";
        maxAttempts = 5;
        cin.get();
    }
}

void playGame(Player& player, const vector<string>& words, int maxAttempts) {
    system("cls");
    string wordToGuess = getRandomWord(words);
    player.reset();
    player.incorrectGuesses = 0;

    cout << "Zacina sa nova hra! Mas " << maxAttempts << " pokusov.\n\n";
    cin.get();

    while (player.incorrectGuesses < maxAttempts) {
        system("cls");
        cout << "========== HANGMAN HRA ==========\n\n";
        cout << drawHangman(player.incorrectGuesses, maxAttempts) << endl;
        cout << "Slovo: ";
        displayWord(wordToGuess, player.guessedChars);

        cout << "\nSpravne hadania: " << player.correctGuesses << " | Chybne hadania: " << player.incorrectGuesses << "/" << maxAttempts << endl;
        cout << "Skore: " << player.score << "\n\n";

        if (!player.guessedChars.empty()) {
            cout << "Hadane pismena: ";
            for (char letter : player.guessedChars) {
                cout << static_cast<char>(tolower(letter)) << " ";
            }
            cout << "\n\n";
        }

        cout << "Hadaj pismeno: ";
        char guess;
        cin >> guess;
        cin.ignore();

        guess = tolower(guess);

        
        if (player.hasGuessed(guess)) {
            cout << "Uz si toto pismeno hadal! Skus ine.\n";
            cin.get();
            continue;
        }

        player.guessedChars.push_back(guess);

        if (containsChar(wordToGuess, guess)) {
                cout << "Spravne! Pismeno '" << static_cast<char>(guess) << "' je v slove!\n";
            player.addCorrectGuess();
        }
        else {
            cout << "Zle! Pismeno '" << static_cast<char>(guess) << "' nie je v slove!\n";
            player.addIncorrectGuess();
        }

        cin.get();

        // Kontrola výhry
        bool foundAllLetters = true;
        for (char letter : wordToGuess) {
            if (find(player.guessedChars.begin(), player.guessedChars.end(), tolower(letter)) == player.guessedChars.end()) {
                foundAllLetters = false;
                break;
            }
        }

        if (foundAllLetters) {
            system("cls");
            cout << drawHangman(player.incorrectGuesses, maxAttempts) << endl;
            cout << "\n*** VYHRAL SI! ***\n";
            cout << "Slovo bolo: " << wordToGuess << endl;
            cout << "Konecne skore: " << player.score << " bodov\n";
            cout << "Spravne hadania: " << player.correctGuesses << endl;
            cin.get();
            return;
        }
    }

    // Prehra
    system("cls");
    cout << drawHangman(player.incorrectGuesses, maxAttempts) << endl;
    cout << "\n*** PREHRAL SI! ***\n";
    cout << "Slovo bolo: " << wordToGuess << endl;
    cout << "Konecne skore: " << player.score << " bodov\n";
    cin.get();
}

void displayHighScores() {
    system("cls");
    cout << "========================================\n";
    cout << "         NAJLEPSIE VYSLEDKY\n";
    cout << "========================================\n\n";

    ifstream file("scores.txt");
    if (!file.is_open()) {
        cout << "Zatial ziadne skore!\n";
    }
    else {
        string line;
        int rank = 1;
        while (getline(file, line) && rank <= 10) {
            cout << rank << ". " << line << endl;
            rank++;
        }
        file.close();
    }

    cout << "\nStlac ENTER na vrat sa do menu...\n";
    cin.get();
}

void saveScore(const Player& player) {
    map<string, int> scores;  

    
    ifstream inFile("scores.txt");
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            size_t dashPos = line.find(" - ");
            if (dashPos != string::npos) {
                string name = line.substr(0, dashPos);
                size_t bodovPos = line.find(" bodov");
                if (bodovPos != string::npos) {
                    string scoreStr = line.substr(dashPos + 3, bodovPos - dashPos - 3);
                    int score = stoi(scoreStr);
                    
                    if (scores.find(name) == scores.end() || score > scores[name]) {
                        scores[name] = score;
                    }
                }
            }
        }
        inFile.close();
    }

    
    if (scores.find(player.name) == scores.end() || player.score > scores[player.name]) {
        scores[player.name] = player.score;
    }

    
    vector<pair<string, int>> sortedScores(scores.begin(), scores.end());

    
    sort(sortedScores.begin(), sortedScores.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
        });

    
    ofstream outFile("scores.txt");
    for (const auto& entry : sortedScores) {
        outFile << entry.first << " - " << entry.second << " bodov\n";
    }
    outFile.close();
}

int main() {
    srand(time(0));
    vector<string> words;
    string word;

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::filesystem::path exeDir = std::filesystem::path(exePath).remove_filename();
    std::filesystem::path dataFile = exeDir / "slova.txt";

    ifstream file(dataFile.string());
    if (!file.is_open()) {
        cout << "CHYBA: Nepodarilo sa otvoriť subor 'slova.txt'! (" << dataFile.string() << ")\n";
        return 1;
    }

    while (file >> word) {
        words.push_back(word);
    }
    file.close();

    if (words.empty()) {
        cout << "CHYBA: Subor 'slova.txt' je prazdny!\n";
        return 1;
    }

    GameState state = MENU;
    Player player;
    int maxAttempts = 7;

    while (true) {
        switch (state) {
        case MENU:
            displayMenu(state);
            break;
        case DIFFICULTY_SELECT:
            cout << "Zadaj svoje meno: ";
            getline(cin, player.name);
            displayDifficultyMenu(maxAttempts);
            playGame(player, words, maxAttempts);
            saveScore(player);
            state = MENU;
            break;
        case HIGH_SCORES:
            displayHighScores();
            state = MENU;
            break;
        default:
            state = MENU;
        }
    }

    return 0;
}