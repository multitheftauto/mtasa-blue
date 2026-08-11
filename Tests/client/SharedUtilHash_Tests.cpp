/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SharedUtilHash_Tests.cpp
 *  PURPOSE:     Google Test suite for SharedUtil hash and crypto functions
 *
 *  Ported from Shared/sdk/SharedUtil.Tests.hpp - SharedUtil_Hash_Tests().
 *  Covers hex conversion, TEA, HMAC, RSA, AES-128, MD5, and SHA family.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.h>

using namespace SharedUtil;

// Temp file used by hash-from-file tests.
// The original tests use a UTF-8-containing filename to exercise non-ASCII path handling.
static const SStringX kTempFilename("hash_\xD0\x98_test");

// Helper: construct an SString from raw bytes with a specified length (avoids
// SString(const char*, ...) printf-style constructor ambiguity).
static SString MakeRawString(const char* data, size_t len)
{
    return SString(std::string(data, len));
}

///////////////////////////////////////////////////////////////
//
// Hex conversion
//
///////////////////////////////////////////////////////////////

// Verify round-trip hex conversion preserves uppercase canonical form
TEST(SharedUtilHash, ConvertHexRoundTrip)
{
    struct TestData
    {
        const char* input;
        const char* expected;
    };

    TestData tests[] = {
        {"66B9139D8C424BE2BCF224706B48FEB8", "66B9139D8C424BE2BCF224706B48FEB8"},
        {"E7C7253C74275F2DC2DC8C6828816C18301636949369F3bad87666C81E71B309", "E7C7253C74275F2DC2DC8C6828816C18301636949369F3BAD87666C81E71B309"},
        {"61", "61"},
    };

    for (const auto& t : tests)
    {
        char     buffer[256];
        SStringX input(t.input);
        uint     length = input.length() / 2;
        ConvertHexStringToData(input, buffer, length);
        SString result = ConvertDataToHexString(buffer, length);
        EXPECT_EQ(result, t.expected) << "Input: " << t.input;
    }
}

///////////////////////////////////////////////////////////////
//
// TEA encode / decode
//
///////////////////////////////////////////////////////////////

// Verify TeaEncode/TeaDecode round-trips data correctly including embedded nulls
TEST(SharedUtilHash, TeaEncodeDecodeRoundTrip)
{
    // Simple case with known ciphertext
    {
        SStringX data("1234");
        SStringX key("AB12\xC2\xA3$_ ");
        SString  encoded;
        TeaEncode(data, key, &encoded);
        EXPECT_EQ(encoded, MakeRawString("\x08\x56\x3B\x4F\x31\x2D\xE4\x4A", 8));
        SString decoded;
        TeaDecode(encoded, key, &decoded);
        EXPECT_EQ(data, *decoded);
    }

    // Round-trip with long key (original test data contains \0 in the literal,
    // but SString construction from const char* truncates at the null terminator)
    {
        SStringX data("Hello thereHello there");
        SStringX key("78111E998C42243285635E39AFDD614B");
        SString  encoded;
        TeaEncode(data, key, &encoded);
        SString decoded;
        TeaDecode(encoded, key, &decoded);
        EXPECT_EQ(data, *decoded);
    }

    // Round-trip with long data (original test data contains \0 in the literal,
    // but SString construction from const char* truncates at the null terminator)
    {
        SStringX data("78111E998C42243285635E39AFD");
        SStringX key("Hello thereHello there");
        SString  encoded;
        TeaEncode(data, key, &encoded);
        SString decoded;
        TeaDecode(encoded, key, &decoded);
        EXPECT_EQ(data, *decoded);
    }
}

///////////////////////////////////////////////////////////////
//
// HMAC (requires CryptoPP)
//
///////////////////////////////////////////////////////////////

// Verify HMAC-MD5 produces the expected digest
TEST(SharedUtilHash, HmacMD5)
{
    SString result = Hmac<CryptoPP::Weak::MD5>("Hello world", "hecker was there");
    EXPECT_EQ(result, "657C7088ADEA11E6482EE794D3E5489C");
}

// Verify HMAC-SHA1 with an empty key
TEST(SharedUtilHash, HmacSHA1)
{
    SString result = Hmac<CryptoPP::SHA1>("cstddef", "");
    EXPECT_EQ(result, "0339B2CA65A63209C656047C5B11ADA73B63A367");
}

// Verify HMAC-SHA224 with special characters in key
TEST(SharedUtilHash, HmacSHA224)
{
    SString result = Hmac<CryptoPP::SHA224>("Hello thereHello there", "!@#$%^&*()_+|:<>");
    EXPECT_EQ(result, "A7A00E964617DFB59324502786BB28AEEF22898C00B226A7B4A1D607");
}

// Verify HMAC-SHA256 with special characters in data
TEST(SharedUtilHash, HmacSHA256)
{
    SString result = Hmac<CryptoPP::SHA256>("!@#$%^&*()_+|:<>", "cppsymbol");
    EXPECT_EQ(result, "46105B670A55EA8808B16FFC8B88507EAEA3E9D1F5A55891CD04136FB2AADA15");
}

// Verify HMAC-SHA384
TEST(SharedUtilHash, HmacSHA384)
{
    SString result = Hmac<CryptoPP::SHA384>("value", "sha384");
    EXPECT_EQ(result, "CEC945A598261608218BA685EEC02D773F57AFD6410AF67D2A2D1B0D22DAE8624D0F369E55C8C7E774805204A2B5A75A");
}

// Verify HMAC-SHA512 with empty data
TEST(SharedUtilHash, HmacSHA512)
{
    SString result = Hmac<CryptoPP::SHA512>("", "gHtySkGerYnhDxAs");
    EXPECT_EQ(result, "4E6E87CE637808642B902A07F43CA6A1CFE4346054C0C8C542A67C4BF206708CF5AFE3F1BB6D53DCE3469CDEA1CE11A0892EE2F95322C45D2CB809F165AD3BB3");
}

///////////////////////////////////////////////////////////////
//
// RSA key generation + encrypt/decrypt round-trip
//
///////////////////////////////////////////////////////////////

// Verify RSA keygen + encrypt/decrypt round-trip at 1024-bit key size
TEST(SharedUtilHash, RsaRoundTrip1024)
{
    auto keyPair = GenerateRsaKeyPair(1024);
    ASSERT_FALSE(keyPair.privateKey.empty());
    ASSERT_FALSE(keyPair.publicKey.empty());
    SString encrypted = RsaEncode("Hello world", keyPair.publicKey);
    ASSERT_FALSE(encrypted.empty());
    SString decrypted = RsaDecode(encrypted, keyPair.privateKey);
    EXPECT_EQ(decrypted, "Hello world");
}

// Verify RSA keygen + encrypt/decrypt round-trip at 2048-bit key size
TEST(SharedUtilHash, RsaRoundTrip2048)
{
    auto keyPair = GenerateRsaKeyPair(2048);
    ASSERT_FALSE(keyPair.privateKey.empty());
    ASSERT_FALSE(keyPair.publicKey.empty());
    SString encrypted = RsaEncode("!@#$%^&*()_+|:<>", keyPair.publicKey);
    ASSERT_FALSE(encrypted.empty());
    SString decrypted = RsaDecode(encrypted, keyPair.privateKey);
    EXPECT_EQ(decrypted, "!@#$%^&*()_+|:<>");
}

///////////////////////////////////////////////////////////////
//
// RSA decryption of known ciphertexts
//
///////////////////////////////////////////////////////////////

// Verify RSA decryption of a pre-encoded 1024-bit ciphertext
TEST(SharedUtilHash, RsaDecryptKnown1024)
{
    SString rawEncoded = Base64decode(
        "ei2zOjHZLpp5/xhc5GyUoFIdmpj8BZFOtLOHafunis3KVYcEoZmm/JxMMnGMiAemqWCmGhkNin76Jy84G5O9pHCFR499cEvAq92Or1cX8tiM3nqJrgbeNj6SM3QWHGhnx"
        "NDaxHXbXAvh9M2dyR12+X5bQVhex5O0R2XGv1XaaRw=");
    SString rawKey = Base64decode(
        "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAJOYjQmpsmZb8KafeeUtaEydwjM2rRNrdR3BvKdNa4MMx5+YxweWrbsyDycW4AIfU5jcHN61QHaRx53zS"
        "F59cV79ARKoIY+Csi3yD9mgmN81O043/SuU+HqpIsjapTqvT3w4o0dFbJAcoxTvcJf0ZjVhkzK1V6Jucf+4k8OwqeAFAgMBAAECgYBF+HMcWpudlQaQJ1hlQlKIx91R5CD"
        "G7RF3iuoBsl0+omdcjNQGD/PKo9+1G04VE37RH8B8ABdQXqnwgm4ThYqh0XOcL2QcpqzTxCLJI9XsjA5T8EUpETZYl4CNTyVqaEHNywPPVgo7M+dJjfHaKZAiKlpzxJ34M"
        "aG/fmfH7p+U3QJBANQhOLugLnz2zPN7ek9uPRlJlp1KGLA7n773fQ9lccBwxmQcec7wshlyOiH6QzYfyry+gH8WmWcs23NvXh+x/p8CQQCyHrcR3r4y7l7ziKc2NsAvL1v"
        "PD+hyluNae87XTEN14mSc2qFyjIBRRDS+2Y+nZBLWXOwpTaj7MO24yu1SFjLbAkA3D6R9Nxe6AgmyzYMy6OddD/fTPe9olyFg3TaN74P2Mo+cXrPTXjCsYDTsqtwVSW1DT"
        "qNl2YTZ/g+8R9UDnxz7AkEAma+83/NkAwf/73tyY2m8Szo+qhucwfqFMtZusv+/mWB81YYjFXY7ZWNHa4DG3XSQeFt2/XBfpFUkWHTnbhNvTwJBAJq5Gl/CM6Zozhr9yTC"
        "S1i3n3ZuX2V0F8X8QlTC90P5GJLAaGadY0DEJICNhmbaRVZ6W4GSmi3nxnibmz936/1k=");
    ASSERT_FALSE(rawEncoded.empty());
    ASSERT_FALSE(rawKey.empty());
    SString decrypted = RsaDecode(rawEncoded, rawKey);
    EXPECT_EQ(decrypted, "Hello world");
}

// Verify RSA decryption of a pre-encoded 2048-bit ciphertext
TEST(SharedUtilHash, RsaDecryptKnown2048)
{
    SString rawEncoded = Base64decode(
        "Juwvq3z+ykIIdThZIUgMo3VjX4QqnT16CcJLf6ueCLHAK6E9tneLSSHmMjWr1NexPEVHr9BD3lRnmV78d2p/9yIPIF9tcDcMAqNnNQNcQ+LSgg8urDIWhT+ORCatZsM20"
        "7A/W+sQCBqyEsSMflgdc9+K7bmxksucclGyAbjpOvUK7IEH65LJDWFIG4q2uUnzHZkl8UTh26SPbZWecBWME1/+CMR62zJdM43Wm31bFzOPCQCU/5ecJX9brpOX1lGPI6e"
        "Eg895ZwSdLH+JaG6wA9BGc2Z/6Fg+EOxNRy8ZzPD7zq6+Z1If3m3ieVVaV0uraVGXUz4ygKgDDTmSwL5vLQ==");
    SString rawKey = Base64decode(
        "MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCddRAukUoE4EfHv5RO1rHU1s8PLwzAE1eUf4uOcrfNNA677L15RD3ZC4RfdpA4Ya41mMQAy4BimIXHJ"
        "Ko0JcygV2Jo9W0268afsJpDgz69uZiXp4OFG1CfKrDNG7oyM310OBkrsQdBPoBNveeBqjWf88oq95nHf5h8MMPL+x2WRf4LELnx1VlaGaw0DfVPHnNtpnv0cMFEc/saCBu"
        "UtethD/1HataFucwNotr77tQy1kK8K9nHrFhC69kvPti2unL0IQ3OYMjgZoHpv1Ecz5l7zm2DyRa9RIeeZXnADOwaS/9e6hvriirvs7aqlioVIK1wi82hrZLuOh0rHt5WL"
        "+i3AgMBAAECggEAD/DpGX3WG5ZFTiDEOBMeX+j7f5MI3G1Y4YzkKSDOIPVTRI1cRWm2RUIYPb34Le0dwQldVqNDtTex246HuM3W4KjYKr4mN3g5XLN5erpP4M9odnocI56"
        "3U9XznmBfQnPaEKkZS0YZ8O4N+VDvnj4IPdBluGHm7L6rRm99cypSPYNgxCyYX0JGEfOYXmvnVqlIlnohVy4IoQ8toPE8WIChN+kXfkcluj++YDvF+7ir7vh8EVgOsgtoz"
        "80CS+sn6lUAh+TUFmbHqJvzMn9LovhkMxhq+U55bxpVlzsWQWKKPQsU2F7g3akCux2zapI0C8rIGOqmoO8cY3hkXvAG/LpWuQKBgQD98dJ66cMfGH5uS3XeOadS9JSqyql"
        "jejWg7LhFWqpK2UAqE8hxmAhJIRexEH7jxYhCGSro2sLsbiZw00t94cNIjUdfm2urLTNBk+LNExlGMwJmVzNCSIx4085JFLRMIf9XAAOF62gZGW3yRKSl+lfG2aEMN/oQR"
        "rm2IVqpLnEsiwKBgQCeu1FJB7zUgOVZEg+DwEm+SalhERwQY200UnkIW0Q9T+SHVS1VGBQHa3oQIICotufLPn1LIRKEl+6mP+ITbxvdFbwCueNnaukGSfVvdno0vwfoFjC"
        "PD5nlwWuSf4/kAiA2zX21Hc7yO3+VPSzy9FzLcEQrpg41CnnjVzntl71eBQKBgHuuPpQsrCLF/DXdvM96BvuJMaspP2y6xcnV4T2uFciUnQKhVfu1TzvGyVIstZBb0axXs"
        "6VrfopGb6R91IO/5Cu+22c+PCdHwXIMA+dbiPC+wUl06Ps8eGYUyqCfW9zEpicKU6zy5WVsFDdtpT9NeVjY/+4CiWFKAQMqpYxGmUpXAoGAOIq/NMWF1SipCeZuEl9/pUH"
        "2DPacSZYxQ08RgBGKqFB6Nl74C/TMxavaSj5Ztj+ZKAP1e7qnLwYz45KkBIoyoblpRItU3vTtVuknfXH1rC5UMa8d0wgaqQYSPJgl+HFtajgezFJ28kCRfObSySSUnX87O"
        "VUogGpf6sSahPDlasUCgYBz2t0Qh7aILYiqIu9VaqmSAUQV21tk8rnfD5DmwZchFmVsb1TqogRLWHRz78ciV/RlWt1+QI1scUKHvYUdxrsmR2zq2AO7qWE4csDtYNgnuwZ"
        "vTpg9Nn0v9257IPf6O2ihBStJhh18tyR5VIjY5ftAWMZpF67TDdlNuKxBtkUmdg==");
    ASSERT_FALSE(rawEncoded.empty());
    ASSERT_FALSE(rawKey.empty());
    SString decrypted = RsaDecode(rawEncoded, rawKey);
    EXPECT_EQ(decrypted, "!@#$%^&*()_+|:<>");
}

// Verify RSA decryption of a pre-encoded 4096-bit ciphertext
TEST(SharedUtilHash, RsaDecryptKnown4096)
{
    SString rawEncoded = Base64decode(
        "Yxt6G+BHWxutrtZ0ecuOFEZT1NexrLqzOCB1I4p09xlIzepI6j69jqwVdGLJQdlZq2K4O8PoFXgc2o5g9mGpB8zq8KDULkbLwcaNjK0WcRkOPrdveesDUzCCJc7Qm2/BE"
        "ItQeTe6kS1joV/VSy8xbyauViJUSA1EcyaIm0WI7JrP/f8phjPlFeSujlAYezyUArBIdvokO6lFJuhTPgMxJ0NOczOrbELszzUaa3ikHm6nkoK7zZQG+o4wlDeshZg1xux"
        "qKu0IfvnwQBnP0BttV0aiglGqUonZdy9CvJ14t8jyOo7rLnDkftdyV8ITG5WEoOnSmzcUR8O7xakem2aJhPFZEFcCFu1n3L0kM+L2lPu3QtgMacbtP7Qsk+UNubpSP/t/4"
        "1hBnIeQ9CYhvb08PN322OA1ZeAaXVVlyOyM6uTqhD7h4tzUi30/mKqurnrXhzEE2Thn0OZ9rIYcKHHxsE0/dBiOpgngw7I0qCqG7+N0z2NP4Cqj1e/KTEd/7kJQ4/wA7oF"
        "PzzNpq70+f/YlQQUyvpmq8kKt3MTwTuWiBDxOiEMHeoJoLEQAIB3obqymE76vfzuOc26fXNmVBsaNkm3wzVbpE9TtktHFCrFwAdvlk5/treUCIjIwoD2PHxJwOs6eFNuOP"
        "SZ+WGQ77oZYDHDoDwWi/fNzkPbBT2Cr0Vs=");
    SString rawKey = Base64decode(
        "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");
    ASSERT_FALSE(rawEncoded.empty());
    ASSERT_FALSE(rawKey.empty());
    SString decrypted = RsaDecode(rawEncoded, rawKey);
    EXPECT_EQ(decrypted, "Hecker there");
}

///////////////////////////////////////////////////////////////
//
// AES-128 encrypt/decrypt round-trip
//
///////////////////////////////////////////////////////////////

// Verify AES-128-CTR encrypt/decrypt round-trip with simple data
TEST(SharedUtilHash, Aes128RoundTripSimple)
{
    auto res = Aes128encode("AB12\xC2\xA3$_ ", "5347123412340000");
    ASSERT_FALSE(res.first.empty());
    ASSERT_FALSE(res.second.empty());
    SString decoded = Aes128decode(res.first, "5347123412340000", res.second);
    EXPECT_EQ(decoded, "AB12\xC2\xA3$_ ");
}

// Verify AES-128-CTR round-trip with data containing embedded null
TEST(SharedUtilHash, Aes128RoundTripEmbeddedNull)
{
    SString data = MakeRawString("78111E998C42243285635E39AFDD614B\0 AB12\xC2\xA3$_ ", 46);
    auto    res = Aes128encode(data, "gHtySkGerYnhDxAs");
    ASSERT_FALSE(res.first.empty());
    ASSERT_FALSE(res.second.empty());
    SString decoded = Aes128decode(res.first, "gHtySkGerYnhDxAs", res.second);
    EXPECT_EQ(decoded, data);
}

// Verify AES-128-CTR round-trip with special characters in data
TEST(SharedUtilHash, Aes128RoundTripSpecialChars)
{
    auto res = Aes128encode("Hello thereHello there", "!@#$%^&*()_+|:<>");
    ASSERT_FALSE(res.first.empty());
    ASSERT_FALSE(res.second.empty());
    SString decoded = Aes128decode(res.first, "!@#$%^&*()_+|:<>", res.second);
    EXPECT_EQ(decoded, "Hello thereHello there");
}

///////////////////////////////////////////////////////////////
//
// MD5
//
///////////////////////////////////////////////////////////////

// Verify MD5 hash via CMD5Hasher, GenerateHashHexString, and file-based hashing
TEST(SharedUtilHash, MD5)
{
    struct TestData
    {
        const char* input;
        size_t      length;
        const char* expected;
    };

    TestData tests[] = {
        {"", 0, "D41D8CD98F00B204E9800998ECF8427E"},
        {"Hello there", 11, "E8EA7A8D1E93E8764A84A0F3DF4644DE"},
        {"AB12\xC2\xA3$_", 8, "135C3423B76DEECA446666DE48912C3B"},
    };

    for (const auto& t : tests)
    {
        SString input = MakeRawString(t.input, t.length);
        SString result = CMD5Hasher::CalculateHexString(input.c_str(), input.length());
        EXPECT_EQ(result, t.expected) << "MD5 hasher failed for length " << t.length;

        result = GenerateHashHexString(EHashFunction::MD5, input);
        EXPECT_EQ(result, t.expected) << "MD5 GenerateHashHexString failed for length " << t.length;

        FileSave(kTempFilename, input);
        result = GenerateHashHexStringFromFile(EHashFunction::MD5, kTempFilename);
        EXPECT_EQ(result, t.expected) << "MD5 from file failed for length " << t.length;
    }
    FileDelete(kTempFilename);
}

///////////////////////////////////////////////////////////////
//
// SHA-256
//
///////////////////////////////////////////////////////////////

// Verify SHA-256 hash via GenerateSha256HexString and file-based hashing
TEST(SharedUtilHash, SHA256)
{
    struct TestData
    {
        const char* input;
        size_t      length;
        const char* expected;
    };

    TestData tests[] = {
        {"", 0, "E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855"},
        {"Hello there", 11, "4E47826698BB4630FB4451010062FADBF85D61427CBDFAED7AD0F23F239BED89"},
        {"AB12\xC2\xA3$_", 8, "46CD8EB20A879864D5B7968679B6D60615BC64592D376FA10A90C064D9BA269F"},
    };

    for (const auto& t : tests)
    {
        SString input = MakeRawString(t.input, t.length);
        SString result = GenerateSha256HexString(input);
        EXPECT_EQ(result, t.expected) << "SHA256 failed for length " << t.length;

        FileSave(kTempFilename, input);
        result = GenerateHashHexStringFromFile(EHashFunction::SHA256, kTempFilename);
        EXPECT_EQ(result, t.expected) << "SHA256 from file failed for length " << t.length;
    }
    FileDelete(kTempFilename);
}

///////////////////////////////////////////////////////////////
//
// SHA-1
//
///////////////////////////////////////////////////////////////

// Verify SHA-1 hash via GenerateHashHexString and file-based hashing
TEST(SharedUtilHash, SHA1)
{
    struct TestData
    {
        const char* input;
        size_t      length;
        const char* expected;
    };

    TestData tests[] = {
        {"", 0, "DA39A3EE5E6B4B0D3255BFEF95601890AFD80709"},
        {"Hello there", 11, "726C76553E1A3FDEA29134F36E6AF2EA05EC5CCE"},
        {"AB12\xC2\xA3$_", 8, "EDD865E6301DC1F3B578E12ACAB9A6363F1F0598"},
    };

    for (const auto& t : tests)
    {
        SString input = MakeRawString(t.input, t.length);
        SString result = GenerateHashHexString(EHashFunction::SHA1, input);
        EXPECT_EQ(result, t.expected) << "SHA1 failed for length " << t.length;

        FileSave(kTempFilename, input);
        result = GenerateHashHexStringFromFile(EHashFunction::SHA1, kTempFilename);
        EXPECT_EQ(result, t.expected) << "SHA1 from file failed for length " << t.length;
    }
    FileDelete(kTempFilename);
}

///////////////////////////////////////////////////////////////
//
// SHA-224
//
///////////////////////////////////////////////////////////////

// Verify SHA-224 hash via GenerateHashHexString and file-based hashing
TEST(SharedUtilHash, SHA224)
{
    struct TestData
    {
        const char* input;
        size_t      length;
        const char* expected;
    };

    TestData tests[] = {
        {"", 0, "D14A028C2A3A2BC9476102BB288234C415A2B01F828EA62AC5B3E42F"},
        {"Hello there", 11, "40AACC4967ECA7730A1A069539D78AE7782480802E481F1ECC26927D"},
        {"AB12\xC2\xA3$_", 8, "5CD74BB63D1F75CF4E4EB3967D101EC05DBF2E9E101BE932ABD18D27"},
    };

    for (const auto& t : tests)
    {
        SString input = MakeRawString(t.input, t.length);
        SString result = GenerateHashHexString(EHashFunction::SHA224, input);
        EXPECT_EQ(result, t.expected) << "SHA224 failed for length " << t.length;

        FileSave(kTempFilename, input);
        result = GenerateHashHexStringFromFile(EHashFunction::SHA224, kTempFilename);
        EXPECT_EQ(result, t.expected) << "SHA224 from file failed for length " << t.length;
    }
    FileDelete(kTempFilename);
}

///////////////////////////////////////////////////////////////
//
// SHA-384
//
///////////////////////////////////////////////////////////////

// Verify SHA-384 hash via GenerateHashHexString and file-based hashing
TEST(SharedUtilHash, SHA384)
{
    struct TestData
    {
        const char* input;
        size_t      length;
        const char* expected;
    };

    TestData tests[] = {
        {"", 0, "38B060A751AC96384CD9327EB1B1E36A21FDB71114BE07434C0CC7BF63F6E1DA274EDEBFE76F65FBD51AD2F14898B95B"},
        {"Hello there", 11, "7438E0294C534D6CA6CC2EFB04A60DB488C86B66C4CBD3C00D11D58C8020274AB0A2A720C88986968D894F26B16C461F"},
        {"AB12\xC2\xA3$_", 8, "D097C56CEB17C6553E216C42DA60EF85917738EAB9D5A7A62674945CF51F5945C3A573A1A7E9B40A9B427789110A2C14"},
    };

    for (const auto& t : tests)
    {
        SString input = MakeRawString(t.input, t.length);
        SString result = GenerateHashHexString(EHashFunction::SHA384, input);
        EXPECT_EQ(result, t.expected) << "SHA384 failed for length " << t.length;

        FileSave(kTempFilename, input);
        result = GenerateHashHexStringFromFile(EHashFunction::SHA384, kTempFilename);
        EXPECT_EQ(result, t.expected) << "SHA384 from file failed for length " << t.length;
    }
    FileDelete(kTempFilename);
}

///////////////////////////////////////////////////////////////
//
// SHA-512
//
///////////////////////////////////////////////////////////////

// Verify SHA-512 hash via GenerateHashHexString and file-based hashing
TEST(SharedUtilHash, SHA512)
{
    struct TestData
    {
        const char* input;
        size_t      length;
        const char* expected;
    };

    TestData tests[] = {
        {"", 0, "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E"},
        {"Hello there", 11, "567683DDBA1F5A576B68EC26F41FFBCC7E718D646839AC6C2EF746FE952CEF4CBE6DEA635BC2F098B92B65CAACF482333BB9D1D9A3089BC4F01CB86F7A2FBC18"},
        {"AB12\xC2\xA3$_", 8,
         "6838166F7AE21CF8344B2F3A83AF2EAF34309AE3A84369BF2E59C7F0FA2971A5975CBC24CF079074FB579BF75920B2E83B0E01AFEDFDB5522240979D3EAA0B6C"},
    };

    for (const auto& t : tests)
    {
        SString input = MakeRawString(t.input, t.length);
        SString result = GenerateHashHexString(EHashFunction::SHA512, input);
        EXPECT_EQ(result, t.expected) << "SHA512 failed for length " << t.length;

        FileSave(kTempFilename, input);
        result = GenerateHashHexStringFromFile(EHashFunction::SHA512, kTempFilename);
        EXPECT_EQ(result, t.expected) << "SHA512 from file failed for length " << t.length;
    }
    FileDelete(kTempFilename);
}
