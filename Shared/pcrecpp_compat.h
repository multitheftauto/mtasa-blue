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
    // Upper bounds applied to each match to keep regex evaluation predictable.
    // Patterns and subjects ultimately come from Lua scripts, so we cap the
    // amount of work a single match may perform. These limits are generous
    // enough that legitimate patterns are unaffected.
    static const uint32_t kMatchLimit = 1000000;  // max backtracking steps
    static const uint32_t kDepthLimit = 10000;    // max backtracking depth
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

        explicit StringPiece(const std::string& str) : m_pData(str.data()), m_uiLength(str.size()) {}

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
        RE(const std::string& pattern, const RE_Options& options) : m_pCode(nullptr), m_pMatchContext(nullptr)
        {
            int        errorcode;
            PCRE2_SIZE erroroffset;

            m_pCode = pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pattern.c_str()), pattern.size(), options.options(), &errorcode, &erroroffset, nullptr);

            if (m_pCode == nullptr)
            {
                // Compilation failed — m_pCode stays NULL.
                // Optionally, could log the error here, but matching
                // old pcrecpp behavior which silently fails.
                PCRE2_UCHAR buffer[256];
                pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
                return;
            }

            // Create a match context with explicit limits to keep the work
            // done by a single match bounded. If creation fails we fall back to
            // a NULL context (PCRE2's compile-time defaults apply).
            m_pMatchContext = pcre2_match_context_create(nullptr);
            if (m_pMatchContext)
            {
                pcre2_set_match_limit(m_pMatchContext, kMatchLimit);
                pcre2_set_depth_limit(m_pMatchContext, kDepthLimit);
            }
        }

        ~RE()
        {
            if (m_pMatchContext)
                pcre2_match_context_free(m_pMatchContext);
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

            int rc = pcre2_match(m_pCode, reinterpret_cast<PCRE2_SPTR>(str.c_str()), str.size(), 0, 0, matchData, m_pMatchContext);

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
            // Start with a generous estimate for the worst case.
            PCRE2_SIZE outlen = pStr->size() + (pStr->size() / 2) + 256;
            if (outlen < 256)
                outlen = 256;

            std::string output;
            bool        retried = false;
            int         rc;

        retry:
            output.resize(outlen);
            outlen = output.size();

            // PCRE2_SUBSTITUTE_OVERFLOW_LENGTH ensures that, on a buffer
            // overflow, outlen is updated with the exact required size so the
            // single retry below can allocate correctly. Without it, the
            // retry would loop on the same size and silently fail.
            rc = pcre2_substitute(m_pCode, reinterpret_cast<PCRE2_SPTR>(pStr->c_str()), pStr->size(), 0,
                                  PCRE2_SUBSTITUTE_GLOBAL | PCRE2_SUBSTITUTE_OVERFLOW_LENGTH, nullptr, m_pMatchContext,
                                  reinterpret_cast<PCRE2_SPTR>(replacement.c_str()), replacement.size(), reinterpret_cast<PCRE2_UCHAR*>(&output[0]), &outlen);

            if (rc == PCRE2_ERROR_NOMEMORY && !retried)
            {
                // Output buffer was too small; outlen now holds the required
                // size (thanks to PCRE2_SUBSTITUTE_OVERFLOW_LENGTH). Retry once.
                retried = true;
                goto retry;
            }

            if (rc > 0)
            {
                // pcre2_substitute returns the number of substitutions made.
                // We return true only if at least one replacement occurred,
                // matching the old pcrecpp::RE::GlobalReplace behavior.
                output.resize(outlen);
                *pStr = output;
                return true;
            }

            // rc == 0 means no substitutions were made (but no error)
            // rc < 0 means an error occurred
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

            int rc = pcre2_match(m_pCode, reinterpret_cast<PCRE2_SPTR>(pInput->data()), pInput->size(), 0, 0, matchData, m_pMatchContext);

            if (rc > 0)
            {
                // Get the ovector (pair of start/end offsets for the match)
                PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(matchData);
                PCRE2_SIZE  start = ovector[0];
                PCRE2_SIZE  end = ovector[1];

                // Defensive bounds check: a well-formed match is always within
                // the subject, but verify before doing pointer arithmetic.
                if (start <= end && end <= pInput->size())
                {
                    // Extract matched substring
                    pOutput->assign(pInput->data() + start, end - start);

                    // Guarantee forward progress. A zero-width match (e.g.
                    // patterns like "x*" or "\\b") matches the empty string
                    // without advancing; consuming 0 bytes would let a caller
                    // loop without making progress. Advance by at least one
                    // code unit in that case.
                    PCRE2_SIZE consume = (end > start) ? end : end + 1;
                    if (consume > pInput->size())
                        consume = pInput->size();
                    pInput->remove_prefix(consume);

                    pcre2_match_data_free(matchData);
                    return true;
                }
            }

            pcre2_match_data_free(matchData);
            return false;
        }

    private:
        pcre2_code*          m_pCode;
        pcre2_match_context* m_pMatchContext;
    };

}  // namespace pcrecpp
