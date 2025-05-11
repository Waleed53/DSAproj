#include <iostream>
#include <unordered_set>
#include <string>
#include <cstring>
#include <map>
using namespace std;

class TrieNode {
public:
    TrieNode* children[26];
    bool isEndOfWord;

    TrieNode() {
        isEndOfWord = false;
        for (int i = 0; i < 26; i++) {
            children[i] = nullptr;
        }
    }
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    void insert(const string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            if (!isalpha(ch)) continue;
            int index = tolower(ch) - 'a';
            if (!node->children[index])
                node->children[index] = new TrieNode();
            node = node->children[index];
        }
        node->isEndOfWord = true;
    }

    bool search(const string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            if (!isalpha(ch)) continue;
            int index = tolower(ch) - 'a';
            if (!node->children[index])
                return false;
            node = node->children[index];
        }
        return node->isEndOfWord;
    }
};

const int d = 256;
const int q = 101; 

class SpamDetector {
private:
    Trie spamTrie;
    unordered_set<string> blacklistedSenders;
    map<string, bool> learningMemory;
    string spamWords[5] = {"win", "lottery", "free", "click", "urgent"}; 

    bool rabinKarpSearch(const string& text, const string& pattern) {
        int m = pattern.size();
        int n = text.size();
        if (m > n) return false;

        int i, j;
        int p = 0; 
        int t = 0; 
        int h = 1;

        for (i = 0; i < m - 1; i++) {
            h = (h * d) % q;
        }

        for (i = 0; i < m; i++) {
            p = (d * p + pattern[i]) % q;
            t = (d * t + text[i]) % q;
        }

        for (i = 0; i <= n - m; i++) {
            if (p == t) {
                for (j = 0; j < m; j++) {
                    if (text[i + j] != pattern[j]) break;
                }
                if (j == m) return true; 
            }

            if (i < n - m) {
                t = (d * (t - text[i] * h) + text[i + m]) % q;
                if (t < 0) t = (t + q);
            }
        }

        return false; 
    }

public:
    SpamDetector() {
        for (int i = 0; i < 5; i++) {
            spamTrie.insert(spamWords[i]);
        }

        blacklistedSenders.insert("spam@example.com");
        blacklistedSenders.insert("fraud@scam.com");
    }

    bool isSpamKeywordPresent(const string& message) {
        string word = "";
        for (size_t i = 0; i <= message.length(); ++i) {
            if (i < message.length() && isalpha(message[i])) {
                word += tolower(message[i]);
            } else {
                if (!word.empty() && spamTrie.search(word))
                    return true;
                word = "";
            }
        }

        for (int i = 0; i < 5; i++) {
            if (rabinKarpSearch(message, spamWords[i]))
                return true; 
        }

        return false;
    }

    string checkMessage(const string& sender, const string& message) {
        string key = sender + message;
        if (learningMemory.find(key) != learningMemory.end()) {
            return learningMemory[key] ? "Spam" : "Not Spam";
        }

        if (blacklistedSenders.find(sender) != blacklistedSenders.end()) {
            cout << "[Auto-Detected] Blacklisted sender detected." << endl;
            return "Spam";
        }

        if (isSpamKeywordPresent(message)) {
            cout << "[Auto-Detected] Suspicious keywords found in message." << endl;
            return "Spam";
        }

        return "Not Spam";
    }

    void learnFromUser(const string& sender, const string& message) {
        string key = sender + message;
        string userResponse;

        cout << "Do you consider this spam? (yes/no): ";
        getline(cin, userResponse);

        if (userResponse == "yes") {
            learningMemory[key] = true;
            cout << "Marked as spam and remembered.\n";
        } else {
            learningMemory[key] = false;
            cout << "Marked as not spam and remembered.\n";
        }
    }
};

int main() {
    SpamDetector detector;
    string sender, message, line;

    while (true) {
        cout << "\nEnter sender email (or type 'exit' to quit): ";
        getline(cin, sender);
        if (sender == "exit") break;

        cout << "Enter full email message (type 'END' on a new line to finish):\n";
        message = "";
        while (getline(cin, line)) {
            if (line == "END") break;
            message += line + "\n";
        }

        string result = detector.checkMessage(sender, message);
        cout << "Result: " << result << endl;

        detector.learnFromUser(sender, message);
    }

    return 0;
}

