#include "MD5.h"

// Precomputed Table
const unsigned int MD5::K[64]
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

// Shifts per round
const unsigned int MD5::s[64]
{
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

unsigned int MD5::LeftRotate(unsigned int value, const unsigned int shiftAmount)
{
    return (value << shiftAmount) | (value >> (32 - shiftAmount));
}

unsigned int MD5::LittleEndianToBigEndian(unsigned int value)
{
    unsigned int result = 0;
    unsigned char* src = (unsigned char*)&value;
    unsigned char* dst = (unsigned char*)&result + sizeof(result) - sizeof(value);

    for (int i = 0; i < sizeof(value); i++)
        dst[i] = src[sizeof(value) - i - 1];

    return result;
}

vector<unsigned char> MD5::AsciiToBytes(const string& str)
{
    vector<unsigned char> result;

    for (auto& e : str)
    {
        unsigned char b = e;
        result.push_back(b);
    }

    return result;
}

string MD5::AsciiToBinary(const string& str)
{
    string result;
    for (char c : str)
        result += bitset<8>(c).to_string();

    return result;
}

string MD5::HashToString(const vector<unsigned int>& hash)
{
    stringstream ss;

    for (int i = 0; i < hash.size(); i++)
        ss << hex << std::setw(8) << std::setfill('0') << hash[i];

    return ss.str();
}

string MD5::Hash(string str)
{
    // Initialize messageblocks from string
    vector<vector<unsigned int>> messageBlocks;
    InitMessageBlocks(str, messageBlocks);

    // Temporary hash variables
    unsigned int a0 = kA0;
    unsigned int b0 = kB0;
    unsigned int c0 = kC0;
    unsigned int d0 = kD0;

    // For each message block (512-bits), do...
    for (int blockIndex = 0; blockIndex < messageBlocks.size(); blockIndex++)
    {
        vector<unsigned int>& M = messageBlocks[blockIndex];

        // Hash value for current chunk
        unsigned int A = a0;
        unsigned int B = b0;
        unsigned int C = c0;
        unsigned int D = d0;

        // Iteration
        for (int g = 0; g < iteration; g++)
        {
            // Round algorithm
            for (int i = 0; i < 64; i++)
            {
                unsigned int F = 0, g = 0;

                if (i < 16)
                {
                    F = (B & C) | (~B & D);
                    g = i;
                }
                else if (i < 32)
                {
                    F = (D & B) | (~D & C);
                    g = (5 * i + 1) % 16;
                }
                else if (i < 48)
                {
                    F = B ^ C ^ D;
                    g = (3 * i + 5) % 16;
                }
                else
                {
                    F = C ^ (B | ~D);
                    g = (7 * i) % 16;
                }

                // Swap and shift
                F += A + K[i] + M[g];
                A = D;
                D = C;
                C = B;
                B += LeftRotate(F, s[i]);
            }
        }

        // Add current hash values
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    // Convert little endian to big endian
    a0 = LittleEndianToBigEndian(a0);
    b0 = LittleEndianToBigEndian(b0);
    c0 = LittleEndianToBigEndian(c0);
    d0 = LittleEndianToBigEndian(d0);

    // Return string converted hash
    return HashToString({ a0,b0,c0,d0 });
}

void MD5::InitMessageBlocks(string str, vector<vector<unsigned int>>& messageBlocks)
{
    // Convert string to bytes
    vector<unsigned char> bytes = AsciiToBytes(str);

    // Get string length bit size
    unsigned long long originalLength = AsciiToBinary(str).size();

    // Divide by 4 bytes
    size_t wordCount = str.size() / 4;
    int remainder = str.size() % 4;

    // Initialize temporary vector
    vector<unsigned int> M(wordCount, 0);

    // Copy divisible 4 bytes to vector
    memcpy(M.data(), bytes.data(), bytes.size()-remainder);

    // Append 0x80 or 0b10000000
    M.push_back(0x80);

    // If there are remaining bytes...
    if (remainder != 0)
    {
        M[M.size() - 1] <<= 8;

        // Fill last remaining bytes
        for (size_t i = bytes.size() - 1; i >= bytes.size() - remainder; i--)
        {
            M[M.size() - 1] |= bytes[i];

            if (i <= bytes.size() - remainder)
                break;

            M[M.size() - 1] <<= 8;
        }
    }

    // Pad 0 bytes
    int padding = 16 - M.size() % 16;
    for (int i = 0; i < padding; i++)
        M.push_back(0x0);

    // Append message length in bits to last 8 bytes
    *(unsigned long long*)(M.data() + M.size() - 2) = originalLength;

    // Split bytes into 16 unsigned integers and add to messageBlocks
    size_t totalBlocks = M.size() / 16;
    for (int i = 0; i < totalBlocks; i++)
        messageBlocks.push_back({ M.begin() + i * 16, M.begin() + (i + 1) * 16 });
}

string MD5::GenerateRandomSalt()
{
    const static string alphanumeric = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    string result = "";
    for (int i = 0; i < saltSize; i++)
    {
        unsigned char byte = alphanumeric[rand() % alphanumeric.size()];
        result += byte;
    }

    return result;
}

void MD5::SetIteration(int n)
{
    iteration = n;
}

void MD5::SetSaltSize(int n)
{
    saltSize = n;
}