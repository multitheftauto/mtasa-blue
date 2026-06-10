/*****************************************************************************
 * pcrecpp_compat.h - Compatibility wrapper for PCRE2 C++ API
 *
 * This provides the same interface as the old pcrecpp (PCRE1 C++ wrapper)
 * but is backed by the PCRE2 C API. It supports the subset of pcrecpp
 * features used by MTA:
 *   - RE_Options (caseless, multiline, dotall, extended, utf8)
 *   - RE (PartialMatch, GlobalReplace, FindAndConsume)
 *   - StringPiece (simple string wrapper)
 *
 * Original pcrecpp was dropped in PCRE2, so this wrapper is needed for
 * backwards compatibility during the migration from PCRE1 to PCRE2.
 *****************************************************************************/
#pragma once

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <string>

namespace pcrecpp
{
    //////////////////////////////////////////////////////////////////////////
    // RE_Options - mirrors old pcrecpp::RE_Options
    //////////////////////////////////////////////////////////////////////////
    class RE_Options
    {
    public:
        RE_Options() : m_uiFlags(0) {}

        void set_caseless(bool b)
        {
            if (b)
                m_uiFlags |= PCRE2_CASELESS;
            else
                m_uiFlags &= ~PCRE2_CASELESS;
        }

        void set_multiline(bool b)
        {
            if (b)
                m_uiFlags |= PCRE2_MULTILINE;
            else
                m_uiFlags &= ~PCRE2_MULTILINE;
        }

        void set_dotall(bool b)
        {
            if (b)
                m_uiFlags |= PCRE2_DOTALL;
            else
                m_uiFlags &= ~PCRE2_DOTALL;
        }

        void set_extended(bool b)
        {
            if (b)
                m_uiFlags |= PCRE2_EXTENDED;
            else
                m_uiFlags &= ~PCRE2_EXTENDED;
        }

        void set_utf8(bool b)
        {
            if (b)
            {
                m_uiFlags |= PCRE2_UTF;
                // When UTF is enabled, also enable UCP for consistent
                // Unicode character class behavior (\w, \d, etc.)
                m_uiFlags |= PCRE2_UCP;
            }
            else
            {
                m_uiFlags &= ~PCRE2_UTF;
                m_uiFlags &= ~PCRE2_UCP;
            }
        }

        // Internal: get the raw PCRE2 option bits
        uint32_t options() const { return m_uiFlags; }

    private:
        uint32_t m_uiFlags;
    };

    //////////////////////////////////////////////////////////////////////////
    // StringPiece - simple string wrapper used by FindAndConsume
    //////////////////////////////////////////////////////////////////////////
    class StringPiece
    {
    public:
        StringPiece() : m_pData(nullptr), m_uiLength(0) {}

        explicit StringPiece(const std::string& str)
            : m_pData(str.data()), m_uiLength(str.size())
        {
        }

        const char* data() const { return m_pData; }
        size_t      size() const { return m_uiLength; }
        bool        empty() const { return m_uiLength == 0; }

        // Advance past 'n' characters (consuming them)
        void remove_prefix(size_t n)
        {
            if (n > m_uiLength)
                n = m_uiLength;
            m_pData += n;
            m_uiLength -= n;
        }

    private:
        const char* m_pData;
        size_t      m_uiLength;
    };

    //////////////////////////////////////////////////////////////////////////
    // RE - regular expression engine (mirrors old pcrecpp::RE)
    //////////////////////////////////////////////////////////////////////////
    class RE
    {
    public:
        // Construct and compile a pattern with the given options.
        // On compilation failure, the pattern is set to NULL and all
        // match operations will return false.
        RE(const std::string& pattern, const RE_Options& options)
            : m_pCode(nullptr)
        {
            int         errorcode;
            PCRE2_SIZE  erroroffset;

            m_pCode = pcre2_compile(
                reinterpret_cast<PCRE2_SPTR>(pattern.c_str()),
                PCRE2_ZERO_TERMINATED,
                options.options(),
                &errorcode,
                &erroroffset,
                nullptr);

            if (m_pCode == nullptr)
            {
                // Compilation failed — m_pCode stays NULL.
                // Optionally, could log the error here, but matching
                // old pcrecpp behavior which silently fails.
                PCRE2_UCHAR buffer[256];
                pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
            }
        }

        ~RE()
        {
            if (m_pCode)
                pcre2_code_free(m_pCode);
        }

        // Non-copyable, non-movable (simplifies ownership)
        RE(const RE&) = delete;
        RE& operator=(const RE&) = delete;

        ////////////////////////////////////////////////////////////////////
        // PartialMatch - returns true if the pattern matches anywhere in str
        ////////////////////////////////////////////////////////////////////
        bool PartialMatch(const std::string& str) const
        {
            if (!m_pCode)
                return false;

            pcre2_match_data* matchData = pcre2_match_data_create_from_pattern(m_pCode, nullptr);
            if (!matchData)
                return false;

            int rc = pcre2_match(
                m_pCode,
                reinterpret_cast<PCRE2_SPTR>(str.c_str()),
                str.size(),
                0,
                0,
                matchData,
                nullptr);

            pcre2_match_data_free(matchData);
            return (rc >= 0);
        }

        ////////////////////////////////////////////////////////////////////
        // GlobalReplace - replace all occurrences of pattern in *pStr
        // Returns true if any replacements were made.
        ////////////////////////////////////////////////////////////////////
        bool GlobalReplace(const std::string& replacement, std::string* pStr) const
        {
            if (!m_pCode || !pStr)
                return false;

            // PCRE2 requires the output buffer to be sized appropriately.
            // We allocate a buffer large enough for the worst case.
            PCRE2_SIZE outlen = pStr->size() + (pStr->size() / 2) + 256;
            if (outlen < 256)
                outlen = 256;

            std::string output;
            bool        retried = false;
            int         rc;

        retry:
            output.resize(outlen);
            outlen = output.size();

            rc = pcre2_substitute(
                m_pCode,
                reinterpret_cast<PCRE2_SPTR>(pStr->c_str()),
                pStr->size(),
                0,
                PCRE2_SUBSTITUTE_GLOBAL,
                nullptr,
                nullptr,
                reinterpret_cast<PCRE2_SPTR>(replacement.c_str()),
                replacement.size(),
                reinterpret_cast<PCRE2_UCHAR*>(&output[0]),
                &outlen);

            if (rc == PCRE2_ERROR_NOMEMORY && !retried)
            {
                // Output buffer was too small; retry with the required size
                retried = true;
                goto retry;
            }

            if (rc >= 0)
            {
                output.resize(outlen);
                *pStr = output;
                return true;
            }

            // rc == 0 means no substitutions were made (but no error)
            // For compatibility with old pcrecpp::GlobalReplace, we return
            // true only if substitutions were made (rc > 0).
            return false;
        }

        ////////////////////////////////////////////////////////////////////
        // FindAndConsume - find the first match in input, store in output,
        //                  and advance input past the match
        ////////////////////////////////////////////////////////////////////
        bool FindAndConsume(StringPiece* pInput, std::string* pOutput) const
        {
            if (!m_pCode || !pInput || !pOutput)
                return false;

            pcre2_match_data* matchData = pcre2_match_data_create_from_pattern(m_pCode, nullptr);
            if (!matchData)
                return false;

            int rc = pcre2_match(
                m_pCode,
                reinterpret_cast<PCRE2_SPTR>(pInput->data()),
                pInput->size(),
                0,
                0,
                matchData,
                nullptr);

            if (rc > 0)
            {
                // Get the ovector (pair of start/end offsets for the match)
                PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(matchData);
                if (ovector[0] <= ovector[1])
                {
                    // Extract matched substring
                    pOutput->assign(
                        pInput->data() + ovector[0],
                        ovector[1] - ovector[0]);

                    // Consume past the end of the match
                    pInput->remove_prefix(ovector[1]);
                    pcre2_match_data_free(matchData);
                    return true;
                }
            }

            pcre2_match_data_free(matchData);
            return false;
        }

    private:
        pcre2_code* m_pCode;
    };

}  // namespace pcrecpp
