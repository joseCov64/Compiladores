/**
 * ============================================================
 *  FSA – Finite State Automaton for Unsigned Numbers
 *  Based on: "Regular Expressions" slide 13 (Compilers 101)
 *
 *  Regular expression recognized:
 *    digit   -> [0-9]
 *    digits  -> digit+
 *    number  -> digits ( '.' digits )? ( 'E' [+-]? digits )?
 *
 *  Examples of valid tokens:
 *    42        3.14       1E10
 *    3.14E+2   0.5E-3     100E10
 *
 *  Transition diagram (states):
 *
 *    --> [0] --digit--> [1] <--digit-- [1]   (integer part)
 *        [1] --'.'--> [2] --digit--> [3] <--digit-- [3]   (decimal part)
 *        [1] --'E'--> [5]            (exponent, no decimal)
 *        [3] --'E'--> [5]            (exponent, with decimal)
 *        [5] --[+-]--> [6] --digit--> [7] <--digit-- [7]
 *        [5] --digit-> [7]
 *
 *    Accept states: 1, 3, 7
 *    State -1 = ERROR / FAIL
 * ============================================================
 */

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

// ─────────────────────────────────────────────
//  Class: State
//  Represents a single state in the FSA
// ─────────────────────────────────────────────
class State
{
public:
    int id;
    bool isAccepting;
    std::string description;

    State() : id(-1), isAccepting(false) {}
    State(int id, bool accepting, const std::string &desc)
        : id(id), isAccepting(accepting), description(desc) {}
};

// ─────────────────────────────────────────────
//  Class: FSA
//  Finite State Automaton for unsigned numbers
// ─────────────────────────────────────────────
class FSA
{
private:
    std::map<int, State> states;
    // transitions[currentState][inputClass] = nextState
    std::map<int, std::map<std::string, int>> transitions;
    int currentState;
    int startState;
    int errorState;

    // ── Classify a character into an input class ──────────────
    std::string classify(char c) const
    {
        if (c >= '0' && c <= '9')
            return "digit";
        if (c == '.')
            return "dot";
        if (c == 'E' || c == 'e')
            return "E";
        if (c == '+' || c == '-')
            return "sign";
        return "other";
    }

    // ── Add a transition ──────────────────────────────────────
    void addTransition(int from, const std::string &inputClass, int to)
    {
        transitions[from][inputClass] = to;
    }

    // ── Build the automaton (states + transition table) ───────
    void build()
    {
        // ----- States -----
        // State 0: start
        states[0] = State(0, false, "Start");
        // State 1: read one or more digits (integer part) — ACCEPT
        states[1] = State(1, true, "Integer digits");
        // State 2: read dot '.'
        states[2] = State(2, false, "Dot read");
        // State 3: read digits after dot — ACCEPT
        states[3] = State(3, true, "Decimal digits");
        // State 4: (unused – reserved for extensibility)
        // State 5: read 'E'
        states[5] = State(5, false, "Exponent E read");
        // State 6: read sign [+-] after E
        states[6] = State(6, false, "Exponent sign read");
        // State 7: read digits of exponent — ACCEPT
        states[7] = State(7, true, "Exponent digits");

        // ----- Transitions -----
        //  From state 0
        addTransition(0, "digit", 1); // 0 --digit--> 1

        //  From state 1  (integer part loop)
        addTransition(1, "digit", 1); // 1 --digit--> 1  (loop)
        addTransition(1, "dot", 2);   // 1 --'.'--> 2
        addTransition(1, "E", 5);     // 1 --'E'--> 5

        //  From state 2  (must read at least one decimal digit)
        addTransition(2, "digit", 3); // 2 --digit--> 3

        //  From state 3  (decimal loop)
        addTransition(3, "digit", 3); // 3 --digit--> 3  (loop)
        addTransition(3, "E", 5);     // 3 --'E'--> 5

        //  From state 5  (after 'E')
        addTransition(5, "sign", 6);  // 5 --[+-]--> 6
        addTransition(5, "digit", 7); // 5 --digit--> 7

        //  From state 6  (after optional sign)
        addTransition(6, "digit", 7); // 6 --digit--> 7

        //  From state 7  (exponent loop)
        addTransition(7, "digit", 7); // 7 --digit--> 7  (loop)
    }

public:
    FSA() : startState(0), errorState(-1)
    {
        build();
        reset();
    }

    // ── Reset to initial state ────────────────────────────────
    void reset()
    {
        currentState = startState;
    }

    // ── Feed one character to the automaton ───────────────────
    bool step(char c)
    {
        if (currentState == errorState)
            return false;

        std::string cls = classify(c);
        auto stateIt = transitions.find(currentState);
        if (stateIt != transitions.end())
        {
            auto transIt = stateIt->second.find(cls);
            if (transIt != stateIt->second.end())
            {
                currentState = transIt->second;
                return true;
            }
        }
        currentState = errorState;
        return false;
    }

    // ── Run the full input string ─────────────────────────────
    bool run(const std::string &input)
    {
        reset();
        for (char c : input)
        {
            if (!step(c))
                return false;
        }
        return isAccepting();
    }

    // ── Is the current state an accepting state? ──────────────
    bool isAccepting() const
    {
        if (currentState == errorState)
            return false;
        auto it = states.find(currentState);
        return it != states.end() && it->second.isAccepting;
    }

    int getCurrentState() const { return currentState; }

    // ── Print the full transition table ──────────────────────
    void printTransitionTable() const
    {
        std::vector<std::string> classes = {"digit", "dot", "E", "sign", "other"};
        std::cout << "\n=== Transition Table ===\n";
        std::cout << "State\t";
        for (auto &c : classes)
            std::cout << c << "\t";
        std::cout << "Accept?\n";
        std::cout << std::string(60, '-') << "\n";

        for (auto &[sid, state] : states)
        {
            std::cout << "  " << sid << "  \t";
            for (auto &cls : classes)
            {
                auto stateIt = transitions.find(sid);
                if (stateIt != transitions.end())
                {
                    auto transIt = stateIt->second.find(cls);
                    if (transIt != stateIt->second.end())
                    {
                        std::cout << transIt->second << "\t";
                    }
                    else
                    {
                        std::cout << "err\t";
                    }
                }
                else
                {
                    std::cout << "err\t";
                }
            }
            std::cout << (state.isAccepting ? "YES" : "no") << "\n";
        }
        std::cout << "\n";
    }

    // ── Validate a token and print result ────────────────────
    void validate(const std::string &token) const
    {
        FSA tmp = *this; // use a copy so we don't disturb this FSA
        bool valid = tmp.run(token);
        std::cout << "  \"" << token << "\""
                  << std::string(20 - token.size(), ' ')
                  << " -> " << (valid ? "VALID   (state " : "INVALID (state ")
                  << tmp.getCurrentState() << ")\n";
    }
};

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────
int main()
{
    FSA automaton;

    automaton.printTransitionTable();

    std::cout << "=== Validation Tests ===\n\n";

    std::cout << "[SHOULD ACCEPT]\n";
    automaton.validate("0");
    automaton.validate("42");
    automaton.validate("3.14");
    automaton.validate("100");
    automaton.validate("1E10");
    automaton.validate("3E+2");
    automaton.validate("3E-2");
    automaton.validate("3.14E10");
    automaton.validate("3.14E+10");
    automaton.validate("0.5E-3");

    std::cout << "\n[SHOULD REJECT]\n";
    automaton.validate("");          // empty
    automaton.validate(".");         // just a dot
    automaton.validate(".5");        // leading dot
    automaton.validate("3.");        // trailing dot (no decimal digits)
    automaton.validate("3E");        // E with no exponent digits
    automaton.validate("3E+");       // sign but no digits
    automaton.validate("3.1.4");     // double dot
    automaton.validate("12abc");     // letters in number
    automaton.validate("E10");       // starts with E
    automaton.validate("3.14E10.5"); // dot in exponent

    std::cout << "\n=== Interactive Mode ===\n";
    std::cout << "Enter a token to validate (Ctrl+D / Ctrl+Z to quit):\n";
    std::string input;
    while (std::cout << "> " && std::getline(std::cin, input))
    {
        automaton.validate(input);
    }

    return 0;
}