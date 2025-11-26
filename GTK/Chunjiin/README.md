# 천지인 한글 입력기 (Chunjiin Korean Input Method)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![GUI: GTK3](https://img.shields.io/badge/GUI-GTK3-green.svg)](https://www.gtk.org/)

C언어와 GTK3로 구현한 천지인 한글 입력 방식 애플리케이션입니다.

## 소개

천지인(天地人) 입력 방식은 한글의 창제 원리를 따라 만들어진 휴대폰 키패드 입력 방식입니다. 천(天, 하늘 - 가로획 ·), 지(地, 땅 - 세로획 ㅣ), 인(人, 사람 - 가로획 ㅡ)의 세 가지 기본 요소로 모든 한글을 입력할 수 있습니다.

이 프로그램은 원래 Java/Android로 작성된 코드([sigigi/dotnet_chunjiin](https://github.com/sigigi/dotnet_chunjiin))를 C 언어로 포팅하고 GTK3 GUI를 추가한 버전입니다.

### 특징

- ✅ **완전한 한글 입력 지원** - 초성, 중성, 종성, 11가지 겹받침 모두 지원
- ✅ **영문 입력 지원** - 대문자/소문자 모드 전환
- ✅ **숫자 입력 지원** - 0-9 직접 입력
- ✅ **특수문자 입력 지원** - 30가지 특수문자 입력
- ✅ **5가지 입력 모드** - 한글, 영문(대), 영문(소), 숫자, 특수문자
- ✅ **직관적인 GUI** - GTK3 기반의 컴팩트한 인터페이스 (320x640)
- ✅ **안정적인 문자 처리** - wchar_t/UTF-8 변환으로 정확한 한글 표시
- ✅ **크로스 플랫폼** - Linux, Windows (Cygwin/MSYS2) 지원

## 빌드 요구사항

### Linux

```bash
# Ubuntu/Debian
sudo apt-get install build-essential libgtk-3-dev

# Fedora/RHEL
sudo dnf install gcc gtk3-devel

# Arch Linux
sudo pacman -S gcc gtk3
```

### Cygwin (Windows)

setup-x86_64.exe에서 다음 패키지 설치:
- gcc-core
- make
- pkg-config
- libgtk3-devel

### MSYS2 (Windows)

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk3
```

## 빌드 방법

```bash
# 저장소 클론 (또는 소스 다운로드)
git clone https://github.com/sigigi/dotnet_chunjiin.git
cd dotnet_chunjiin/Chunjiin

# 빌드
make

# 실행
./chunjiin      # Linux/Cygwin
# 또는
./chunjiin.exe  # Windows

# 빌드와 실행을 한번에
make run

# 빌드 파일 정리
make clean

# 시스템에 설치 (선택사항)
sudo make install
```

### Makefile 타겟

| 타겟 | 설명 |
|------|------|
| `make` | 애플리케이션 빌드 |
| `make run` | 빌드 후 실행 |
| `make clean` | 빌드 파일 제거 |
| `make install` | /usr/local/bin에 설치 |
| `make help` | 도움말 표시 |

## 사용 방법

### 버튼 레이아웃

```
┌──────────────────────────────┐
│     모드 변경 버튼           │
├──────┬──────┬──────┐
│  1   │  2   │  3   │  첫째 행: 천, 지, 인
├──────┼──────┼──────┤
│  4   │  5   │  6   │  둘째 행: ㄱ, ㄴ, ㄷ
├──────┼──────┼──────┤
│  7   │  8   │  9   │  셋째 행: ㅂ, ㅅ, ㅈ
├──────┼──────┼──────┤
│Space │  0   │ Del  │  넷째 행: 공백, ㅇㅁ, 삭제
├──────┴──────┴──────┤
│   전체 지우기         │  다섯째 행
└──────────────────────┘
```

### 한글 모드

#### 자음 입력

- **버튼 0**: ㅇ → ㅁ → ㅇ (반복)
- **버튼 4**: ㄱ → ㅋ → ㄲ → ㄱ
- **버튼 5**: ㄴ → ㄹ → ㄴ
- **버튼 6**: ㄷ → ㅌ → ㄸ → ㄷ
- **버튼 7**: ㅂ → ㅍ → ㅃ → ㅂ
- **버튼 8**: ㅅ → ㅎ → ㅆ → ㅅ
- **버튼 9**: ㅈ → ㅊ → ㅉ → ㅈ

#### 모음 입력

- **버튼 1**: ㅣ (세로 획)
- **버튼 2**: · (점, 가로 획의 시작)
- **버튼 3**: ㅡ (가로 획)

#### 복합 모음 만들기

- ㅏ = ㅣ(1) + ·(2)
- ㅑ = ㅣ(1) + ·(2) + ·(2)
- ㅓ = ·(2) + ㅣ(1)
- ㅕ = ·(2) + ·(2) + ㅣ(1)
- ㅗ = ·(2) + ㅡ(3)
- ㅛ = ·(2) + ·(2) + ㅡ(3)
- ㅜ = ㅡ(3) + ·(2)
- ㅠ = ㅡ(3) + ·(2) + ·(2)
- ㅐ = ㅣ(1) + ·(2) + ㅣ(1)
- ㅔ = ·(2) + ㅣ(1) + ㅣ(1)

#### 예시: "한글" 입력하기

**"한"** 입력:
1. `8` (ㅎ)
2. `1` (ㅣ) + `2` (·) = ㅏ
3. `5` (ㄴ) - 종성

**"글"** 입력:
1. `4` (ㄱ)
2. `3` (ㅡ)
3. `5` (ㄹ) - 종성

### 영문 모드

각 버튼을 반복해서 눌러 해당 문자를 선택합니다.

- **버튼 0**: @ → ? → ! → @
- **버튼 1**: . → Q → Z → .
- **버튼 2**: A → B → C → A
- **버튼 3**: D → E → F → D
- **버튼 4**: G → H → I → G
- **버튼 5**: J → K → L → J
- **버튼 6**: M → N → O → M
- **버튼 7**: P → R → S → P
- **버튼 8**: T → U → V → T
- **버튼 9**: W → X → Y → W

### 숫자 모드

각 버튼이 해당하는 숫자를 입력합니다 (0~9).

### 특수문자 모드

각 버튼을 반복해서 눌러 해당 특수문자를 선택합니다.

- **버튼 0**: ~ → ` → ^ → ~
- **버튼 1**: ! → @ → # → !
- **버튼 2**: $ → % → & → $
- **버튼 3**: * → ( → ) → =
- **버튼 4**: + → { → } → +
- **버튼 5**: [ → ] → = → [
- **버튼 6**: < → > → | → <
- **버튼 7**: - → _ → -
- **버튼 8**: : → ; → :
- **버튼 9**: " → ' → / → "

### 모드 전환

상단의 **"모드 변경"** 버튼을 클릭하여 다음 순서로 모드를 전환할 수 있습니다:

```
한글 → 영문(대) → 영문(소) → 숫자 → 특수문자 → 한글 → ...
```

## 파일 구조

```
Chunjiin/
├── chunjiin.h            # 헤더 파일 (구조체 및 함수 선언)
├── chunjiin.c            # 핵심 로직 및 유틸리티
├── chunjiin_hangul.c     # 한글 입력 로직 (hangul_make, eng_make, num_make, write 함수)
├── main.c                # GTK3 GUI 애플리케이션
├── Makefile              # 빌드 설정
├── README.md             # 이 문서
├── .gitignore            # Git 제외 파일 목록
├── Chunjiin.java         # 원본 Java 코드 (참고용)
└── Chunjiinactivity.java # 원본 Android Activity (참고용)
```

### 코드 구조

| 파일 | 주요 함수 | 설명 |
|------|----------|------|
| **chunjiin.h** | - | 데이터 구조체 및 함수 프로토타입 정의 (5가지 모드) |
| **chunjiin.c** | `get_unicode()`<br>`check_double()`<br>`change_mode()`<br>`get_button_text()` | 한글 유니코드 조합, 겹받침 처리, 모드 전환, 버튼 텍스트 매핑 |
| **chunjiin_hangul.c** | `hangul_make()`<br>`eng_make()`<br>`num_make()`<br>`special_make()`<br>`write_hangul()`<br>`write_engnum()` | 각 모드별 입력 처리 및 텍스트 출력 |
| **main.c** | `wchar_to_utf8()`<br>`on_button_clicked()`<br>`on_mode_button_clicked()`<br>`activate()` | GTK GUI (320x640), 이벤트 처리, UTF-8 변환 |

## 기술 세부사항

### 문자 인코딩

- 내부적으로 `wchar_t` (UTF-32 on Linux) 사용
- GTK 디스플레이를 위해 UTF-8로 자동 변환
- 한글 유니코드 조합 알고리즘 구현 (초성 + 중성 + 종성)

### 한글 조합 원리

한글 완성형 유니코드 = `44032 + (초성 × 588) + (중성 × 28) + 종성`

예: "한" = ㅎ(18) + ㅏ(0) + ㄴ(4)
     = 44032 + (18 × 588) + (0 × 28) + 4
     = 54620 (0xD55C)

### 겹받침 지원

다음 겹받침을 완벽하게 지원합니다:
- ㄳ (ㄱ + ㅅ)
- ㄵ (ㄴ + ㅈ)
- ㄶ (ㄴ + ㅎ)
- ㄺ (ㄹ + ㄱ)
- ㄻ (ㄹ + ㅁ)
- ㄼ (ㄹ + ㅂ)
- ㄽ (ㄹ + ㅅ)
- ㄾ (ㄹ + ㅌ)
- ㄿ (ㄹ + ㅍ)
- ㅀ (ㄹ + ㅎ)
- ㅄ (ㅂ + ㅅ)

## 라이선스

MIT License

원본 저작권: Copyright (c) 2014 KimYs (a.k.a ZeDA)
- Blog: http://blog.naver.com/irineu2
- GitHub: https://github.com/sigigi/dotnet_chunjiin

C/GTK 포트: Copyright (c) 2025 Claude (Anthropic)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## 문제 해결

### GTK3를 찾을 수 없다는 오류

```bash
# pkg-config가 GTK3를 찾을 수 있는지 확인
pkg-config --modversion gtk+-3.0

# GTK3가 설치되어 있지 않다면 위의 "빌드 요구사항" 섹션을 참고하여 설치하세요
```

### Cygwin에서 실행 시 디스플레이 오류

```bash
# X11 서버가 실행 중인지 확인
# Cygwin/X를 설치하고 startxwin을 실행하세요
```

### Windows에서 한글이 깨지는 경우

한글 폰트가 설치되어 있는지 확인하세요. Windows에는 기본적으로 맑은 고딕, 돋움 등의 한글 폰트가 포함되어 있습니다.

### 버튼 클릭 시 텍스트가 업데이트되지 않는 경우

이 문제는 wchar_t to UTF-8 변환 이슈로 이미 수정되었습니다. 최신 버전을 사용하고 있는지 확인하세요.

### 컴파일 경고 발생

일부 시스템에서 `-Wdeprecated-declarations` 경고가 발생할 수 있습니다. 이는 GTK3의 폰트 설정 함수 때문이며 정상적으로 동작합니다.

## 개발 정보

### 빌드 환경

- **컴파일러**: GCC 4.8 이상
- **표준**: C99
- **의존성**: GTK+ 3.0, GLib 2.0

### 메모리 관리

- 텍스트 버퍼 최대 크기: 1024 wchar_t (약 4KB)
- 동적 메모리 할당 최소화
- GTK 위젯 자동 메모리 관리

### 디버깅

```bash
# 디버그 정보 포함하여 빌드
gcc -g -Wall -Wextra $(pkg-config --cflags gtk+-3.0) *.c -o chunjiin $(pkg-config --libs gtk+-3.0)

# Valgrind로 메모리 누수 체크
valgrind --leak-check=full ./chunjiin

# GDB 디버거 실행
gdb ./chunjiin
```

## 기여

버그 리포트나 개선 제안은 환영합니다!

### 기여 방법

1. 이 저장소를 Fork 합니다
2. 새 브랜치를 생성합니다 (`git checkout -b feature/amazing-feature`)
3. 변경사항을 커밋합니다 (`git commit -m 'Add amazing feature'`)
4. 브랜치에 Push 합니다 (`git push origin feature/amazing-feature`)
5. Pull Request를 생성합니다

### 코딩 스타일

- 들여쓰기: 스페이스 4칸
- 변수명: snake_case
- 함수명: snake_case
- 구조체: PascalCase

## 참고 자료

- 원본 Java/Android 코드: [GitHub - sigigi/dotnet_chunjiin](https://github.com/sigigi/dotnet_chunjiin)
- 천지인 입력법: [Wikipedia - 천지인 (입력기)](https://ko.wikipedia.org/wiki/천지인_(입력기))
- 한글 유니코드: [Wikipedia - 한글 유니코드](https://ko.wikipedia.org/wiki/한글_유니코드)
- GTK3 문서: [GTK Documentation](https://docs.gtk.org/gtk3/)
- 한글 자모 유니코드 범위:
  - 초성: U+1100 ~ U+1112 (19자)
  - 중성: U+1161 ~ U+1175 (21자)
  - 종성: U+11A8 ~ U+11C2 (27자)
  - 완성형: U+AC00 ~ U+D7A3 (11,172자)

## 버전 히스토리

### v1.1.0 (2025)
- ✨ 특수문자 입력 모드 추가 (30가지 특수문자)
- 🎨 UI 레이아웃 개선 (320x640 컴팩트 디자인)
- 🎨 버튼 배치 재구성 (5행 x 3열)
- 🔧 모드 표시 레이블 제거 (간결한 UI)
- 🐛 GTK 버튼 NULL 체크 추가

### v1.0.0 (2025)
- ✨ 초기 C/GTK3 포팅 완료
- ✨ 한글, 영문, 숫자 입력 모드 구현
- ✨ 11가지 겹받침 완벽 지원
- 🐛 wchar_t to UTF-8 변환 이슈 수정
- 📝 완전한 문서화

## UI 구성

애플리케이션 레이아웃 (320x640 픽셀):
- **최상단**: 제목 ("천지인 한글 입력기")
- **상단**: 모드 변경 버튼 (한글→ENG→eng→123→!@#)
- **중앙**: 입력 텍스트 표시 영역 (스크롤 가능)
- **하단**: 버튼 그리드 (5행 x 3열)
  - 1-3행: 한글 자모 버튼 (1-9)
  - 4행: Space, ㅇㅁ(0), Del
  - 5행: 전체 지우기 (3칸 너비)

## 라이선스 고지

이 프로젝트는 MIT 라이선스 하에 배포됩니다. 자유롭게 사용, 수정, 배포할 수 있습니다.

---

Made with ❤️ using C and GTK3

**Original Author**: KimYs (ZeDA) - [Blog](http://blog.naver.com/irineu2)
**C/GTK Port**: Claude (Anthropic) - 2025
