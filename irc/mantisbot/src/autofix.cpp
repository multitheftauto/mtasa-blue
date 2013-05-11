#include "mantisbot.h"
#include <rsl/net/irc/user.h>
#include <rsl/net/socket/ipv4.h>
#include <rsl/net/http/client.h>

using namespace Rsl::Net::IRC;
using Rsl::Net::Socket::IPV4Addr;
using namespace Rsl::Net::HTTP;

extern bool MantisLogin(const IRCUser& source, const char* channel);
extern std::string mantis_cookie;
void FixIssue ( const std::string&, int, const std::string& );

void CheckForAutofixIssue ( const std::string& author, const std::string& msg, const std::string& link )
{
    const char* cmsg = msg.c_str ();
    const char* p = cmsg;
    const char* p2;

    if ( !MantisLogin(IRCUser(), 0 ) )
        return;

    if ( strcasestr ( p, "Backported ") != 0 ) return;

    do
    {
        p2 = strcasestr ( p, "Fixed #" );

        if ( !p2 )
            p2 = strcasestr ( p, "Fixes #" );

        if ( p2 )
        {
            p = strchr ( p2, '#' ) + 1;
            int issueNumber = atoi(p);
            if ( issueNumber > 0 )
            {
                FixIssue ( author, issueNumber, link );
            }
        }
        else
            p = 0;
    } while ( p );
}

void FixIssue ( const std::string& author, int issueNumber, const std::string& link )
{
    const Config& config = MantisBot::Instance()->GetConfig();
    const char* authorId = MantisBot::Instance()->GetConfig().GetParser().GetValue("autofix.userids", author.c_str());

    IPV4Addr addr(config.data.mantis.address, config.data.mantis.service);
    IPV4Addr bindAddr("0.0.0.0", "0");

    char tmp [ 32 ];
    snprintf ( tmp, sizeof(tmp), "bug_id=%d", issueNumber );

    HTTPClient httpToken(config.data.mantis.address, addr, bindAddr, config.data.mantis.ssl);
    httpToken.SendHeader("Content-Type", "application/x-www-form-urlencoded");
    httpToken.SendHeader("Cookie", std::string("MANTIS_STRING_COOKIE=") + mantis_cookie);
    httpToken.Connect("/bug_update_advanced_page.php", HTTP_METHOD_POST);
    httpToken.Send(tmp);

    if ( !httpToken.Ok() || httpToken.ResponseStatus() != 200 )
        return;
    std::string contents;
    HTTPStream& stream = httpToken.GetStream (); 
    while ( stream.Eof() == false )
    {
        size_t s = stream.Read ( tmp, sizeof(tmp) );
        contents.append ( tmp, s );
    }

    // Look for the security token
    const char* p = strstr ( contents.c_str(), "name=\"bug_update_token\" value=\"" );
    if ( !p )
        return;

    p = p + strlen("name=\"bug_update_token\" value=\"");
    const char* p2 = strchr(p, '"' );
    if ( !p2 )
        return;

    std::string token;
    token.assign ( p, p2-p );


    // Look for the mantis author id
    char mantisAuthorId [ 128 ];
    mantisAuthorId[0] = '\0';

    if ( authorId )
    {
        p = strstr ( contents.c_str(), "name=\"handler_id\">" );
        if ( !p )
            authorId = 0;
        else
        {
            p = strstr ( p, authorId );
            if ( !p )
                authorId = 0;
            else
            {
                while ( p > contents.c_str() && *p != '"' ) --p;
                --p;
                while ( p > contents.c_str() && *p != '"' ) --p;
                ++p;
                int nAuthorId = atoi(p);
                snprintf(mantisAuthorId, sizeof(mantisAuthorId), "%d", nAuthorId);
            }
        }
    }

    std::string phpsession;

    const std::vector<HTTPHeader>& headers = httpToken.GetResponseHeaders();
    for (std::vector<HTTPHeader>::const_iterator i = headers.begin();
         i != headers.end();
         i++)
    {
        const HTTPHeader* header = &(*i);
        if (!strcasecmp(header->name.c_str(), "Set-Cookie") && !strncmp(header->value.c_str(), "PHPSESSID=", 10))
        {
          const char* p = strchr(header->value.c_str() + 10, ';');
          if (!p)
            break;
          phpsession.append(header->value.c_str() + 10, p - header->value.c_str() - 10);
          break;
        }
    }

    if ( phpsession == "" ) return;

    char poststr [ 512 ];
    snprintf ( poststr, sizeof(poststr), "bug_update_token=%s"
                                         "&bug_id=%d"
                                         "&update_mode=1"
                                         "%s%s"
                                         "&resolution=20"
                                         "&status=80"
                                         "&fixed_in_version=%s"
                                         , token.c_str()
                                         , issueNumber
                                         , authorId ? "&handler_id=" : ""
                                         , authorId ? mantisAuthorId : ""
                                         , config.data.autofix.version );
    HTTPClient httpUpdate(config.data.mantis.address, addr, bindAddr, config.data.mantis.ssl);
    httpToken.SendHeader("Content-Type", "application/x-www-form-urlencoded");
    httpToken.SendHeader("Cookie", std::string("MANTIS_STRING_COOKIE=") + mantis_cookie + "; PHPSESSID=" + phpsession + ";" );
    httpToken.Connect("/bug_update.php", HTTP_METHOD_POST);
    httpToken.Send(poststr);

    // Leave a note ;)
    snprintf ( poststr, sizeof(poststr), "bug_id=%d&bugnote_text=Issue automatically marked as fixed by mantisbot ( Fixed in %s )", issueNumber, link.c_str() );
    HTTPClient httpNote(config.data.mantis.address, addr, bindAddr, config.data.mantis.ssl);
    httpToken.SendHeader("Content-Type", "application/x-www-form-urlencoded");
    httpToken.SendHeader("Cookie", std::string("MANTIS_STRING_COOKIE=") + mantis_cookie + "; PHPSESSID=" + phpsession + ";" );
    httpToken.Connect("/bugnote_add.php", HTTP_METHOD_POST);
    httpToken.Send(poststr);
}

