# Memory-Efficient Versioned File Indexer

## Overview
[cite_start]This program implements a memory-efficient versioned file indexer that processes large text files incrementally using a fixed-size buffer[cite: 7, 17]. [cite_start]It builds a case-insensitive, word-level frequency index without ever loading the entire file into memory[cite: 18, 20, 23, 25]. [cite_start]The system supports maintaining multiple file versions simultaneously and executing various analytical queries on them[cite: 37, 38, 40].

## System Architecture
[cite_start]The solution is built using strong object-oriented design and consists of the following core classes[cite: 59, 62]:
* [cite_start]**`BufferedFileReader`**: Handles reading the file incrementally in chunks specified by a strict buffer size (between 256 KB and 1024 KB)[cite: 54, 55, 63].
* [cite_start]**`Tokenizer`**: Parses the raw character buffer into valid alphanumeric words, correctly handling tokens that span across buffer boundaries[cite: 19, 57, 64].
* [cite_start]**`VersionedIndexer`**: Maps user-defined version names to their respective word frequency maps[cite: 36, 37, 65].
* [cite_start]**`Query` Hierarchy**: An abstract base class with derived classes (`WordQuery`, `DiffQuery`, `TopKQuery`) to process specific user requests[cite: 66, 68].

## C++ Requirements Met
* [cite_start]**Templates**: Utilizes a custom `freq_map<k, v>` template class for memory-efficient hash map encapsulation[cite: 72].
* [cite_start]**Inheritance & Polymorphism**: Uses dynamic dispatch via the `Query` interface and its overridden `execute()` methods[cite: 68, 69].
* [cite_start]**Function Overloading**: Implements overloaded versions of `buildSingleVersion()` to gracefully handle single-file and two-file indexing requirements[cite: 70].
* [cite_start]**Exception Handling**: Uses `try/catch/throw` blocks to handle runtime errors, such as invalid buffer sizes or unreadable file paths[cite: 71].

## Compilation
Use a standard C++ compiler (like `g++`) to compile the source code. [cite_start]Note that per the assignment instructions, the C++ source file is named with a `.c` extension[cite: 104, 105].

```bash
g++ -O3 -std=c++14 -o analyzer rollnumber_firstname.c

Usage Examples
The program is executed via the command line. Below are examples for the three supported query types:
+1

1. Word Count Query (Single File) Returns the frequency of a specific word in a version.
+2

Bash
./analyzer --file dataset_v1.txt --version v1 \
--buffer 512 --query word --word error
2. Top-K Query (Single File) Displays the top 'K' most frequent words in a version, sorted in descending order.
+2

Bash
./analyzer --file dataset_v1.txt --version v1 \
--buffer 512 --query top --top 10
3. Difference Query (Two Files) Computes the frequency difference of a word between two distinct versions.
+2

Bash
./analyzer --file1 dataset_v1.txt --version1 v1 \
--file2 dataset_v2.txt --version2 v2 \
--buffer 512 --query diff --word error

***

**One quick formatting note for your final ZIP file:**
The assignment document asks for `rollnumber firstname.md` (with a space instead of an underscore) in one bullet point [cite: 106], but standardizes `rollnumber_firstname` everywhere else[cite: 102, 104, 107, 108]. It is highly likely the space is a typo in the document. I recommend naming it `rollnumber_firstname.md` to match the rest of your files, but you might want to double-check with your professor or TA just to be 100% safe.

Would you like some help structuring the contents of the `rollnumber_firstname.pdf` assignment report next?
