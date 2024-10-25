/*
 * Ultimate Word Search Generator
 * Copyright (C) 2024  Alexandra Dogwood
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact: hello@adogwood.com
 */


#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>

// Log levels for controlling log output in production
enum class LogLevel { DEBUG, INFO, WARN, ERROR };
LogLevel currentLogLevel = LogLevel::INFO; // Set this to change log verbosity

// Logging function that respects the log level
void log(LogLevel level, const std::string& message) {
    if (level >= currentLogLevel) {
        std::string prefix;
        switch (level) {
            case LogLevel::DEBUG: prefix = "[DEBUG] "; break;
            case LogLevel::INFO:  prefix = "[INFO] "; break;
            case LogLevel::WARN:  prefix = "[WARN] "; break;
            case LogLevel::ERROR: prefix = "[ERROR] "; break;
        }
        std::cout << prefix << message << std::endl;
    }
}

class WordSearch {
public:
    WordSearch(int rows, int cols, const std::vector<std::string>& words, const std::vector<char>& letters, const std::unordered_set<std::string>& bannedWords)
        : rows(rows), cols(cols), words(words), letters(letters), bannedWords(bannedWords), rng(std::random_device{}()) {
        grid.resize(rows, std::vector<char>(cols, ' ')); // Initialize the grid with empty spaces
    }

    // Generate the word search puzzle
    void generate() {
        log(LogLevel::DEBUG, "Shuffling words...");
        std::shuffle(words.begin(), words.end(), rng); // Shuffle words for random placement
        for (const auto& word : words) {
            if (bannedWords.find(word) == bannedWords.end()) {
                log(LogLevel::DEBUG, "Placing word: " + word);
                placeWord(word); // Place each word in the grid
            } else {
                log(LogLevel::DEBUG, "Skipping banned word: " + word);
            }
        }

        // Fill remaining empty cells with random letters
        fillGrid();
    }

    // Print the grid to the specified output stream
    void printGrid(std::ostream& out) const {
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                out << cell; // Print each cell in the row
            }
            out << "\n"; // New line after each row
        }
    }

private:
    int rows, cols;
    std::vector<std::string> words; // Words to place in the grid
    std::vector<char> letters; // Letters for filling empty spaces
    std::unordered_set<std::string> bannedWords; // Banned words that cannot appear in the grid
    std::vector<std::vector<char>> grid; // 2D grid for the puzzle
    std::mt19937 rng; // Random number generator

    // Check if a word can be placed in the specified direction
    bool canPlaceWord(const std::string& word, int row, int col, int dr, int dc) const {
        int wordLength = word.length();
        // Check bounds
        if (row + dr * (wordLength - 1) < 0 || row + dr * (wordLength - 1) >= rows ||
            col + dc * (wordLength - 1) < 0 || col + dc * (wordLength - 1) >= cols) {
            return false;
        }

        // Check if the word fits in the grid
        for (int i = 0; i < wordLength; ++i) {
            int newRow = row + dr * i;
            int newCol = col + dc * i;

            if (grid[newRow][newCol] != ' ' && grid[newRow][newCol] != word[i]) {
                return false; // Collision with existing letters
            }
        }

        return true;
    }

    // Place a word in the grid
    void placeWord(const std::string& word) {
        static const std::vector<std::pair<int, int>> directions = { {0, 1}, {1, 0}, {1, 1}, {0, -1}, {-1, 0}, {-1, -1} };

        std::uniform_int_distribution<int> rowDist(0, rows - 1);
        std::uniform_int_distribution<int> colDist(0, cols - 1);
        std::uniform_int_distribution<int> dirDist(0, directions.size() - 1);
        const int maxAttempts = 100; // Max attempts to place a word

        for (int attempts = 0; attempts < maxAttempts; ++attempts) {
            int directionIndex = dirDist(rng);
            int dr = directions[directionIndex].first;
            int dc = directions[directionIndex].second;
            int row = rowDist(rng);
            int col = colDist(rng);

            if (canPlaceWord(word, row, col, dr, dc)) {
                for (int i = 0; i < word.length(); ++i) {
                    int newRow = row + dr * i;
                    int newCol = col + dc * i;
                    grid[newRow][newCol] = word[i]; // Place the word in the grid
                }
                return; // Word placed successfully
            }
        }

        log(LogLevel::WARN, "Failed to place word: " + word + " after " + std::to_string(maxAttempts) + " attempts.");
    }

    // Fill empty spaces in the grid with random letters
    void fillGrid() {
        log(LogLevel::DEBUG, "Filling the grid...");
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (grid[r][c] == ' ') {
                    char randomLetter;
                    bool validLetter = false;

                    // Keep generating random letters until a valid one is found
                    while (!validLetter) {
                        randomLetter = getRandomLetter();
                        grid[r][c] = randomLetter; // Place random letter

                        if (!containsBannedWords()) {
                            validLetter = true; // Accept the letter if no banned words are formed
                        } else {
                            grid[r][c] = ' '; // Reset if a banned word is formed
                        }
                    }
                }
            }
        }
    }

    // Get a random letter from the letters vector
    char getRandomLetter() {
        std::uniform_int_distribution<int> letterDist(0, letters.size() - 1);
        return letters[letterDist(rng)];
    }

    // Check if any banned words are formed in the grid
    bool containsBannedWords() const {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (checkForBannedWordAround(r, c)) {
                    return true; // Found a banned word
                }
            }
        }
        return false;
    }

    // Check for banned words around the given position
    bool checkForBannedWordAround(int r, int c) const {
        for (const auto& bannedWord : bannedWords) {
            if (checkForBannedWord(bannedWord, r, c)) {
                return true; // Found a banned word
            }
        }
        return false;
    }

    // Check if the given banned word can be formed starting at the given position
    bool checkForBannedWord(const std::string& word, int r, int c) const {
        static const std::vector<std::pair<int, int>> directions = {
            {0, 1}, {1, 0}, {1, 1}, {0, -1}, {-1, 0}, {-1, -1}, {1, -1}, {-1, 1}
        };

        for (const auto& dir : directions) {
            if (canFormWord(word, r, c, dir.first, dir.second)) {
                return true; // Found a match
            }
        }

        return false; // No match found
    }

    // Check if a word can be formed from the starting position in the given direction
    bool canFormWord(const std::string& word, int row, int col, int dr, int dc) const {
        int wordLength = word.length();

        if (row + dr * (wordLength - 1) < 0 || row + dr * (wordLength - 1) >= rows ||
            col + dc * (wordLength - 1) < 0 || col + dc * (wordLength - 1) >= cols) {
            return false; // Out of bounds
        }

        // Check if the word matches the characters in the grid
        for (int i = 0; i < wordLength; ++i) {
            int newRow = row + dr * i;
            int newCol = col + dc * i;

            if (grid[newRow][newCol] != word[i]) {
                return false; // Mismatch found
            }
        }

        return true; // Word matches
    }
};

// Generate a single puzzle and save it to the output file
void generatePuzzle(int puzzleNumber, const std::vector<std::string>& words, const std::vector<char>& letters,
                    const std::unordered_set<std::string>& bannedWords, int rows, int cols, const std::string& outputFile) {
    log(LogLevel::INFO, "Generating puzzle " + std::to_string(puzzleNumber + 1) + "...");
    WordSearch ws(rows, cols, words, letters, bannedWords);
    ws.generate();

    // Save the generated grid to the output file
    std::ofstream file(outputFile, std::ios::app);
    if (file.is_open()) {
        file << "Puzzle " << puzzleNumber + 1 << ":\n";
        ws.printGrid(file); // Print the grid to the file
        file << "\n";
    } else {
        log(LogLevel::ERROR, "Error opening output file.");
    }
}

// Generate multiple puzzles in parallel
void generatePuzzles(int numPuzzles, const std::vector<std::string>& words, const std::vector<char>& letters,
                     const std::unordered_set<std::string>& bannedWords, int rows, int cols, const std::string& outputFile) {
    // Clear the output file
    std::ofstream clearFile(outputFile);
    clearFile.close();

    std::vector<std::future<void>> futures;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numPuzzles; ++i) {
        // Launch puzzle generation in a separate thread
        futures.emplace_back(std::async(std::launch::async, generatePuzzle, i, std::cref(words), std::cref(letters), std::cref(bannedWords), rows, cols, outputFile));
    }

    for (auto& future : futures) {
        future.get(); // Wait for all puzzles to finish
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalElapsed = endTime - startTime;
    log(LogLevel::INFO, "All puzzles generated in " + std::to_string(totalElapsed.count()) + " seconds.");
}

// Main function to get user input and initiate puzzle generation
int main() {

    std::cout << "Ultimate Word Search Generator  Copyright (C) 2024  Alexandra Dogwood" << std::endl;
    std::cout << "This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'." << std::endl;
    std::cout << "This is free software, and you are welcome to redistribute it under certain conditions; type 'show c' for details." << std::endl;


    int rows, cols, numPuzzles;
    std::string outputFile;
    std::vector<std::string> words;
    std::vector<char> letters;
    std::unordered_set<std::string> bannedWords;

    std::cout << "Enter number of rows (e.g., 30): ";
    std::cin >> rows;
    std::cout << "Enter number of columns (e.g., 25): ";
    std::cin >> cols;

    std::cout << "Enter letters (e.g., A B C D): ";
    std::string letterInput;
    std::cin.ignore(); // Ignore remaining newline
    std::getline(std::cin, letterInput);
    std::istringstream letterStream(letterInput);
    char letter;
    while (letterStream >> letter) {
        letters.push_back(letter);
    }

    if (letters.empty()) {
        std::cerr << "Error: No letters provided. Exiting.\n";
        return 1; // Exit if no letters are provided
    }

    std::cout << "Enter words (type 'done' when finished): ";
    std::string word;
    while (std::cin >> word && word != "done") {
        words.push_back(word);
    }

    std::cout << "Enter banned words (type 'done' when finished): ";
    while (std::cin >> word && word != "done") {
        bannedWords.insert(word);
    }

    std::cout << "Enter number of puzzles to generate: ";
    std::cin >> numPuzzles;

    std::cout << "Enter output file name: ";
    std::cin >> outputFile;

    generatePuzzles(numPuzzles, words, letters, bannedWords, rows, cols, outputFile);

    return 0;
}
