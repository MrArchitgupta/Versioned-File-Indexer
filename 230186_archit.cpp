#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <queue>

using namespace std;

// i am using these two function and not the standard one becuase they are faster
bool my_isalnum(char c) {
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')){
        return true;
    }
    else return false;
}

char my_tolower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 32; 
    }
    return c;
}


template<typename k, typename v> class freq_map {
private:
    unordered_map<k, v> data;
public:
    void increment(const k& key) {
        data[key]++;
    }

    v get(const k& key) const {
        auto it = data.find(key);
        if (it != data.end())
            return it->second;
        return 0;
    }

    const unordered_map<k, v>& getData() const {
        return data;
    }
};


class file_reader {
private:
    ifstream file;
    size_t b_size;
    vector<char> buffer;

public:
    file_reader(const string& path, size_t kb) {
        if (kb < 256 || kb > 1024) {
            throw invalid_argument("Buffer size must be between 256 and 1024 KB");
        }

        b_size = kb * 1024; // converting to bytes becuase 1kb=1024 byte
        buffer.resize(b_size);

        file.open(path, ios::in | ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Failed to open file: " + path);
        }    
    }

    size_t read_chunk(const char*& out_buffer) {
        if (!file.good()) return 0;

        file.read(buffer.data(), b_size);
        size_t bytes_read = file.gcount();
        
        out_buffer = buffer.data();
        return bytes_read;
    }

    bool eof() const {
        return file.eof();
    }
};


class Tokenizer {
private:
    string curr;

public:
    Tokenizer() {
        curr.reserve(256);
    }

    template<typename Func>
    void tokenize(const char* chunk, size_t length, Func processword) {
        for (size_t i = 0; i < length; i++) {
            char c = chunk[i];

            if (my_isalnum(c)) {
                curr.push_back(my_tolower(c));
            } 
            else {
                if (!curr.empty()) {
                    processword(curr);
                    curr.clear();
                }
            }
        }
    }

    template<typename Func>
    void flush(Func processword) {
        if (!curr.empty()) {
            processword(curr);
            curr.clear();
        }
    }
};


class VersionedIndexer {
private:
    unordered_map<string, freq_map<string, size_t>> versions;

public:
    void buildIndex(const string& version_name, const string& filepath, size_t buffer) {

        file_reader reader(filepath, buffer);
        Tokenizer tokenizer;

        freq_map<string, size_t> freq; 

        const char* data_chunk = nullptr;
        size_t bytes_read = 0;

        while ((bytes_read = reader.read_chunk(data_chunk)) > 0) {
            tokenizer.tokenize(data_chunk, bytes_read, [&](const string& word){
                freq.increment(word);
            });
        }

        tokenizer.flush([&](const string& word){
            freq.increment(word);
        });

        versions[version_name] = move(freq);
    }
      
    size_t get_cnt(const string& version, const string& word) const {
        auto it = versions.find(version);
        if (it == versions.end())
            throw runtime_error("Version not found");

        return it->second.get(word);
    }

    const freq_map<string, size_t>& getVersion(const string& version) const {
        auto it = versions.find(version);
        if (it == versions.end()){
            throw runtime_error("Version not found");
        }
        return it->second;
    }
};


class Query {
public:
// The "= 0" makes this a pure virtual function making the class abstract
    virtual void execute(VersionedIndexer& indexer) = 0;
    virtual ~Query() = default;
};

// now i will define 3 derived classes for 3 different types of query
class Wordquery : public Query {
private:
    string version;
    string word;

public:
    Wordquery(const string& v, const string& w) : version(v), word(w) {}

    void execute(VersionedIndexer& indexer) override {
        size_t count = indexer.get_cnt(version, word);
        cout << "Version: " << version << "\n";
        cout << "Word: " << word << "\n";
        cout << "Frequency: " << count << "\n";
    }
};


class Diffquery : public Query {
private:
    string ver1;
    string ver2;
    string word;

public:
    Diffquery(const string& v1, const string& v2, const string& w)
        : ver1(v1), ver2(v2), word(w) {}

    void execute(VersionedIndexer& indexer) override {
        size_t c1 = indexer.get_cnt(ver1, word);
        size_t c2 = indexer.get_cnt(ver2, word);
        
        cout << "Version1: " << ver1 << "\n";
        cout << "Version2: " << ver2 << "\n";
        cout<< "Word: " << word << "\n"; // it is the not the original word it is converted into lower case
        cout<< "Difference (v2 - v1): " << static_cast<long long>(c2) - static_cast<long long>(c1) << "\n";
    } 
};


class Topkquery : public Query {
private:
    string ver;
    size_t k;

public:
    Topkquery(const string& v, size_t top) : ver(v), k(top) {}

    void execute(VersionedIndexer& indexer) override {
        auto& data = indexer.getVersion(ver).getData();

        auto cmp = [](const pair<string, size_t>& a, const pair<string, size_t>& b){
            return a.second > b.second;
        };
        // i am using priority queue instead of sorting whole frequency table because generally k is small
        // so time complexity would be O(klogk) instead of O(nlogn)

        priority_queue<pair<string, size_t>, vector<pair<string, size_t>>, decltype(cmp) > pq(cmp);

        for(const auto& entry : data){
            if(pq.size() < k) {
                pq.push(entry);
            } else if(entry.second > pq.top().second) {
                pq.pop();
                pq.push(entry);
            }
        }

        vector<pair<string, size_t>> res;
        res.reserve(pq.size());
        while(!pq.empty()){
            res.push_back(pq.top());
            pq.pop();
        }

        reverse(res.begin(), res.end());
        cout << "Version: " << ver << "\n"<< "Top " << k << " words:\n";

        for(auto &p : res) {
            cout << p.first << " -> " << p.second << "\n";
        }

    }
};

// it demonstrate the function overloading --- the name is name but numbe rof argument is different
void build_single_version(VersionedIndexer& indexer,
                        const string& file,
                        const string& version,
                        size_t buffer) {
    indexer.buildIndex(version, file, buffer);
}

void build_single_version(VersionedIndexer& indexer,
                        const string& file1,
                        const string& version1,
                        const string& file2,
                        const string& version2,
                        size_t buffer) {
    indexer.buildIndex(version1, file1, buffer);
    indexer.buildIndex(version2, file2, buffer);
}


int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false); // using for fast input output 
    cin.tie(nullptr);

    try {
        string file, file1, file2;
        string version, version1, version2;
        string qtype, word;
        size_t bufferkb = 0;
        size_t topK = 0;

        for (int i = 1; i < argc; i++) {
            string arg = argv[i];

            if (arg == "--file") file = argv[++i];
            else if (arg == "--file1") file1 = argv[++i];
            else if (arg == "--file2") file2 = argv[++i];
            else if (arg == "--version") version = argv[++i];
            else if (arg == "--version1") version1 = argv[++i];
            else if (arg == "--version2") version2 = argv[++i];
            else if (arg == "--buffer") bufferkb = stoul(argv[++i]);
            else if (arg == "--query") qtype = argv[++i];
            else if (arg == "--word") word = argv[++i];
            else if (arg == "--top") topK = stoul(argv[++i]);
        }

       
        for (char& c : word) {
            c = my_tolower(c);
        }

        auto start = chrono::high_resolution_clock::now();
        VersionedIndexer indexer;
        unique_ptr<Query> query;

        if (qtype == "word") {
            build_single_version(indexer, file, version, bufferkb);
            query = make_unique<Wordquery>(version, word);
        }
        else if (qtype == "diff") {
            build_single_version(indexer, file1, version1, file2, version2, bufferkb);
            query = make_unique<Diffquery>(version1, version2, word);
        }
        else if (qtype == "top") {
            build_single_version(indexer, file, version, bufferkb);
            query = make_unique<Topkquery>(version, topK);
        }
        else {
            throw invalid_argument("Invalid query type");
        }

        query->execute(indexer);

        auto end = chrono::high_resolution_clock::now();
        double time = chrono::duration<double>(end - start).count();

        cout<< "Buffer Size: " << bufferkb << " KB\n";
        cout<< "Execution Time: " << time << " seconds\n";
    }
    catch (exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}