#pragma once

#define NICKGEN_NUM_ADJECTIVES      1048
#define NICKGEN_NUM_NOUNS           934

class CNickGenerator
{
public:
    static const char* const m_szAdjectives[NICKGEN_NUM_ADJECTIVES];
    static const char* const m_szNouns[NICKGEN_NUM_NOUNS];
    static const char* GetRandomNickname();
};