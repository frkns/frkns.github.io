#include "bits/stdc++.h"
using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int randint(int l, int r) {
    return uniform_int_distribution(l, r)(rng);
}

double randdouble() {
    return uniform_real_distribution<double>(0, 1)(rng);
}

template <typename T> T choice(vector<T> vec) {
    return vec[randint(0, vec.size() - 1)];
}

vector<string> get_dict(const string& filename) {
    ifstream file(filename);
    vector<string> words;
    string line;
    while (getline(file, line))
        words.push_back(line);
    return words;
}

string randname(const vector<string>& dict) {

    static const array<int, 11> weights = {0, 5, 10, 15, 10, 5, 2, 1, 1};
    static discrete_distribution dist(weights.begin(), weights.end());

    int length = dist(rng);
    vector<string> words(length);

    for (int i = 0; i < length; i++) {
        string word = choice(dict);
        words[i] = word;
    }

    string sep = "";
    enum {
        CAMEL,
        KEHAB,
        PASCAL,
        SNAKE,
        SCREAMING,
        ENUM_LAST,
    };
    switch (randint(0, ENUM_LAST - 1)) {
        case CAMEL:
            for_each(words.begin() + 1, words.end(), [](string& word) {
                word[0] = toupper(word[0]);
            });
            break;
        case KEHAB:
            sep = "-";
            break;
        case PASCAL:
            for (string& word : words)
                word[0] = toupper(word[0]);
            break;
        case SNAKE:
            sep = "_";
            break;
        case SCREAMING:
            sep = "_";
            for (string& word : words)
                for (char& c : word)
                    c = toupper(c);
            break;
    }

    vector<string> extensions = {
        ".txt",   ".sh",     ".py",   ".a.out", ".out",        ".cc",  ".cpp",
        ".c",     ".h",      ".cxx",  ".html",  ".hs",         ".rs",  ".netrc",
        ".vimrc", ".config", ".json", ".pyx",   ".js",         ".jsx", ".ts",
        ".tsx",   ".md",     ".gzip", ".WOW",   ".hush-login", ".hpp", ".tmux.conf"};

    ostringstream oss;
    for (int i = 0; i < words.size(); i++)
        oss << words[i] << (i == words.size() - 1 ? "" : sep);

    if (randdouble() < 0.8)
        oss << choice(extensions);

    return oss.str();
}

const int NUM_FILES = 200;

int main() {
    vector<string> dict = get_dict("google-10k.txt");
    filesystem::create_directory("fun");
    for (int i = 0; i < NUM_FILES; i++) {
        cout << randname(dict) << '\n';
    }
}
