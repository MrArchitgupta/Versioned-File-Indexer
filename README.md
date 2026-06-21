# Memory Efficient Versioned File Indexer

## Overview

This program builds a word-level index for large text files while keeping memory usage low. Instead of loading the entire file into memory, the file is read in small chunks using a fixed-size buffer. Words are extracted from these chunks and their frequencies are stored in an index.

Each input file is treated as a separate **version**, which allows the program to compare different files or run queries on them.

The program supports three types of queries:
- Word frequency query
- Difference query between two versions
- Top-K most frequent words

## Design

The program is written using an object-oriented design where each class has a clear responsibility.

**file_reader**  
Handles reading the input file using a fixed-size buffer (between 256 KB and 1024 KB). It reads the file chunk by chunk.

**Tokenizer**  
Extracts words from the buffer. Words are considered sequences of alphanumeric characters. All characters are converted to lowercase so that word matching is case-insensitive.

**freq_map (Template Class)**  
Stores word frequencies using an `unordered_map`. It allows incrementing word counts and retrieving frequencies.

**VersionedIndexer**  
Maintains the frequency index for each version of the file. It is responsible for building the index and providing data for queries.

**Query Classes**  
An abstract base class `Query` is used with three derived classes:
- `Wordquery`
- `Diffquery`
- `Topkquery`

This allows different queries to be executed using runtime polymorphism.

## File Processing

The file is processed incrementally using the buffer:

1. A chunk of the file is read into the buffer.
2. The tokenizer scans the chunk and extracts words.
3. Word frequencies are updated in the index.
4. The process repeats until the whole file has been processed.

Since only a fixed-size buffer is used, memory usage does not depend on the size of the file.

## Compilation

Compile the program using:

g++ -O2 230186_archit.cpp -o analyzer

## Usage

### Word Query

./analyzer --file test_logs.txt --version v1 --buffer 256 --query word --word error

### Top-K Query

./analyzer --file test_logs.txt --version v1 --buffer 256 --query top --top 10

### Difference Query

./analyzer --file1 test_logs.txt --version1 v1 --file2 verbose_logs.txt --version2 v2 --buffer 256 --query diff --word request

Note: you can change the buffer size and the word. In above commands I used 256.

## Features
- Processes files using a fixed-size buffer (256–1024 KB)
- Case-insensitive word indexing
- Supports multiple file versions
- Efficient Top-K query using a priority queue
- Demonstrates C++ templates, inheritance, polymorphism, and exception handling

## Assumptions

- Words consist of alphanumeric characters.
- Word matching is case-insensitive.
- Memory usage depends only on the number of unique words, not the size of the file.

## Conclusion

This program shows how large files can be processed efficiently without loading them fully into memory. By reading the file in chunks and using a modular design, the system builds a word index that is both memory-efficient and easy to extend.
