#!/usr/bin/env python3
"""Сборка примеров и сравнение с независимым перебором на малых входах."""

import argparse
import itertools
from pathlib import Path
import random
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parent
RNG = random.Random(20261112)


def run(binary, data):
    return subprocess.run(
        [str(binary)], input=data.encode("latin1"), stdout=subprocess.PIPE,
        stderr=subprocess.PIPE, check=True,
    ).stdout.decode("ascii")


def occurrences(pattern, text):
    if not pattern:
        return []
    return [i for i in range(len(text) - len(pattern) + 1)
            if text[i:i + len(pattern)] == pattern]


def brute_prefix(s):
    return [max((d for d in range(1, i + 1)
                 if s[:d] == s[i - d + 1:i + 1]), default=0)
            for i in range(len(s))]


def brute_z(s):
    result = [0] * len(s)
    for i in range(1, len(s)):
        while i + result[i] < len(s) and s[result[i]] == s[i + result[i]]:
            result[i] += 1
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sanitize", action="store_true")
    args = parser.parse_args()
    flags = ["-std=c++17", "-Wall", "-Wextra", "-Werror", "-pedantic"]
    flags += (["-O1", "-g", "-fsanitize=address,undefined",
               "-fno-omit-frame-pointer"] if args.sanitize else ["-O2"])

    with tempfile.TemporaryDirectory(prefix="text_search_verify_") as directory:
        work = Path(directory)

        def compile_file(source, target):
            subprocess.run(["c++", *flags, str(source), "-o", str(target)], check=True)
            return target

        programs = {}
        for source in sorted((ROOT / "code").glob("*.cpp")):
            programs[source.stem] = compile_file(source, work / source.stem)

        def driver(stem, body):
            source = (ROOT / "code" / f"{stem}.cpp").read_text()
            declarations = source.split("int main()", 1)[0]
            generated = work / f"{stem}_driver.cpp"
            generated.write_text(declarations + "\nint main() {\n" + body + "\n}\n")
            return compile_file(generated, work / f"{stem}_driver")

        example = "aba\nababa\n"
        for stem in ("01_naive", "03_z", "04_kmp", "05_kmp_realtime", "06_boyer_moore"):
            assert run(programs[stem], example) == "0\n2\n", stem
        assert run(programs["02_prefix"], "ababaca\n") == "0 0 1 2 3 0 1\n"
        assert run(programs["09_aho_corasick"], "2\naba\nba\nababa\n") == "0 0\n1 1\n2 0\n3 1\n"
        assert run(programs["10_suffix_tree"], "banana\nana\n") == "1\n3\n"

        words = ["".join(w) for length in range(6)
                 for w in itertools.product("ab", repeat=length)]
        cases = list(itertools.product(words, repeat=2))
        alphabet = "abc #$\x00\x80\xff"
        for _ in range(1000):
            pattern = "".join(RNG.choices(alphabet, k=RNG.randrange(13)))
            text = "".join(RNG.choices(alphabet, k=RNG.randrange(35)))
            cases.append((pattern, text))
        cases += [("a" * 60, "a" * 180), ("a" * 59 + "b", "a" * 180)]
        data = "".join(p + "\n" + t + "\n" for p, t in cases)
        expressions = {
            "01_naive": "auto answer = naiveSearch(pattern, text);",
            "03_z": "auto answer = zSearch(pattern, text);",
            "04_kmp": "auto answer = kmpSearch(pattern, text);",
            "05_kmp_realtime": """
                std::vector<int> answer;
                RealTimeKmp matcher(pattern);
                for (int i = 0; i < static_cast<int>(text.size()); ++i) {
                    if (matcher.feed(text[i]))
                        answer.push_back(i - static_cast<int>(pattern.size()) + 1);
                }
            """,
            "06_boyer_moore": "auto answer = boyerMooreSimple(pattern, text);",
            "10_suffix_tree": """
                SuffixTreeSimple tree(text);
                auto answer = tree.find(pattern);
                if (answer != tree.find(pattern)) return 2;
            """,
        }
        for stem, expression in expressions.items():
            binary = driver(stem, """
                std::string pattern, text;
                while (std::getline(std::cin, pattern) && std::getline(std::cin, text)) {
            """ + expression + """
                    std::cout << answer.size();
                    for (auto value : answer) std::cout << ' ' << value;
                    std::cout << '\\n';
                }
            """)
            lines = run(binary, data).splitlines()
            assert len(lines) == len(cases), stem
            for (pattern, text), line in zip(cases, lines):
                actual = list(map(int, line.split()))
                expected = occurrences(pattern, text)
                assert actual == [len(expected), *expected], (stem, pattern, text, actual, expected)

        array_cases = words + [text for _, text in cases[-1002:]]
        array_data = "".join(s + "\n" for s in array_cases)
        for stem, expression, oracle in (
            ("02_prefix", "auto answer = prefixFunction(s);", brute_prefix),
            ("03_z", "std::vector<int> symbols; for (unsigned char c : s) symbols.push_back(c);"
             "auto answer = zFunction(symbols);", brute_z),
        ):
            binary = driver(stem, "std::string s; while (std::getline(std::cin, s)) {" + expression + """
                std::cout << answer.size();
                for (int value : answer) std::cout << ' ' << value;
                std::cout << '\\n';
            }
            """)
            lines = run(binary, array_data).splitlines()
            assert len(lines) == len(array_cases), stem
            for s, line in zip(array_cases, lines):
                assert list(map(int, line.split())) == [len(s), *oracle(s)], (stem, s)

        binary = driver("06_boyer_moore", """
            for (auto shift : goodSuffixSimple("ababa")) std::cout << shift << ' ';
        """)
        assert run(binary, "").split() == ["2", "2", "2", "4", "4", "1"]

        commands, expected, dictionary = [], [], set()
        for op, word in [("+", "a"), ("+", "ab"), ("+", "abc"), ("-", "ab"),
                         ("?", "a"), ("?", "ab"), ("?", "abc")]:
            commands.append(f"{op} {word}")
            if op == "+":
                dictionary.add(word)
            elif op == "-":
                dictionary.discard(word)
            else:
                expected.append("YES" if word in dictionary else "NO")
        for _ in range(3000):
            op = RNG.choice("++--??")
            word = "".join(RNG.choices("abc", k=RNG.randrange(1, 13)))
            commands.append(f"{op} {word}")
            if op == "+":
                dictionary.add(word)
            elif op == "-":
                dictionary.discard(word)
            else:
                expected.append("YES" if word in dictionary else "NO")
        data = str(len(commands)) + "\n" + "\n".join(commands) + "\n"
        for stem in ("07_trie", "08_compact_trie"):
            assert run(programs[stem], data).splitlines() == expected, stem

        binary = driver("09_aho_corasick", """
            int k;
            while (std::cin >> k) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\\n');
                std::vector<std::string> patterns(k);
                for (auto& pattern : patterns) std::getline(std::cin, pattern);
                std::string text;
                std::getline(std::cin, text);
                AhoCorasick matcher(patterns);
                auto answer = matcher.search(text);
                if (answer != matcher.search(text)) return 2;
                std::cout << answer.size();
                for (auto match : answer) std::cout << ' ' << match.first << ' ' << match.second;
                std::cout << '\\n';
            }
        """)
        multi_cases = [(["he", "she", "e", "he"], "she"),
                       (["a", "aa", "a", ""], "aaaa"), ([], "abc")]
        for _ in range(700):
            patterns = ["".join(RNG.choices(alphabet, k=RNG.randrange(7)))
                        for _ in range(RNG.randrange(9))]
            text = "".join(RNG.choices(alphabet, k=RNG.randrange(40)))
            multi_cases.append((patterns, text))
        data = "".join(str(len(patterns)) + "\n" + "".join(p + "\n" for p in patterns)
                       + text + "\n" for patterns, text in multi_cases)
        lines = run(binary, data).splitlines()
        assert len(lines) == len(multi_cases)
        for (patterns, text), line in zip(multi_cases, lines):
            values = list(map(int, line.split()))
            actual = list(zip(values[1::2], values[2::2]))
            expected = [(start, index) for index, pattern in enumerate(patterns)
                        for start in occurrences(pattern, text)]
            expected.sort(key=lambda match: (match[0] + len(patterns[match[1]]),
                                             -len(patterns[match[1]]), match[1]))
            assert values[0] == len(expected) and actual == expected, (patterns, text, actual, expected)

        print(f"OK: {len(programs)} programs, {len(cases)} cases per single-pattern matcher, "
              f"{len(array_cases)} cases per array function, {len(commands)} dictionary commands, "
              f"{len(multi_cases)} multi-pattern cases")


if __name__ == "__main__":
    main()
