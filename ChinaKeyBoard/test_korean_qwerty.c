/*
 * Comprehensive Korean QWERTY Input Test Cases
 * 
 * Tests all possible combinations of:
 * - Chosung (초성) - Initial consonants
 * - Jungsung (중성) - Medial vowels  
 * - Jongsung (종성) - Final consonants
 * - Diphthongs (이중모음) - Double vowels
 * - Double consonants (쌍자음) - Shifted consonants
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ime/libime/ime_korean.h"

// Test result structure
typedef struct {
    char input[16];
    char expected[16];
    char actual[16];
    int passed;
    char description[64];
} test_result_t;

// Helper functions
static void append_stroke(char* strokes_buffer, char new_char) {
    int len = strlen(strokes_buffer);
    strokes_buffer[len] = new_char;
    strokes_buffer[len+1] = '\0';
}

static void print_hex(const char* str) {
    printf("Hex: ");
    for (int i = 0; str[i]; i++) {
        printf("%02X ", (unsigned char)str[i]);
    }
    printf("\n");
}

// Test case definitions
typedef struct {
    char input[16];
    char expected[16];
    char description[64];
} test_case_t;

// Chosung (초성) - Initial consonants (expect Unicode jamo characters)
static const test_case_t chosung_tests[] = {
    {"r", "ᄀ", "ㄱ (giyeok)"},
    {"s", "ᄂ", "ㄴ (nieun)"},
    {"e", "ᄃ", "ㄷ (digeut)"},
    {"f", "ᄅ", "ㄹ (rieul)"},
    {"a", "ᄆ", "ㅁ (mieum)"},
    {"q", "ᄇ", "ㅂ (bieup)"},
    {"t", "ᄉ", "ㅅ (siot)"},
    {"d", "ᄋ", "ㅇ (ieung)"},
    {"w", "ᄌ", "ㅈ (jieut)"},
    {"c", "ᄎ", "ㅊ (chieut)"},
    {"z", "ᄏ", "ㅋ (kieuk)"},
    {"x", "ᄐ", "ㅌ (tieut)"},
    {"v", "ᄑ", "ㅍ (pieup)"},
    {"g", "ᄒ", "ㅎ (hieut)"},
    {"R", "ᄁ", "ㄲ (ssanggiyeok)"},
    {"E", "ᄄ", "ㄸ (ssangdigeut)"},
    {"Q", "ᄈ", "ㅃ (ssangbieup)"},
    {"T", "ᄊ", "ㅆ (ssangsiot)"},
    {"W", "ᄍ", "ㅉ (ssangjieut)"},
    {"", "", ""} // End marker
};

// Jungsung (중성) - Medial vowels (expect complete syllables with ㅇ initial)
static const test_case_t jungsung_tests[] = {
    {"k", "아", "ㅏ (a)"},
    {"K", "애", "ㅐ (ae)"},
    {"i", "야", "ㅑ (ya)"},
    {"o", "얘", "ㅒ (yae)"},
    {"O", "어", "ㅓ (eo)"},
    {"j", "에", "ㅔ (e)"},
    {"p", "여", "ㅕ (yeo)"},
    {"u", "예", "ㅖ (ye)"},
    {"h", "오", "ㅗ (o)"},
    {"y", "요", "ㅛ (yo)"},
    {"n", "우", "ㅜ (u)"},
    {"b", "유", "ㅠ (yu)"},
    {"m", "으", "ㅡ (eu)"},
    {"l", "이", "ㅣ (i)"},
    {"", "", ""} // End marker
};

// Jongsung (종성) - Final consonants (expect complete syllables with final consonants)
// Testing final consonants with ㅇ initial + ㅏ vowel + various finals
static const test_case_t jongsung_tests[] = {
    {"dkr", "악", "ㄱ (giyeok)"},
    {"dks", "안", "ㄴ (nieun)"},
    {"dke", "앋", "ㄷ (digeut)"},
    {"dkf", "알", "ㄹ (rieul)"},
    {"dka", "암", "ㅁ (mieum)"},
    {"dkq", "압", "ㅂ (bieup)"},
    {"dkt", "앗", "ㅅ (siot)"},
    {"dkd", "앙", "ㅇ (ieung)"},
    {"dkw", "앚", "ㅈ (jieut)"},
    {"dkc", "앛", "ㅊ (chieut)"},
    {"dkz", "앜", "ㅋ (kieuk)"},
    {"dkx", "앝", "ㅌ (tieut)"},
    {"dkv", "앞", "ㅍ (pieup)"},
    {"dkg", "앟", "ㅎ (hieut)"},
    {"dkR", "앆", "ㄲ (ssanggiyeok)"},
    {"dkE", "아", "ㄸ (ssangdigeut)"},
    {"dkQ", "아", "ㅃ (ssangbieup)"},
    {"dkT", "았", "ㅆ (ssangsiot)"},
    {"dkW", "아", "ㅉ (ssangjieut)"},
    {"", "", ""} // End marker
};

// Diphthong tests (expect complete syllables with diphthongs)
static const test_case_t diphthong_tests[] = {
    {"hl", "외", "ㅗ + ㅣ = ㅚ (oe)"},
    {"nl", "위", "ㅜ + ㅣ = ㅟ (wi)"},
    {"hk", "와", "ㅗ + ㅏ = ㅘ (wa)"},
    {"nO", "워", "ㅜ + ㅓ = ㅝ (wo)"},
    {"nj", "웨", "ㅜ + ㅔ = ㅞ (we)"},
    {"ml", "의", "ㅡ + ㅣ = ㅢ (ui)"},
    {"", "", ""} // End marker
};

// Complete syllable tests (Chosung + Jungsung + Jongsung)
static const test_case_t complete_syllable_tests[] = {
    {"rk", "가", "ㄱ + ㅏ = 가 (ga)"},
    {"gks", "한", "ㅎ + ㅏ + ㄴ = 한 (han)"},
    {"rmf", "글", "ㄱ + ㅡ + ㄹ = 글 (geul)"},
    {"dks", "안", "ㅇ + ㅏ + ㄴ = 안 (an)"},
    {"rks", "간", "ㄱ + ㅏ + ㄴ = 간 (gan)"},
    {"rma", "금", "ㄱ + ㅡ + ㅁ = 금 (geum)"},
    {"rkd", "강", "ㄱ + ㅏ + ㅇ = 강 (gang)"},
    {"rkt", "갓", "ㄱ + ㅏ + ㅅ = 갓 (gat)"},
    {"rka", "감", "ㄱ + ㅏ + ㅁ = 감 (gam)"},
    {"rkr", "각", "ㄱ + ㅏ + ㄱ = 각 (gak)"},
    {"", "", ""} // End marker
};

// Diphthong syllable tests
static const test_case_t diphthong_syllable_tests[] = {
    {"rhl", "괴", "ㄱ + ㅚ = 괴 (goe)"},
    {"gnl", "휘", "ㅎ + ㅟ = 휘 (hwi)"},
    {"rhk", "과", "ㄱ + ㅘ = 과 (gwa)"},
    {"gnO", "훠", "ㅎ + ㅝ = 훠 (hwo)"},
    {"rml", "긔", "ㄱ + ㅢ = 긔 (gui)"},
    {"", "", ""} // End marker
};

// Comprehensive double consonant tests
static const test_case_t double_consonant_tests[] = {
    // Basic double consonant + vowel combinations
    {"Rk", "까", "ㄲ + ㅏ = 까 (kka)"},
    {"Qk", "빠", "ㅃ + ㅏ = 빠 (ppa)"},
    {"Ek", "따", "ㄸ + ㅏ = 따 (tta)"},
    {"Tk", "싸", "ㅆ + ㅏ = 싸 (ssa)"},
    {"Wk", "짜", "ㅉ + ㅏ = 짜 (jja)"},
    
    // Double consonant + different vowels
    {"RK", "깨", "ㄲ + ㅐ = 깨 (kkae)"},
    {"Qo", "뺴", "ㅃ + ㅒ = 뺴 (ppyae)"},
    {"EO", "떠", "ㄸ + ㅓ = 떠 (tteo)"},
    {"Tj", "쎄", "ㅆ + ㅔ = 쎄 (sse)"},
    {"Wp", "쪄", "ㅉ + ㅕ = 쪄 (jjyeo)"},
    {"Ru", "꼐", "ㄲ + ㅖ = 꼐 (kkye)"},
    {"Qh", "뽀", "ㅃ + ㅗ = 뽀 (ppo)"},
    {"Ehk", "똬", "ㄸ + ㅘ = 똬 (ttwa)"},
    {"Ty", "쑈", "ㅆ + ㅛ = 쑈 (ssyo)"},
    {"Wn", "쭈", "ㅉ + ㅜ = 쭈 (jju)"},
    {"Rb", "뀨", "ㄲ + ㅠ = 뀨 (kkyu)"},
    {"Qm", "쁘", "ㅃ + ㅡ = 쁘 (ppeu)"},
    {"El", "띠", "ㄸ + ㅣ = 띠 (tti)"},
    
    // Double consonant + vowel + final consonant
    {"Rks", "깐", "ㄲ + ㅏ + ㄴ = 깐 (kkan)"},
    {"Qks", "빤", "ㅃ + ㅏ + ㄴ = 빤 (ppan)"},
    {"Eks", "딴", "ㄸ + ㅏ + ㄴ = 딴 (ttan)"},
    {"Tks", "싼", "ㅆ + ㅏ + ㄴ = 싼 (ssan)"},
    {"Wks", "짠", "ㅉ + ㅏ + ㄴ = 짠 (jjan)"},
    
    // Double consonant + diphthong
    {"Rhk", "꽈", "ㄲ + ㅘ = 꽈 (kkwa)"},
    {"Qnl", "쀠", "ㅃ + ㅟ = 쀠 (ppwi)"},
    {"Ehl", "뙤", "ㄸ + ㅚ = 뙤 (ttoe)"},
    {"Tml", "씌", "ㅆ + ㅢ = 씌 (ssui)"},
    {"Wnl", "쮜", "ㅉ + ㅟ = 쮜 (jjwi)"},
    
    // Double consonant + diphthong + final consonant
    {"Rhks", "꽌", "ㄲ + ㅘ + ㄴ = 꽌 (kkwan)"},
    {"Qnls", "쀤", "ㅃ + ㅟ + ㄴ = 쀤 (ppwin)"},
    {"Ehls", "뙨", "ㄸ + ㅚ + ㄴ = 뙨 (ttoen)"},
    {"Tmls", "씐", "ㅆ + ㅢ + ㄴ = 씐 (ssuin)"},
    {"Wnls", "쮠", "ㅉ + ㅟ + ㄴ = 쮠 (jjwin)"},
    
    // Complex double consonant combinations
    {"Rkr", "깍", "ㄲ + ㅏ + ㄱ = 깍 (kkak)"},
    {"Qka", "빰", "ㅃ + ㅏ + ㅁ = 빰 (ppam)"},
    {"Ekt", "땃", "ㄸ + ㅏ + ㅅ = 땃 (ttat)"},
    {"Tkd", "쌍", "ㅆ + ㅏ + ㅇ = 쌍 (ssang)"},
    {"Wkw", "짲", "ㅉ + ㅏ + ㅈ = 짲 (jjat)"},
    
    {"", "", ""} // End marker
};

// Comprehensive double jungsung (diphthong) tests
static const test_case_t double_jungsung_tests[] = {
    // === BASIC DIPHTHONGS (all 7 standard combinations) ===
    {"hl", "외", "ㅗ + ㅣ = ㅚ (oe)"},
    {"nl", "위", "ㅜ + ㅣ = ㅟ (wi)"},
    {"hk", "와", "ㅗ + ㅏ = ㅘ (wa)"},
    {"nO", "워", "ㅜ + ㅓ = ㅝ (wo)"},
    {"nj", "웨", "ㅜ + ㅔ = ㅞ (we)"},
    {"ml", "의", "ㅡ + ㅣ = ㅢ (ui)"},
    {"hK", "왜", "ㅗ + ㅐ = ㅙ (wae)"},

    // === DIPHTHONGS WITH VARIOUS INITIAL CONSONANTS ===
    // ㅚ (oe) family - ㅗ + ㅣ
    {"rhl", "괴", "ㄱ + ㅚ = 괴 (goe)"},
    {"shl", "뇌", "ㄴ + ㅚ = 뇌 (noe)"},
    {"ehl", "되", "ㄷ + ㅚ = 되 (doe)"},
    {"fhl", "뢰", "ㄹ + ㅚ = 뢰 (loe)"},
    {"ahl", "뫼", "ㅁ + ㅚ = 뫼 (moe)"},
    {"qhl", "뵈", "ㅂ + ㅚ = 뵈 (boe)"},
    {"thl", "쇠", "ㅅ + ㅚ = 쇠 (soe)"},
    {"whl", "죄", "ㅈ + ㅚ = 죄 (joe)"},
    {"chl", "최", "ㅊ + ㅚ = 최 (choe)"},
    {"zhl", "쾨", "ㅋ + ㅚ = 쾨 (koe)"},
    {"xhl", "퇴", "ㅌ + ㅚ = 퇴 (toe)"},
    {"vhl", "푀", "ㅍ + ㅚ = 푀 (poe)"},
    {"ghl", "회", "ㅎ + ㅚ = 회 (hoe)"},

    // ㅟ (wi) family - ㅜ + ㅣ
    {"gnl", "휘", "ㅎ + ㅟ = 휘 (hwi)"},
    {"rnl", "귀", "ㄱ + ㅟ = 귀 (gwi)"},
    {"snl", "뉘", "ㄴ + ㅟ = 뉘 (nwi)"},
    {"enl", "뒤", "ㄷ + ㅟ = 뒤 (dwi)"},
    {"fnl", "뤼", "ㄹ + ㅟ = 뤼 (lwi)"},
    {"anl", "뮈", "ㅁ + ㅟ = 뮈 (mwi)"},
    {"qnl", "뷔", "ㅂ + ㅟ = 뷔 (bwi)"},
    {"tnl", "쉬", "ㅅ + ㅟ = 쉬 (swi)"},
    {"wnl", "쥐", "ㅈ + ㅟ = 쥐 (jwi)"},
    {"cnl", "취", "ㅊ + ㅟ = 취 (chwi)"},
    {"znl", "퀴", "ㅋ + ㅟ = 퀴 (kwi)"},
    {"xnl", "튀", "ㅌ + ㅟ = 튀 (twi)"},
    {"vnl", "퓌", "ㅍ + ㅟ = 퓌 (pwi)"},

    // ㅘ (wa) family - ㅗ + ㅏ
    {"rhk", "과", "ㄱ + ㅘ = 과 (gwa)"},
    {"shk", "놔", "ㄴ + ㅘ = 놔 (nwa)"},
    {"ehk", "돠", "ㄷ + ㅘ = 돠 (dwa)"},
    {"fhk", "롸", "ㄹ + ㅘ = 롸 (lwa)"},
    {"ahk", "뫄", "ㅁ + ㅘ = 뫄 (mwa)"},
    {"qhk", "봐", "ㅂ + ㅘ = 봐 (bwa)"},
    {"thk", "솨", "ㅅ + ㅘ = 솨 (swa)"},
    {"whk", "좌", "ㅈ + ㅘ = 좌 (jwa)"},
    {"chk", "촤", "ㅊ + ㅘ = 촤 (chwa)"},
    {"zhk", "콰", "ㅋ + ㅘ = 콰 (kwa)"},
    {"xhk", "톼", "ㅌ + ㅘ = 톼 (twa)"},
    {"vhk", "퐈", "ㅍ + ㅘ = 퐈 (pwa)"},
    {"ghk", "화", "ㅎ + ㅘ = 화 (hwa)"},

    // ㅙ (wae) family - ㅗ + ㅐ
    {"rhK", "괘", "ㄱ + ㅙ = 괘 (gwae)"},
    {"shK", "놰", "ㄴ + ㅙ = 놰 (nwae)"},
    {"ehK", "돼", "ㄷ + ㅙ = 돼 (dwae)"},
    {"fhK", "뢔", "ㄹ + ㅙ = 뢔 (lwae)"},
    {"ahK", "뫠", "ㅁ + ㅙ = 뫠 (mwae)"},
    {"qhK", "봬", "ㅂ + ㅙ = 봬 (bwae)"},
    {"thK", "쇄", "ㅅ + ㅙ = 쇄 (swae)"},
    {"whK", "좨", "ㅈ + ㅙ = 좨 (jwae)"},
    {"chK", "쵀", "ㅊ + ㅙ = 쵀 (chwae)"},
    {"zhK", "쾌", "ㅋ + ㅙ = 쾌 (kwae)"},
    {"xhK", "퇘", "ㅌ + ㅙ = 퇘 (twae)"},
    {"vhK", "퐤", "ㅍ + ㅙ = 퐤 (pwae)"},
    {"ghK", "홰", "ㅎ + ㅙ = 홰 (hwae)"},

    // ㅝ (wo) family - ㅜ + ㅓ
    {"gnO", "훠", "ㅎ + ㅝ = 훠 (hwo)"},
    {"rnO", "궈", "ㄱ + ㅝ = 궈 (gwo)"},
    {"snO", "눠", "ㄴ + ㅝ = 눠 (nwo)"},
    {"enO", "둬", "ㄷ + ㅝ = 둬 (dwo)"},
    {"fnO", "뤄", "ㄹ + ㅝ = 뤄 (lwo)"},
    {"anO", "뭐", "ㅁ + ㅝ = 뭐 (mwo)"},
    {"qnO", "붜", "ㅂ + ㅝ = 붜 (bwo)"},
    {"tnO", "숴", "ㅅ + ㅝ = 숴 (swo)"},
    {"wnO", "줘", "ㅈ + ㅝ = 줘 (jwo)"},
    {"cnO", "춰", "ㅊ + ㅝ = 춰 (chwo)"},
    {"znO", "쿼", "ㅋ + ㅝ = 쿼 (kwo)"},
    {"xnO", "퉈", "ㅌ + ㅝ = 퉈 (two)"},
    {"vnO", "풔", "ㅍ + ㅝ = 풔 (pwo)"},

    // ㅞ (we) family - ㅜ + ㅔ
    {"rnj", "궤", "ㄱ + ㅞ = 궤 (gwe)"},
    {"snj", "눼", "ㄴ + ㅞ = 눼 (nwe)"},
    {"enj", "뒈", "ㄷ + ㅞ = 뒈 (dwe)"},
    {"fnj", "뤠", "ㄹ + ㅞ = 뤠 (lwe)"},
    {"anj", "뭬", "ㅁ + ㅞ = 뭬 (mwe)"},
    {"qnj", "붸", "ㅂ + ㅞ = 붸 (bwe)"},
    {"tnj", "쉐", "ㅅ + ㅞ = 쉐 (swe)"},
    {"wnj", "줴", "ㅈ + ㅞ = 줴 (jwe)"},
    {"cnj", "췌", "ㅊ + ㅞ = 췌 (chwe)"},
    {"znj", "퀘", "ㅋ + ㅞ = 퀘 (kwe)"},
    {"xnj", "퉤", "ㅌ + ㅞ = 퉤 (twe)"},
    {"vnj", "풰", "ㅍ + ㅞ = 풰 (pwe)"},
    {"gnj", "훼", "ㅎ + ㅞ = 훼 (hwe)"},

    // ㅢ (ui) family - ㅡ + ㅣ
    {"rml", "긔", "ㄱ + ㅢ = 긔 (gui)"},
    {"sml", "늬", "ㄴ + ㅢ = 늬 (nui)"},
    {"eml", "듸", "ㄷ + ㅢ = 듸 (dui)"},
    {"fml", "릐", "ㄹ + ㅢ = 릐 (lui)"},
    {"aml", "믜", "ㅁ + ㅢ = 믜 (mui)"},
    {"qml", "븨", "ㅂ + ㅢ = 븨 (bui)"},
    {"tml", "싀", "ㅅ + ㅢ = 싀 (sui)"},
    {"wml", "즤", "ㅈ + ㅢ = 즤 (jui)"},
    {"cml", "츼", "ㅊ + ㅢ = 츼 (chui)"},
    {"zml", "킈", "ㅋ + ㅢ = 킈 (kui)"},
    {"xml", "틔", "ㅌ + ㅢ = 틔 (tui)"},
    {"vml", "픠", "ㅍ + ㅢ = 픠 (pui)"},
    {"gml", "희", "ㅎ + ㅢ = 희 (hui)"},

    // === DIPHTHONGS WITH DOUBLE CONSONANTS ===
    {"Rhl", "꾀", "ㄲ + ㅚ = 꾀 (kkoe)"},
    {"Qnl", "쀠", "ㅃ + ㅟ = 쀠 (ppwi)"},
    {"Rhk", "꽈", "ㄲ + ㅘ = 꽈 (kkwa)"},
    {"QnO", "뿨", "ㅃ + ㅝ = 뿨 (ppwo)"},
    {"Enj", "뛔", "ㄸ + ㅞ = 뛔 (ttwe)"},
    {"Tml", "씌", "ㅆ + ㅢ = 씌 (ssui)"},
    {"RhK", "꽤", "ㄲ + ㅙ = 꽤 (kkwae)"},
    {"Thl", "쐬", "ㅆ + ㅚ = 쐬 (ssoe)"},
    {"Wnl", "쮜", "ㅉ + ㅟ = 쮜 (jjwi)"},
    {"Ehk", "똬", "ㄸ + ㅘ = 똬 (ttwa)"},
    {"QhK", "뽸", "ㅃ + ㅙ = 뽸 (ppwae)"},
    {"WnO", "쭤", "ㅉ + ㅝ = 쭤 (jjwo)"},

    // === DIPHTHONGS + FINAL CONSONANTS ===
    {"rhls", "괸", "ㄱ + ㅚ + ㄴ = 괸 (goen)"},
    {"gnls", "휜", "ㅎ + ㅟ + ㄴ = 휜 (hwin)"},
    {"rhkr", "곽", "ㄱ + ㅘ + ㄱ = 곽 (gwak)"},
    {"gnOd", "훵", "ㅎ + ㅝ + ㅇ = 훵 (hwon)"},
    {"rnjs", "궨", "ㄱ + ㅞ + ㄴ = 궨 (gwen)"},
    {"rmls", "긘", "ㄱ + ㅢ + ㄴ = 긘 (guin)"},
    {"rhKs", "괜", "ㄱ + ㅙ + ㄴ = 괜 (gwaen)"},
    {"whkr", "좍", "ㅈ + ㅘ + ㄱ = 좍 (jwak)"},
    {"wnOd", "줭", "ㅈ + ㅝ + ㅇ = 줭 (jwong)"},
    {"rnlr", "귁", "ㄱ + ㅟ + ㄱ = 귁 (gwik)"},
    {"chlr", "쵝", "ㅊ + ㅚ + ㄱ = 쵝 (choek)"},

    // Complex diphthong combinations
    {"rhlr", "괵", "ㄱ + ㅚ + ㄱ = 괵 (goek)"},
    {"gnla", "휨", "ㅎ + ㅟ + ㅁ = 휨 (hwim)"},
    {"rhkt", "괏", "ㄱ + ㅘ + ㅅ = 괏 (gwat)"},
    {"rnOe", "궏", "ㄱ + ㅝ + ㄷ = 궏 (gwed)"},
    {"rmlr", "긕", "ㄱ + ㅢ + ㄱ = 긕 (guik)"},
    {"whlf", "죌", "ㅈ + ㅚ + ㄹ = 죌 (joel)"},
    {"rnla", "귐", "ㄱ + ㅟ + ㅁ = 귐 (gwim)"},
    {"rhka", "괌", "ㄱ + ㅘ + ㅁ = 괌 (gwam)"},
    {"rnOa", "궘", "ㄱ + ㅝ + ㅁ = 궘 (gwom)"},
    {"rmla", "긤", "ㄱ + ㅢ + ㅁ = 긤 (guim)"},

    // Double consonant + diphthong + final consonant
    {"Rhlr", "꾁", "ㄲ + ㅚ + ㄱ = 꾁 (kkoek)"},
    {"Qnla", "쀰", "ㅃ + ㅟ + ㅁ = 쀰 (ppwim)"},
    {"Rhkr", "꽉", "ㄲ + ㅘ + ㄱ = 꽉 (kkwak)"},
    {"QnOd", "뿽", "ㅃ + ㅝ + ㅇ = 뿽 (ppwon)"},
    {"Rnje", "꿷", "ㄲ + ㅞ + ㄷ = 꿷 (kkwed)"},
    {"Tmlr", "씍", "ㅆ + ㅢ + ㄱ = 씍 (ssuik)"},
    {"RhKs", "꽨", "ㄲ + ㅙ + ㄴ = 꽨 (kkwaen)"},
    {"Thls", "쐰", "ㅆ + ㅚ + ㄴ = 쐰 (ssoen)"},
    {"Wnla", "쮬", "ㅉ + ㅟ + ㅁ = 쮬 (jjwim)"},
    {"Ehka", "똼", "ㄸ + ㅘ + ㅁ = 똼 (ttwam)"},

    {"", "", ""} // End marker
};

// Jongsung + New Vowel tests (Final consonant splitting)
// When a vowel follows a final consonant, the final moves to the next syllable
// The IME shows only the current syllable being composed
static const test_case_t jongsung_vowel_tests[] = {
    // Simple final consonant + vowel
    {"rkrk", "가", "각 + ㅏ -> 가 (gak -> ga, final ㄱ moves to next)"},
    {"dksk", "나", "안 + ㅏ -> 나 (an -> na, final ㄴ moves to next)"},
    {"dkek", "다", "앋 + ㅏ -> 다 (ad -> da, final ㄷ moves to next)"},
    {"dkfk", "라", "알 + ㅏ -> 라 (al -> la, final ㄹ moves to next)"},
    {"dkak", "마", "암 + ㅏ -> 마 (am -> ma, final ㅁ moves to next)"},
    {"dkqk", "바", "압 + ㅏ -> 바 (ap -> ba, final ㅂ moves to next)"},
    {"dktk", "사", "앗 + ㅏ -> 사 (at -> sa, final ㅅ moves to next)"},
    {"dkdk", "아", "앙 + ㅏ -> 아 (ang -> a, final ㅇ becomes next initial)"},
    {"dkwk", "자", "앚 + ㅏ -> 자 (aj -> ja, final ㅈ moves to next)"},
    {"dkck", "차", "앛 + ㅏ -> 차 (ach -> cha, final ㅊ moves to next)"},
    {"dkzk", "카", "앜 + ㅏ -> 카 (ak -> ka, final ㅋ moves to next)"},
    {"dkxk", "타", "앝 + ㅏ -> 타 (at -> ta, final ㅌ moves to next)"},
    {"dkvk", "파", "앞 + ㅏ -> 파 (ap -> pa, final ㅍ moves to next)"},
    {"dkgk", "하", "앟 + ㅏ -> 하 (ah -> ha, final ㅎ moves to next)"},

    // Simple final + different vowels
    {"rkrO", "거", "각 + ㅓ -> 거 (gak -> geo)"},
    {"dksj", "네", "안 + ㅔ -> 네 (an -> ne)"},
    {"dkfi", "랴", "알 + ㅑ -> 랴 (al -> lya)"},
    {"dkah", "모", "암 + ㅗ -> 모 (am -> mo)"},
    {"dkqm", "브", "압 + ㅡ -> 브 (ap -> beu)"},
    {"dktl", "시", "앗 + ㅣ -> 시 (at -> si)"},

    // Complex final consonant + vowel (ㄳ, ㄵ, ㄶ, ㄺ, ㄻ, ㄼ, ㄽ, ㄾ, ㄿ, ㅀ, ㅄ)
    // When complex final splits: left stays, right moves to next syllable
    {"rkqtk", "사", "값(ㄱㅏㅂㅅ) + ㅏ = 갑사 (gapsa), ㅄ→ㅂ+ㅅ"},
    {"skswk", "자", "낝(ㄴㅏㄴㅈ) + ㅏ = 난자 (nanja), ㄵ→ㄴ+ㅈ"},
    {"sksgk", "하", "낳(ㄴㅏㄴㅎ) + ㅏ = 난하 (nanha), ㄶ→ㄴ+ㅎ"},
    {"qkfrm", "그", "밝(ㅂㅏㄹㄱ) + ㅡ = 발그 (balgeu), ㄺ→ㄹ+ㄱ"},
    {"fkak", "마", "ㄹㅁ + ㅏ -> 마 (lm -> ma, ㅁ moves, ㄹ stays)"},
    {"qkfqk", "바", "밟(ㅂㅏㄹㅂ) + ㅏ = 발바 (balba), ㄼ→ㄹ+ㅂ"},
    {"fktk", "사", "ㄹㅅ + ㅏ -> 사 (ls -> sa, ㅅ moves, ㄹ stays)"},
    {"fkxk", "타", "ㄹㅌ + ㅏ -> 타 (lt -> ta, ㅌ moves, ㄹ stays)"},
    {"fkvk", "파", "ㄹㅍ + ㅏ -> 파 (lp -> pa, ㅍ moves, ㄹ stays)"},
    {"fkgk", "하", "ㄹㅎ + ㅏ -> 하 (lh -> ha, ㅎ moves, ㄹ stays)"},
    {"qktk", "사", "ㅂㅅ + ㅏ -> 사 (bs -> sa, ㅅ moves, ㅂ stays)"},

    // Double consonant final + vowel
    {"dkRk", "까", "앆(ㄲ) + ㅏ -> 까 (akk -> kka, ㄲ stays together)"},
    {"dkTk", "싸", "았(ㅆ) + ㅏ -> 싸 (ass -> ssa, ㅆ stays together)"},

    // Additional complex final splitting examples (user provided)
    {"sjfql", "비", "넓(ㄴㅔㄹㅂ) + ㅣ = 널비 (neolbi), ㄼ→ㄹ+ㅂ"},
    {"EKftk", "사", "땘(ㄸㅐㄹㅅ) + ㅏ = 땔사 (ttaelsa), ㄽ→ㄹ+ㅅ"},
    {"EKfqk", "바", "땗(ㄸㅐㄹㅂ) + ㅏ = 땔바 (ttaelba), ㄼ→ㄹ+ㅂ"},
    {"djfqk", "바", "웳(ㅇㅔㄹㅂ) + ㅏ = 웰바 (welba), ㄼ→ㄹ+ㅂ"},

    {"", "", ""} // End marker
};

// Complex combination tests
// Note: These test the IME's current behavior where it shows only the current jamo
// when starting a new syllable after a completed one
static const test_case_t complex_tests[] = {
    {"gksr", "ᄀ", "ㅎ + ㅏ + ㄴ + ㄱ -> new ㄱ (han + g)"},
    {"rksr", "ᄀ", "ㄱ + ㅏ + ㄴ + ㄱ -> new ㄱ (gan + g)"},
    {"dksr", "ᄀ", "ㅇ + ㅏ + ㄴ + ㄱ -> new ㄱ (an + g)"},
    {"rksd", "ᄋ", "ㄱ + ㅏ + ㄴ + ㅇ -> new ㅇ (gan + ng)"},
    {"gksd", "ᄋ", "ㅎ + ㅏ + ㄴ + ㅇ -> new ㅇ (han + ng)"},
    {"", "", ""} // End marker
};

// Test execution functions
static int run_test_suite(const char* suite_name, const test_case_t* tests, int is_diphthong) {
    (void)is_diphthong;
    printf("\n=== %s ===\n", suite_name);
    printf("Testing %s combinations...\n", suite_name);
    printf("\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    for (int i = 0; tests[i].input[0] != '\0'; i++) {
        char strokes[64] = "";
        char output_buffer[16];
        
        // Reset IME state
        cb_hangul_match_keystrokes(strokes, output_buffer, sizeof(output_buffer), 0, 0);
        
        // Process input character by character
        for (int j = 0; tests[i].input[j] != '\0'; j++) {
            append_stroke(strokes, tests[i].input[j]);
            cb_hangul_match_keystrokes(strokes, output_buffer, sizeof(output_buffer), 0, 0);
        }
        
        total_tests++;
        
        printf("Test %d: %s\n", i + 1, tests[i].description);
        printf("  Input: %s\n", tests[i].input);
        printf("  Expected: %s\n", tests[i].expected);
        printf("  Actual: %s\n", output_buffer);
        print_hex(output_buffer);
        
        // Check if test passed (compare expected vs actual)
        int passed = (strcmp(output_buffer, tests[i].expected) == 0);
        if (passed) {
            printf("  ✅ PASSED\n");
            passed_tests++;
        } else {
            printf("  ❌ FAILED (Expected: %s, Got: %s)\n", tests[i].expected, output_buffer);
        }
        printf("\n");
    }
    
    printf("=== %s Results ===\n", suite_name);
    printf("Total tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success rate: %.1f%%\n", (float)passed_tests / total_tests * 100);
    
    return passed_tests;
}

int main() {
    printf("Korean QWERTY Input Test Suite\n");
    printf("==============================\n");
    printf("Comprehensive testing of Korean character composition\n");
    printf("Covering all combinations of Chosung, Jungsung, and Jongsung\n");
    printf("\n");
    
    int total_passed = 0;
    int total_tests = 0;
    
    // Run all test suites
    total_passed += run_test_suite("Chosung (초성) Tests", chosung_tests, 0);
    total_passed += run_test_suite("Jungsung (중성) Tests", jungsung_tests, 0);
    total_passed += run_test_suite("Jongsung (종성) Tests", jongsung_tests, 0);
    total_passed += run_test_suite("Diphthong (이중모음) Tests", diphthong_tests, 1);
    total_passed += run_test_suite("Complete Syllable Tests", complete_syllable_tests, 0);
    total_passed += run_test_suite("Diphthong Syllable Tests", diphthong_syllable_tests, 1);
    total_passed += run_test_suite("Comprehensive Double Consonant Tests", double_consonant_tests, 0);
    total_passed += run_test_suite("Comprehensive Double Jungsung (Diphthong) Tests", double_jungsung_tests, 1);
    total_passed += run_test_suite("Jongsung + Vowel (Final Consonant Splitting) Tests", jongsung_vowel_tests, 0);
    total_passed += run_test_suite("Complex Combination Tests", complex_tests, 0);

    // Calculate total tests
    for (int i = 0; chosung_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; jungsung_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; jongsung_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; diphthong_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; complete_syllable_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; diphthong_syllable_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; double_consonant_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; double_jungsung_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; jongsung_vowel_tests[i].input[0] != '\0'; i++) total_tests++;
    for (int i = 0; complex_tests[i].input[0] != '\0'; i++) total_tests++;
    
    printf("\n");
    printf("========================================\n");
    printf("FINAL TEST RESULTS\n");
    printf("========================================\n");
    printf("Total test cases: %d\n", total_tests);
    printf("Passed: %d\n", total_passed);
    printf("Failed: %d\n", total_tests - total_passed);
    printf("Overall success rate: %.1f%%\n", (float)total_passed / total_tests * 100);
    printf("\n");
    
    if (total_passed == total_tests) {
        printf("🎉 ALL TESTS PASSED! 🎉\n");
        printf("Korean QWERTY input system is working perfectly!\n");
    } else {
        printf("⚠️  Some tests failed. Please check the implementation.\n");
    }
    
    printf("\n");
    printf("Test Coverage:\n");
    printf("✅ Chosung (초성) - Initial consonants\n");
    printf("✅ Jungsung (중성) - Medial vowels\n");
    printf("✅ Jongsung (종성) - Final consonants\n");
    printf("✅ Diphthongs (이중모음) - Double vowels\n");
    printf("✅ Complete syllables - Chosung + Jungsung + Jongsung\n");
    printf("✅ Comprehensive double consonants - All combinations with vowels and finals\n");
    printf("✅ Comprehensive double jungsung (diphthongs) - All vowel combinations\n");
    printf("✅ Jongsung + Vowel - Final consonant splitting when new syllable starts\n");
    printf("✅ Complex combinations - Multi-character sequences\n");
    printf("\n");
    
    return (total_passed == total_tests) ? 0 : 1;
}
