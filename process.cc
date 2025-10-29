#include "bits/stdc++.h"
using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int randint(int l, int r) {
    return uniform_int_distribution(l, r)(rng);
}

double randdouble() {
    return uniform_real_distribution<double>(0, 1)(rng);
}

template <typename T> T choice(const vector<T>& vec) {
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

    static const vector<int> weights = {0, 5, 10, 15, 10, 5, 2, 1, 1, 1};
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
    switch (randint(0, ENUM_LAST)) {
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

    vector<string> extensions = {".txt",   ".sh",         ".py",   ".a.out",    ".out",
                                 ".cc",    ".cpp",        ".c",    ".h",        ".cxx",
                                 ".html",  ".hs",         ".rs",   ".netrc",    ".msft",
                                 ".vimrc", ".config",     ".json", ".pyx",      ".js",
                                 ".jsx",   ".ts",         ".tsx",  ".md",       ".gzip",
                                 ".WOW",   ".hush-login", ".hpp",  ".tmux.conf"};

    ostringstream oss;
    for (int i = 0; i < words.size(); i++)
        oss << words[i] << (i == words.size() - 1 ? "" : sep);

    if (randdouble() < 0.8)
        oss << choice(extensions);

    return oss.str();
}

string shell_format(const string& to, double progress,
                    [[maybe_unused]] const string& desc = "") {
    ostringstream oss;
    oss << fixed << setprecision(1);
    oss << "echo 'Progress: " << progress * 100 << "%'; ";
    oss << "curl -L --progress-bar frkns.github.io/fun/" << to << " | sh";
    return oss.str();
}

int main() {
    const int NUM_FILES = 100;

    vector<string> dict = get_dict("google-10k.txt");

    vector<string> filenames(NUM_FILES);
    for (int i = 0; i < NUM_FILES; i++)
        filenames[i] = randname(dict);

    filesystem::create_directory("fun");

    ofstream entrypoint("fun/entrypoint");
    entrypoint << shell_format(filenames[0], 0);
    entrypoint.close();

    for (int i = 0; i < filenames.size() - 1; i++) {
        ofstream fout("fun/" + filenames[i]);
        fout << shell_format(filenames[i + 1], double(i + 1) / filenames.size());
        // ofstream dtor should be called, so no need to close file...
    }

    ofstream exitpoint("fun/" + filenames.back());
    exitpoint << "curl -s wttr.in | head -n 7 | tee weather && echo && echo 'Fetched "
                 "the weather and saved to `./weather` !!!'";
}
