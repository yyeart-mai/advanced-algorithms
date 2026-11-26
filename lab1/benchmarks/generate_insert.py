import argparse
import random
from string import ascii_lowercase


KEY_LENGTH = 32


def main():
    parser = argparse.ArgumentParser(description="Generate dictionary insertion commands.")
    parser.add_argument("count", type=int, help="number of distinct keys")
    parser.add_argument("seed", type=int, help="seed for reproducible generation")
    parser.add_argument("--prefix", default="", help="common prefix of the generated keys")
    args = parser.parse_args()
    if args.count < 1:
        parser.error("count must be positive")
    prefix_length = len(args.prefix)
    if prefix_length > KEY_LENGTH or any(letter not in ascii_lowercase for letter in args.prefix):
        parser.error(f"prefix must contain at most {KEY_LENGTH} lowercase ASCII letters")
    if args.count > len(ascii_lowercase) ** (KEY_LENGTH - prefix_length):
        parser.error("count exceeds the number of distinct keys for this prefix")

    rng = random.Random(args.seed)
    seen = set()
    while len(seen) < args.count:
        key = "".join(rng.choices(ascii_lowercase, k=KEY_LENGTH))
        key = args.prefix + key[prefix_length:]
        if key in seen:
            continue
        seen.add(key)
        print(f"+ {key} {rng.getrandbits(64)}")


if __name__ == "__main__":
    main()
