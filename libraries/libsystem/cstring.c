/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/MinMax.h>
#include <libsystem/__printf__.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/runtime.h>

/* --- snprintf ------------------------------------------------------------- */

void string_printf_append(printf_info_t *info, char c)
{
    if (info->allocated == -1)
    {
        strapd(info->output, c);
    }
    else
    {
        strnapd((char *)info->output, c, info->allocated);
    }
}

int snprintf(char *s, int n, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = vsnprintf(s, n, fmt, va);

    va_end(va);

    return result;
}

int vsnprintf(char *s, int n, const char *fmt, va_list va)
{

    if (n == 0)
        return 0;

    printf_info_t info = (printf_info_t){
        .format = fmt,
        .append = string_printf_append,
        .output = (char *)s,
        .allocated = n,
    };

    // We need it to start with a 0 because we use strapd.
    s[0] = '\0';
    return __printf(&info, va);
}

// mem* functions ----------------------------------------------------------- //

void *memchr(const void *str, int c, size_t n)
{
    unsigned char *s = (unsigned char *)str;

    for (size_t i = 0; i < n; i++)
    {
        if (*(s + i) == c)
        {
            return (s + i);
        }
    }

    return NULL;
}

int memcmp(const void *str1, const void *str2, size_t n)
{
    const unsigned char *s1 = str1;
    const unsigned char *s2 = str2;

    for (size_t i = 0; i < n; i++)
    {
        if (*(s1 + i) != *(s2 + i))
        {
            return *(s1 + i) - *(s2 + i);
        }
    }

    return 0;
}

void *memmove(void *dest, const void *src, size_t n)
{
    size_t i;
    const unsigned char *usrc = src;
    unsigned char *udest = dest;

    if (udest < usrc)
    {
        for (i = 0; i < n; i++)
            udest[i] = usrc[i];
    }
    else if (udest > usrc)
    {
        for (i = n; i > 0; i--)
            udest[i - 1] = usrc[i - 1];
    }

    return dest;
}

void *memcpy(void *s1, const void *s2, size_t n)
{
    char *cdest;
    char *csrc;
    unsigned int *ldest = (unsigned int *)s1;
    unsigned int *lsrc = (unsigned int *)s2;

    while (n >= sizeof(unsigned int))
    {
        *ldest++ = *lsrc++;
        n -= sizeof(unsigned int);
    }

    cdest = (char *)ldest;
    csrc = (char *)lsrc;

    while (n > 0)
    {
        *cdest++ = *csrc++;
        n -= 1;
    }

    return s1;
}

void *memset(void *str, int c, size_t n)
{
    char *s = str;

    for (size_t i = 0; i < n; i++)
    {
        *(s + i) = c;
    }

    return str;
}

void *memshift(char *mem, int shift, size_t n)
{
    void *dest = mem + shift;
    memmove(dest, mem, n);
    return dest;
}

char *strcat(char *dest, const char *src)
{
    unsigned int dest_size = strlen(dest);
    unsigned int src_size = strlen(src);

    char *d = dest;

    for (size_t i = 0; i < src_size; i++)
    {
        d[dest_size + i] = src[i];
    }

    d[dest_size + src_size] = '\0';

    return dest;
}

char *strncat(char *s1, const char *s2, size_t n)
{
    char *s = s1;
    /* Find the end of S1.  */
    s1 += strlen(s1);
    size_t ss = strnlen(s2, n);
    s1[ss] = '\0';
    memcpy(s1, s2, ss);
    return s;
}

size_t strlcat(char *dst, const char *src, size_t maxlen)
{
    const size_t srclen = strlen(src);
    const size_t dstlen = strnlen(dst, maxlen);
    if (dstlen == maxlen)
        return maxlen + srclen;
    if (srclen < maxlen - dstlen)
    {
        memcpy(dst + dstlen, src, srclen + 1);
    }
    else
    {
        memcpy(dst + dstlen, src, maxlen - 1);
        dst[dstlen + maxlen - 1] = '\0';
    }
    return dstlen + srclen;
}

char *strchr(const char *p, int ch)
{
    char c;

    c = ch;
    for (;; ++p)
    {
        if (*p == c)
            return ((char *)p);
        if (*p == '\0')
            return (NULL);
    }
}

int strcmp(const char *stra, const char *strb)
{
    unsigned int i;

    for (i = 0; stra[i] == strb[i]; i++)
    {
        if (stra[i] == '\0')
            return 0;
    }

    return stra[i] - strb[i];
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    register unsigned char u1, u2;

    while (n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2)
            return u1 - u2;

        if (u1 == '\0')
            return 0;
    }
    return 0;
}

int strcoll(const char *s1, const char *s2)
{
    char t1[1 + strxfrm(0, s1, 0)];
    strxfrm(t1, s1, sizeof(t1));
    char t2[1 + strxfrm(0, s2, 0)];
    strxfrm(t2, s2, sizeof(t2));
    return strcmp(t1, t2);
}

char *strcpy(char *s1, const char *s2)
{
    char *s = s1;
    while ((*s++ = *s2++) != 0)
        ;
    return (s1);
}

char *strncpy(char *s1, const char *s2, size_t n)
{
    size_t size = strnlen(s2, n);
    if (size != n)
        memset(s1 + size, '\0', n - size);
    return memcpy(s1, s2, size);
}

size_t strlcpy(char *dst, const char *src, size_t maxlen)
{
    const size_t srclen = strlen(src);

    if (srclen + 1 < maxlen)
    {
        memcpy(dst, src, srclen + 1);
    }
    else if (maxlen != 0)
    {
        memcpy(dst, src, maxlen - 1);
        dst[maxlen - 1] = '\0';
    }

    return srclen;
}

size_t strcspn(const char *string, const char *chars)
{
    register char c, *p, *s;

    for (s = (char *)string, c = *s; c != 0; s++, c = *s)
    {
        for (p = (char *)chars; *p != 0; p++)
        {
            if (c == *p)
            {
                return s - string;
            }
        }
    }
    return s - string;
}

char *strerror(int errnum)
{
    (void)(errnum);
    return "Error";
}

size_t strlen(const char *str)
{
    size_t lenght = 0;
    while (str[lenght])
        lenght++;
    return lenght;
}

size_t strnlen(const char *s, size_t maxlen)
{
    size_t len;

    for (len = 0; len < maxlen; len++, s++)
    {
        if (!*s)
            break;
    }
    return (len);
}

char *strpbrk(const char *s, const char *accept)
{
    s += strcspn(s, accept);
    return *s ? (char *)s : NULL;
}

char *strrchr(const char *s, int c)
{

    char *rtnval = 0;

    do
    {
        if (*s == c)
            rtnval = (char *)s;
    } while (*s++);
    return (rtnval);
}

size_t strspn(const char *p, const char *s)
{
    int i, j;

    for (i = 0; p[i]; i++)
    {
        for (j = 0; s[j]; j++)
        {
            if (s[j] == p[i])
                break;
        }
        if (!s[j])
            break;
    }
    return (i);
}

char *strstr(const char *s1, const char *s2)
{
    const char *p = s1;
    const size_t len = strlen(s2);

    for (; (p = strchr(p, *s2)) != 0; p++)
    {
        if (strncmp(p, s2, len) == 0)
            return (char *)p;
    }
    return NULL;
}

char *strtok(char *s, char **state, const char *delim)
{
    register int ch;

    if (s == 0)
        s = *state;
    do
    {
        if ((ch = *s++) == '\0')
            return 0;
    } while (strchr(delim, ch));
    --s;
    *state = s + strcspn(s, delim);
    if (**state != 0)
        **state++ = 0;
    return s;
}

size_t strxfrm(char *dest, const char *src, size_t n)
{
    size_t len;

    len = strlen(src);
    (void)memcpy((void *)dest, (void *)src, MIN(n, len + 1));

    return len;
}

void strapd(char *str, char c)
{
    unsigned int len = strlen(str);
    str[len] = c;
    str[len + 1] = '\0';
}

void strbs(char *str)
{
    unsigned int len = strlen(str);
    str[len - 1] = '\0';
}

void strnapd(char *str, char c, size_t n)
{
    for (uint i = 0; i < (n - 1); i++)
    {
        if (str[i] == '\0')
        {
            str[i] = c;
            str[i + 1] = '\0';
            return;
        }
    }
}

void strrvs(char *str)
{
    int c, i, j;
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
    {
        c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
}

void strleadtrim(char *str, char c)
{
    char *start = str;

    while (*start == c)
        start++;

    memmove(str, start, strlen(str + 1));
}

void strtrailtrim(char *str, char c)
{
    char *end = str + strlen(str) - 1;

    while (end > str && *end == c)
        end--;

    end[1] = '\0';
}

char *strdup(const char *s)
{
    int lenght = strlen(s) + 1;
    char *allocated_string = malloc(lenght);

    if (allocated_string != NULL)
    {
        memcpy(allocated_string, s, lenght);
    }

    return allocated_string;
}