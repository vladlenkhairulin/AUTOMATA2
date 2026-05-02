#include <iostream>

#include "catch2/catch_amalgamated.hpp"

#include "../include/Regex.h"
#include "../include/RegexParser.h"
#include "../include/SyntaxTree.h"
#include "../include/Thompson.h"
#include "../include/DFAversion.h"
#include "../include/StateElimination.h"
#include "../include/Match.h"
#include "../include/NFA.h"
#include "../include/Tokens.h"

#include <string>
#include <vector>
#include <set>
#include <map>

static size_t finalsCount(const DFA& dfa) {
    size_t c = 0;
    for (auto* s : dfa.states) if (s->isFinal) c++;
    return c;
}


static bool match(const DFA& dfa, const std::string& word) {
    if (!dfa.start) return false;
    DFAState* cur = dfa.start;
    for (char c : word) {
        auto it = cur->transitions.find(c);
        if (it == cur->transitions.end()) return false;
        cur = it->second;
    }
    return cur->isFinal;
}

TEST_CASE("RegexParser: tokenize/parse/metacharacters") {
    SECTION("Simple concat and OR precedence") {
        RegexParser p;
        auto pf = p.parse("ab|c");
        REQUIRE(pf.size() == 5);
    }
    SECTION("metacharacters become symbols") {
        RegexParser p;
        auto tokens = p.tokenize("&|&+&?&.&(&)");
        REQUIRE(tokens.size() == 6);
        bool hasEscapedOr = false;
        bool hasEscapedPlus = false;
        bool hasEscapedQuestion = false;
        bool hasEscapedDot = false;
        bool hasEscapedLpar = false;
        bool hasEscapedRpar = false;

        for (const auto& token : tokens) {
            if (token.type == TokenType::SYMBOL) {
                if (token.value == "|") hasEscapedOr = true;
                if (token.value == "+") hasEscapedPlus = true;
                if (token.value == "?") hasEscapedQuestion = true;
                if (token.value == ".") hasEscapedDot = true;
                if (token.value == "(") hasEscapedLpar = true;
                if (token.value == ")") hasEscapedRpar = true;
            }
        }
        REQUIRE(hasEscapedOr);
        REQUIRE(hasEscapedPlus);
        REQUIRE(hasEscapedQuestion);
        REQUIRE(hasEscapedDot);
        REQUIRE(hasEscapedLpar);
        REQUIRE(hasEscapedRpar);

        for (const auto& token : tokens) {
            REQUIRE(token.type != TokenType::OR);
            REQUIRE(token.type != TokenType::PLUS);
            REQUIRE(token.type != TokenType::OPTION);
            REQUIRE(token.type != TokenType::DOT);
            REQUIRE(token.type != TokenType::LPAR);
            REQUIRE(token.type != TokenType::RPAR);
        }
    }
    SECTION("Repeat exact") {
        RegexParser p;
        auto pf = p.parse("x{5}");
        REQUIRE_FALSE(pf.empty());
        bool hasRepeat = false;
        for (const auto& token : pf) {
            if (token.type == TokenType::REPEAT && token.value == "5,5") {
                hasRepeat = true;
            }
        }
        REQUIRE(hasRepeat);
    }
    SECTION("Repeat range") {
        RegexParser p;
        auto pf = p.parse("digit{2,5}");
        REQUIRE_FALSE(pf.empty());

        bool hasRangeRepeat = false;
        for (const auto& token : pf) {
            if (token.type == TokenType::REPEAT && (token.value.find(',') != std::string::npos)) {
                hasRangeRepeat = true;
            }
        }
        REQUIRE(hasRangeRepeat);
    }
    SECTION("Repeat open upper") {
        RegexParser p;
        auto pf = p.parse("a{2,}");
        REQUIRE_FALSE(pf.empty());
    }
    SECTION("Repeat open lower") {
        RegexParser p;
        auto pf = p.parse("letter{,7}");
        REQUIRE_FALSE(pf.empty());
        bool hasOpenLower = false;
        for (const auto& token : pf) {
            if (token.type == TokenType::REPEAT && token.value == ",7") {
                hasOpenLower = true;
            }
        }
        REQUIRE(hasOpenLower);
    }
    SECTION("Invalid repeat throws") {
        RegexParser p;
        REQUIRE_THROWS(p.parse("a{2,3"));
        REQUIRE_THROWS(p.parse("a{}"));
    }
    SECTION("Named group") {
        RegexParser p;
        auto pf = p.parse("(<x>a)<x>");
        REQUIRE_FALSE(pf.empty());
        REQUIRE(pf.size() == 4);

        REQUIRE(pf[0].type == TokenType::SYMBOL);
        REQUIRE(pf[0].value == "a");
        REQUIRE(pf[1].type == TokenType::GRP);
        REQUIRE(pf[1].value == "x");
        REQUIRE(pf[2].type == TokenType::GRPREF);
        REQUIRE(pf[2].value == "x");
        REQUIRE(pf[3].type == TokenType::CONCAT);
    }
}

TEST_CASE("SyntaxTree: postfix tree building") {
    SECTION("a|bc") {
        RegexParser p;
        SyntaxTree st;
        auto root = st.build(p.parse("a|bc"));
        REQUIRE(root != nullptr);
    }
    SECTION("Unary operator") {
        RegexParser p;
        SyntaxTree st;
        auto root = st.build(p.parse("a+"));
        REQUIRE(root != nullptr);
    }
    SECTION("Invalid postfix returns nullptr") {
        SyntaxTree st;
        std::vector<Token> bad;
        bad.emplace_back(TokenType::OR);
        REQUIRE(st.build(bad) == nullptr);
    }
}

TEST_CASE("Thompson: build NFA for all operators") {
    Thompson th;
    SECTION("Symbol") {
        NFA nfa = th.build("a");
        REQUIRE(nfa.start != nullptr);
        REQUIRE(nfa.end != nullptr);
    }

    SECTION("Dot any symbol") {
        NFA nfa = th.build(".");
        REQUIRE(nfa.start != nullptr);
    }

    SECTION("OR") {
        NFA nfa = th.build("a|b");
        REQUIRE(nfa.start != nullptr);
    }

    SECTION("Concat") {
        NFA nfa = th.build("ab");
        REQUIRE(nfa.start != nullptr);
    }
    SECTION("Plus") {
        NFA nfa = th.build("a+");
        REQUIRE(nfa.start != nullptr);
    }
    SECTION("Optional") {
        NFA nfa = th.build("a?");
        REQUIRE(nfa.start != nullptr);
    }
    SECTION("Repeat cases") {
        REQUIRE_NOTHROW(th.build("a{3}"));
        REQUIRE_NOTHROW(th.build("a{2,4}"));
        REQUIRE_NOTHROW(th.build("a{2,}"));
        REQUIRE_NOTHROW(th.build("a{,3}"));
    }
    SECTION("Invalid repeat range throws") {
        REQUIRE_THROWS(th.build("a{5,2}"));
    }
    SECTION("Named capture group") {
        NFA nfa = th.build("(<g>ab)");
        REQUIRE(nfa.start != nullptr);
    }
}

TEST_CASE("DFAversion: convert/minimize/compile") {
    DFAversion dv;
    Thompson th;
    SECTION("Convert simple regex") {
        auto dfa = dv.convert(th.build("ab"));
        REQUIRE(dfa.start != nullptr);
        REQUIRE(dfa.states.size() >= 1);
    }
    SECTION("Minimize") {
        auto raw = dv.convert(th.build("a|a"));
        auto mini = dv.minimize(raw);
        REQUIRE(mini.states.size() <= raw.states.size());
    }
    SECTION("Compile path") {
        auto dfa = dv.compile("(a|b)+c");
        REQUIRE(dfa.start != nullptr);
        REQUIRE(dfa.states.size() >= 1);
    }
    SECTION("Minimize empty DFA safe") {
        DFA empty;
        auto m = dv.minimize(empty);
        REQUIRE(m.states.empty());
    }
    SECTION("Minimizing at work") {
        auto nfa = th.build("(ab|aс)+");
        auto raw = dv.convert(nfa);
        auto minimized = dv.minimize(raw);
        REQUIRE(raw.start != nullptr);
        REQUIRE(minimized.start != nullptr);
        REQUIRE(minimized.states.size() <= raw.states.size());
    }
}

TEST_CASE("DFAversion: complement language") {
    DFAversion dv;
    SECTION("Complement flips finals") {
        auto dfa = dv.compile("a");
        auto comp = dv.complement(dfa);
        REQUIRE(comp.start != nullptr);
        REQUIRE(comp.states.size() >= 1);
        size_t originals = finalsCount(dfa);
        size_t complement_states = comp.states.size();
        REQUIRE(originals > 0);
        REQUIRE(complement_states > originals);
    }
    SECTION("Complement empty") {
        DFA empty;
        auto c = dv.complement(empty);
        REQUIRE(c.states.empty());
    }
}

TEST_CASE("DFAversion: reverse language") {
    DFAversion dv;
    SECTION("Reverse ab ") {
        DFA dfa = dv.compile("ab");
        DFA rev = dv.reverse(dfa);
        REQUIRE(rev.start != nullptr);
        REQUIRE(rev.states.size() >= 1);
        StateElimination se;
        std::string restored = se.toRegex(rev);
        REQUIRE_FALSE(restored.empty());
        Regex re(restored);
        auto ok = re.findAll("xxbazz");
        REQUIRE(ok.size() == 1);
        REQUIRE(ok[0] == "ba");
        auto bad = re.findAll("xxabzz");
        REQUIRE(bad.empty());
    }

    SECTION("Reverse empty safe") {
        DFA empty;
        auto r = dv.reverse(empty);
        REQUIRE(r.states.empty());
    }
}

TEST_CASE("DFAversion: equivalence of languages") {
    DFAversion dv;
    SECTION("Equivalent regexes") {
        DFA dfa1 = dv.compile("a|b");
        DFA dfa2 = dv.compile("(a|b)");
        REQUIRE(dv.equivalence(dfa1, dfa2));
    }
    SECTION("Equivalent regexes2") {
        DFA dfa1 = dv.compile("(a|b)+");
        DFA dfa2 = dv.compile("(a|b)(a|b)?{0,}");
        REQUIRE(dv.equivalence(dfa1, dfa2));
    }
    SECTION("Non-equivalent regexes") {
        DFA dfa1 = dv.compile("ab+");
        DFA dfa2 = dv.compile("ab");
        REQUIRE_FALSE(dv.equivalence(dfa1, dfa2));
    }
    SECTION("Equivalence of empty languages") {
        DFA empty1{};
        DFA empty2 = dv.compile("");
        REQUIRE(dv.equivalence(empty1, empty2));
    }
}

TEST_CASE("Regex findAll") {
    SECTION("Simple string") {
        Regex re("ab");
        auto v = re.findAll("zzabyyab");
        REQUIRE(v.size() == 2);
        REQUIRE(v[0] == "ab");
        REQUIRE(v[1] == "ab");
    }

    SECTION("Overload with captures") {
        Regex re("(<x>ab)");
        std::vector<Match> out;
        re.findAll("zzabyy", out);
        REQUIRE(out.size() == 1);
        REQUIRE(out[0].value == "ab");
        REQUIRE(out[0].groups["x"] == "ab");
        REQUIRE(out[0][0] == "ab");
        REQUIRE(out[0][1] == "ab");
        REQUIRE(out[0][2] == "");
    }

    SECTION("Optional operator") {
        Regex re("ab?c");
        auto v = re.findAll("abc ac");
        REQUIRE(v.size() == 2);
    }

    SECTION("Plus operator") {
        Regex re("a+");
        auto v = re.findAll("xaaxaxaaax");
        REQUIRE(v.size() >= 2);
        for (const auto& match : v) {
            REQUIRE_FALSE(match.empty());
            for (char ch : match) {
                REQUIRE(ch == 'a');
            }
        }
    }
    SECTION("Dot") {
        Regex re("a.c");
        auto v = re.findAll("abc axc a-c");
        REQUIRE(v.size() == 3);
    }
    SECTION("Repeat range") {
        Regex re("a{2,3}");
        auto v = re.findAll("aaaa a aaa aa");
        REQUIRE(v.size() >= 1);
        REQUIRE(v[0].length() >= 2);
        REQUIRE(v[0].length() <= 3);
    }
    SECTION("metacharacters") {
        Regex re("&|");
        auto v = re.findAll("a|b||");
        REQUIRE(v.size() == 3);
        REQUIRE(v[0] == "|");
        REQUIRE(v[1] == "|");
        REQUIRE(v[2] == "|");
    }
    SECTION("No matches") {
        Regex re("xyz");
        auto v = re.findAll("aaaa");
        REQUIRE(v.empty());
    }
}


TEST_CASE("Regex findAll: empty string") {
    SECTION("Empty text findAll") {
        Regex re("a");
        auto v = re.findAll("");
        REQUIRE(v.empty());
    }
    SECTION("Empty text simple findAll") {
        Regex re("ab");
        std::vector<Match> out;
        re.findAll("", out);
        REQUIRE(out.empty());
    }
}

TEST_CASE("Match object: iterator and index operator") {
    Match m;
    m.value = "full";
    m.groups["a"] = "111";
    m.groups["b"] = "222";

    SECTION("Numeric index") {
        REQUIRE(m[0] == "full");
        REQUIRE((m[1] == "111" && m[2] == "222"));
        REQUIRE(m[10].empty());
    }
    SECTION("Iterator works") {
        size_t count = 0;
        for (auto const& [k,v] : m) {
            count++;
        }
        REQUIRE(count == 2);
    }
}

TEST_CASE("StateElimination") {
    DFAversion dv;
    StateElimination se;
    SECTION("Initial regex equals state-eliminated regex") {
        auto dfa1 = dv.compile("(a|b)+c");
        std::string restored = se.toRegex(dfa1);
        REQUIRE_FALSE(restored.empty());
        auto dfa2 = dv.compile(restored);
        REQUIRE(finalsCount(dfa2) == finalsCount(dfa1));
    }
    SECTION("Empty DFA returns empty regex") {
        DFA empty;
        REQUIRE(se.toRegex(empty).empty());
    }
}

TEST_CASE("Named group reference GRPREF") {
    SECTION("Same captured text must repeat") {
        Regex re("(<x>ab)<x>");
        auto v = re.findAll("xxababjjda");
        REQUIRE(v.size() == 1);
        REQUIRE(v[0] == "abab");
    }
}

TEST_CASE("DFAVersion: empty complement and reverse") {
    DFAversion dv;
    SECTION("Empty reverse") {
        DFA empty;
        DFA r = dv.reverse(empty);
        REQUIRE(r.states.empty());
        REQUIRE(r.start == nullptr);
    }
    SECTION("Empty complement") {
        DFA empty;
        DFA c = dv.complement(empty);
        REQUIRE(c.states.empty());
        REQUIRE(c.start == nullptr);
    }
}

TEST_CASE("DFAversion: GRPREF without group") {
    DFAversion dv;
    SECTION("GRPREF turns into automaton with 0 transitions and 1 state") {
        DFA dfa = dv.compile("<x>");
        REQUIRE(dfa.start != nullptr);
        REQUIRE(dfa.states.size() == 1);
        REQUIRE(dfa.start->isFinal);
        REQUIRE(dfa.start->transitions.empty());
    }
}

TEST_CASE("Empty complement") {
    SECTION("Empty") {
        DFAversion dv;
        DFA dfa = dv.compile("");
        DFA com = dv.complement(dfa);
        REQUIRE(com.states.empty());
        REQUIRE(com.start == nullptr);
        StateElimination se;
    }
    SECTION("Test reverse") {
        DFAversion dv;
        DFA dfa = dv.compile("((me)+)?p|hi{2,7}");
        DFA rev = dv.reverse(dfa);
        REQUIRE(match(rev, "iiih"));
        REQUIRE(match(rev, "p"));
        REQUIRE(match(rev, "pem"));
        REQUIRE(match(rev, "pemem"));


        /*Thompson th;
        StateElimination se;
        std::string reg = se.toRegex(rev);
        Regex re(reg);
        std::vector<std::string> res1 = re.findAll("iiih");
        std::vector<std::string> res2 = re.findAll("p");
        std::vector<std::string> res3 = re.findAll("pem");
        std::vector<std::string> res4 = re.findAll("pemem");
        std::cout << "\n";
        REQUIRE(res1.size() == 1);
        REQUIRE(res1[0] == "iiih");
        REQUIRE(res2.size() == 1);
        REQUIRE(res2[0] == "p");
        REQUIRE(res3.size() == 1);
        REQUIRE(res3[0] == "pem");
        REQUIRE(res4.size() == 1);
        REQUIRE(res4[0] == "pemem");*/
    }
}
