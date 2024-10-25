```markdown
# Ultimate Word Search Generator

Welcome to the **Ultimate Word Search Generator**! This program was developed to create challenging word search puzzles, specifically for the book *Find the Cl!t: It's not that hard (right?)*, which will be released on **10/31/2024**. The unique aspect of this book is that all puzzles are generated using only **four letters** while avoiding the spelling of a specific word.

## Features

- Generates customizable word search puzzles.
- Supports dynamic grid sizes (rows and columns).
- Allows for an empty word list, adapting to various user needs.
- Can filter out banned words to prevent specific unwanted terms in the puzzles.
- Concurrent puzzle generation for efficiency using multi-threading.

## Getting Started

### Prerequisites

- A C++ compiler that supports C++11 or later.
- Standard libraries for I/O operations.

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/adogwood/Ultimate-WordSearch-Generator.git
   ```

2. Navigate to the directory:
   ```bash
   cd Ultimate-WordSearch-Generator
   ```

3. Compile the program:
   ```bash
   g++ ultimateWordSearchGenerator.cpp -o wordsearch
   ```

### Usage

Run the program:
```bash
./wordsearch
```

Follow the prompts to enter:

- Number of rows and columns for the grid.
- A list of letters (separated by spaces).
- Words to include in the puzzles (type 'done' when finished).
- Banned words (type 'done' when finished).
- The number of puzzles to generate.
- The output file name to save the puzzles.

### Example Input

```
Enter number of rows (e.g., 30): 30
Enter number of columns (e.g., 25): 25
Enter letters (e.g., A B C D): A B C D
Enter words (type 'done' when finished): example
done
Enter banned words (type 'done' when finished): forbidden
done
Enter number of puzzles to generate: 5
Enter output file name: puzzles.txt
```

## License

This project is licensed under the **GNU General Public License (GPL) v3.0**. You can view the full license text in the [LICENSE](LICENSE) file.

## License Notices

Each source file in this repository includes a copyright notice indicating that it is licensed under the GNU General Public License (GPL) v3.0. This includes a statement of warranty disclaimer and the terms under which the software can be redistributed and modified.

If you have questions regarding the license or the program itself, please contact me at hello@adogwood.com


## Acknowledgments

- Special thanks to the open-source community for their contributions and support.
- This project was inspired by the challenges of creating engaging and entertaining word search puzzles.

## Contributing

Contributions are welcome! If you have suggestions for improvements or want to report issues, please open an issue or submit a pull request.



