#pragma once

#include <cstring>
#include <string>

namespace hpp
{
inline int numcmp(const char *a, const char *aend, const char *b, const char *bend)
{
    for (;;)
    {
        if (a == aend)
        {
            if (b == bend)
            {
                return 0;
            }
            return -1;
        }
        if (b == bend)
        {
            return 1;
        }
        if (*a == *b && !isdigit((unsigned char) *a))
        {
            ++a, ++b;
            continue;
        }
        if (!isdigit((unsigned char) *a) || !isdigit((unsigned char) *b))
        {
            return *a - *b;
        }

        // skip leading zeros in both strings
        while (*a == '0' && ++a != aend)
            ;
        while (*b == '0' && ++b != aend)
            ;

        // skip to end of the consecutive digits
        const char *aa = a;
        while (a != aend && isdigit((unsigned char) *a))
        {
            ++a;
        }
        std::ptrdiff_t alen = a - aa;

        const char *bb = b;
        while (b != bend && isdigit((unsigned char) *b))
        {
            ++b;
        }
        std::ptrdiff_t blen = b - bb;

        if (alen != blen)
        {
            return int(alen - blen);
        }

        // same number of consecutive digits in both strings
        while (aa != a)
        {
            if (*aa != *bb)
            {
                return *aa - *bb;
            }
            ++aa, ++bb;
        }
    }
}

inline int numcmp(const std::string& a, const std::string& b)
{
    return numcmp(a.data(), a.data() + a.size(), b.data(), b.data() + b.size());
}

inline int numcmp(const char *a, const char *b)
{
    return numcmp(a, a + strlen(a), b, b + strlen(b));
}

struct string_numeric_order
{
    bool operator()(const std::string& x, const std::string& y) const
    {
        return numcmp(x, y) < 0;
    }

    bool operator()(const char* x, const char* y) const
    {
        return numcmp(x, y) < 0;
    }
};
}
