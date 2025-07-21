# Korean IME (Input Method Editor)

A C-based Korean input method editor that supports real-time character composition, compound consonants/vowels, backspace handling, and multi-character input.

## Features

- **Real-time Korean character composition** from individual jamos (consonants and vowels)
- **Compound choseong (initial consonants)** support (ㄲ, ㄸ, ㅃ, ㅆ, ㅉ)
- **Compound jungseong (medial vowels)** support (ㅘ, ㅙ, ㅚ, ㅝ, ㅞ, ㅟ, ㅢ)
- **Compound jongseong (final consonants)** support (ㄳ, ㄵ, ㄶ, ㄺ, ㄻ, ㄼ, ㄽ, ㄾ, ㄿ, ㅀ, ㅄ)
- **Backspace handling** with proper character decomposition
- **Space handling** and multi-character input
- **Individual jamo display** when not forming complete syllables
- **Comprehensive test suite** with extensive compound jongseong coverage

## Compound Jongseong Testing

The test suite includes comprehensive coverage for compound jongseong (final consonants) with:

### Basic Compound Jongseong Tests
- All 11 compound jongseong combinations: ㄳ, ㄵ, ㄶ, ㄺ, ㄻ, ㄼ, ㄽ, ㄾ, ㄿ, ㅀ, ㅄ
- Tests with all 19 choseong (initial consonants)
- Tests with all 21 jungseong (medial vowels)

### Advanced Compound Jongseong Tests
- **Different Jungseong Combinations**: Tests with ㅣ, ㅜ, ㅗ, ㅓ, ㅔ jungseong
- **Edge Cases**: Compound jongseong with compound jungseong (ㅘ, ㅙ, ㅚ, ㅝ, ㅞ)
- **Following Jungseong**: Compound jongseong followed by additional jungseong
- **Multiple Sequences**: Multiple compound jongseong in sequence
- **Backspace Simulation**: Proper decomposition when backspacing
- **Invalid Combinations**: Handling of invalid compound jongseong combinations

### Test Coverage Statistics
- **Total Tests**: 287
- **Passing Tests**: 221 (77%)
- **Failing Tests**: 66 (23%)

The failing tests are primarily related to complex edge cases involving compound jungseong with compound jongseong combinations, which represent advanced Korean character composition scenarios.

## Building and Running

### Prerequisites
- GCC compiler
- Linux/Unix environment with Korean locale support

### Build
```bash
make
```

### Run Tests
```bash
make test
```

### Run Demo
```bash
make demo
```

### Clean
```bash
make clean
```

## Usage

The Korean IME supports the following input mappings:

### Choseong (Initial Consonants)
- `r` → ㄱ, `R` → ㄲ
- `s` → ㄴ, `S` → ㄴ
- `e` → ㄷ, `E` → ㄸ
- `f` → ㄹ, `F` → ㄹ
- `a` → ㅁ, `A` → ㅁ
- `q` → ㅂ, `Q` → ㅃ
- `t` → ㅅ, `T` → ㅆ
- `d` → ㅇ, `D` → ㅇ
- `w` → ㅈ, `W` → ㅉ
- `c` → ㅊ, `C` → ㅊ
- `z` → ㅋ, `Z` → ㅋ
- `x` → ㅌ, `X` → ㅌ
- `v` → ㅍ, `V` → ㅍ
- `g` → ㅎ, `G` → ㅎ

### Jungseong (Medial Vowels)
- `k` → ㅏ
- `o` → ㅐ
- `i` → ㅑ
- `O` → ㅒ
- `j` → ㅓ
- `p` → ㅔ
- `u` → ㅕ
- `P` → ㅖ
- `h` → ㅗ
- `y` → ㅛ
- `n` → ㅜ
- `b` → ㅠ
- `l` → ㅣ
- `m` → ㅡ
- `hk` → ㅘ (compound)
- `ho` → ㅙ (compound)
- `hl` → ㅚ (compound)
- `nj` → ㅝ (compound)
- `np` → ㅞ (compound)

### Jongseong (Final Consonants)
- `r` → ㄱ
- `s` → ㄴ
- `e` → ㄷ
- `f` → ㄹ
- `a` → ㅁ
- `q` → ㅂ
- `t` → ㅅ
- `d` → ㅇ
- `w` → ㅈ
- `c` → ㅊ
- `z` → ㅋ
- `x` → ㅌ
- `v` → ㅍ
- `g` → ㅎ
- `rt` → ㄳ (compound)
- `sw` → ㄵ (compound)
- `sg` → ㄶ (compound)
- `fr` → ㄺ (compound)
- `fa` → ㄻ (compound)
- `fq` → ㄼ (compound)
- `ft` → ㄽ (compound)
- `fx` → ㄾ (compound)
- `fv` → ㄿ (compound)
- `fg` → ㅀ (compound)
- `qt` → ㅄ (compound)

## Examples

### Basic Composition
- `ek` → 다 (ㄷ + ㅏ)
- `qk` → 바 (ㅂ + ㅏ)
- `ekr` → 닥 (ㄷ + ㅏ + ㄱ)

### Compound Jongseong
- `ekfr` → 닭 (ㄷ + ㅏ + ㄺ)
- `ekfa` → 닮 (ㄷ + ㅏ + ㄻ)
- `ekfq` → 닯 (ㄷ + ㅏ + ㄼ)
- `ekft` → 닰 (ㄷ + ㅏ + ㄽ)
- `ekfx` → 닱 (ㄷ + ㅏ + ㄾ)
- `ekfv` → 닲 (ㄷ + ㅏ + ㄿ)
- `ekfg` → 닳 (ㄷ + ㅏ + ㅀ)

### Compound Jungseong
- `dhk` → 와 (ㅇ + ㅘ)
- `dho` → 왜 (ㅇ + ㅙ)
- `dhl` → 외 (ㅇ + ㅚ)
- `dnj` → 워 (ㅇ + ㅝ)
- `dnp` → 웨 (ㅇ + ㅞ)

### Multiple Syllables
- `ekek` → 다다 (ㄷ + ㅏ + ㄷ + ㅏ)
- `ekfrk` → 달가 (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ)

## Architecture

The Korean IME uses a modular design with:

- **Character mapping tables** for choseong, jungseong, and jongseong
- **Real-time composition logic** that processes input progressively
- **Backspace handling** with proper character decomposition
- **Comprehensive test suite** for validation

## License

This project is open source and available under the MIT License. 