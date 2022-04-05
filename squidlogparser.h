/***************************************************************************
 * Copyright (c) 2022                                                      *
 *      Volnei Cervi Puttini.  All rights reserved.                        *
 *      vcputtini@gmail.com
 *                                                                         *
 * Redistribution and use in source and binary forms, with or without      *
 * modification, are permitted provided that the following conditions      *
 * are met:                                                                *
 * 1. Redistributions of source code must retain the above copyright       *
 *    notice, this list of conditions and the following disclaimer.        *
 * 2. Redistributions in binary form must reproduce the above copyright    *
 *    notice, this list of conditions and the following disclaimer in the  *
 *    documentation and/or other materials provided with the distribution. *
 * 4. Neither the name of the Author     nor the names of its contributors *
 *    may be used to endorse or promote products derived from this software*
 *    without specific prior written permission.                           *
 *                                                                         *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      *
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS  *
 * BE LIABLEFOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR   *
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF    *
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS*
 * INTERRUPTION)                                                           *
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,     *
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING   *
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE      *
 * POSSIBILITY OFSUCH DAMAGE.                                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*!
 * \brief How this file is organized:
 *
 * Namespace: squidlogparser
 *
 * Utilities:
 * class IPv4Addr: General handling of IPv4 addresses. It supports the basic
 * operations needed for IPv4 addresses.
 *
 * template Visitor: Implements Visitor, a helper function for deducing the type
 * of data stored in the variable std::variant.
 * ----------------------------------------------------------------------------
 * struct SquidLogData
 * class DataKey
 * class SquidLogParser
 * class SLPQuery
 * class SLPRawToXML
 */
#ifndef SQUIDLOGPARSER_H
#define SQUIDLOGPARSER_H

#include <algorithm>
#include <arpa/inet.h> // inet_pton()
#include <array>
#include <chrono>
#include <climits> // INT_MAX, LONG_MAX, UINT_MAX, ...
#include <cmath>   // std::isless(), std::isgreater(), ...
#include <cstddef> // size_t
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iomanip> // std::setw()
#include <iostream>
#include <iterator> // std::back_inserter() ...
#include <map>
#include <numeric> // accumulate
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <tinyxml2.h>
using namespace tinyxml2; // mandatory

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) ||                  \
  defined(__WIN64__) || defined(WIN32) || defined(_WIN32) ||                   \
  defined(__WIN32__) || defined(__NT__)
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)
#else
#define Q_DECL_EXPORT __attribute__((visibility("default")))
#define Q_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(SquidLogParser_LIBRARY)
#define SquidLogParser_EXPORT Q_DECL_EXPORT
#else
#define SquidLogParser_EXPORT Q_DECL_IMPORT
#endif

namespace squidlogparser {

/* Utilities---------------------------------------------------------------- */

/*!
 * \brief General handling of IPv4 addresses. It supports the basic operations
 * needed for IPv4 addresses.
 */
class SquidLogParser_EXPORT IPv4Addr
{
public:
  explicit IPv4Addr();
  explicit IPv4Addr(const std::string addr_);
  explicit IPv4Addr(const char* addr_);

  inline std::string getAddr() const;
  inline uint32_t getInt() const;

  static bool isValid(const std::string addr_);
  static uint32_t iptol(const std::string addr_);
  inline static std::string ltoip(uint32_t addr_);

  IPv4Addr& operator=(const IPv4Addr& rhs_);
  inline bool operator>(const IPv4Addr& rhs_) const;
  inline bool operator<(const IPv4Addr& rhs_) const;
  inline bool operator==(const IPv4Addr& rhs_) const;
  inline bool operator!=(const IPv4Addr& rhs_) const;

private:
  std::string str_ = {};
  uint32_t num_ = 0UL;

  static void splitP(std::array<std::string, 4>& arr_, const std::string src_);
};

/* ------------------------------------------------------------------------- */
/*!
 * \internal
 * \brief Implements Visitor, a helper function for deducing the type of data
 * stored in the variable std::variant.
 * \note Based on: https://en.cppreference.com/w/cpp/utility/variant/visit
 */
template<class... Ts>
struct SquidLogParser_EXPORT overloadedP : Ts...
{
  using Ts::operator()...;
};

template<class... Ts>
overloadedP(Ts...) -> overloadedP<Ts...>;
struct SquidLogParser_EXPORT Visitor
{
public:
  enum class TypeVar
  {
    TInt = 0x00,
    TLong,
    TUint,
    TString
  };
  using var_t = std::variant<int, long, uint32_t, std::string>;
  TypeVar varType(var_t t_) const;
};

/* ------------------------------------------------------------------------- */

/*!
 * \brief The SquidLogParserData struct
 *
 *
 * \note Based on: http://www.squid-cache.org/Doc/config/logformat/
 *
 * The default formats available (which do not need re-defining) are:
 *
 * \verbatim
 * logformat squid      %ts.%03tu %6tr %>a %Ss/%03>Hs %<st %rm %ru %[un
 * %Sh/%<a %mt
 *
 * logformat common     %>a %[ui %[un [%tl] "%rm %ru HTTP/%rv"
 * %>Hs %<st %Ss:%Sh
 *
 * logformat combined   %>a %[ui %[un [%tl] "%rm %ru HTTP/%rv" %>Hs %<st
 *  "%{Referer}>h" "%{User-Agent}>h" %Ss:%Sh
 *
 * logformat referrer   %ts.%03tu %>a %{Referer}>h %ru
 *
 * logformat useragent  %>a [%tl] "%{User-Agent}>h"
 * \endverbatim
 *
 */

struct SquidLogParser_EXPORT SquidLogData
{
  /*!
   * \brief The LogFormat enum
   *
   * \warning Do not change the order of the objects defined below.
   */
  enum class LogFormat
  {
    Squid = 0x00,
    Common,
    Combined,
    Referrer,
    UserAgent,
    Unknown
  };

  // --------------------------------------------------------------------------

  /*!
   * \brief The LogFields enum
   *
   * \warning Do not change the order of the objects defined below.
   */
  enum class Fields
  {

    Timestamp = 0x00,
    CliSrcIpAddr, // >a

    LocalTime,     // tl - Local time
    UserName,      //  un - User name (any available), esle return '-'
    UserNameIdent, //  ui - User name from ident

    ResponseTime, // tr (squid)

    ReqMethod,       // rm - Request method (GET/POST etc)
    ReqURL,          // ru - Request URL received (or computed) and sanitized
    ReqProtoVersion, // rv - Request protocol version
    HttpStatus,      // >Hs - HTTP status code sent to the client
    ReqStatusHierStatus, // %Ss/%03>Hs e.g: TCP_MISS/200

    TotalSizeReply, // <st

    HierStatusIpAddress, // %Sh/%<a (squid) ex: ORIGINAL_DST/99.247.57.31
    MimeContentType,     // mt - MIME content type (squid)
    OrigRcvReqHeader,    // >h - Original received request header (combined)

    Referrer,  // %{referrer}
    UserAgent, // %{User-Agent}

    Unknown
  };

  /*!
   * \internal
   * \brief The DataSet_Squid struct
   *
   * \warning Do not change the order of the objects defined below.
   */
  struct DataSet_Squid
  {
    uint32_t timeStamp = 0;
    uint32_t cliSrcIpAddr = 0;
    std::string localTime = {};
    std::string userName = {};
    std::string userNameIdent = {};
    int responseTime = 0;

    std::string reqMethod = {};
    std::string reqURL = {};
    std::string reqProtoVersion = {};
    int httpStatus = 0;
    std::string reqStatusHierStatus = {};

    int totalSizeReply = 0;

    std::string hierStatusIpAddress = {};
    std::string mimeTypeContent = {};
    std::string origRcvReqHeader = {};

    std::string referrer = {};
    std::string userAgent = {};
  };

  // --------------------------------------------------------------------------

  std::map<short, int> HttpCodesUniques_m;
  const std::map<const short, const std::string_view> HttpCodesText_m = {
    // INFORMATIVE RESPONSES
    { 100, "Continue" },
    { 101, "Switching Protocol" },
    { 102, "Processing (WebDAV)" },
    { 103, "Early Hints" },

    // SUCCESS_RESPONSES
    { 200, "OK" },
    { 201, "Created" },
    { 202, "Accepted" },
    { 203, "Non-Authoritative Information" },
    { 204, "No Content" },
    { 205, "Reset Content" },
    { 206, "Partial Content" },
    { 207, "Multi-Status (WebDAV)" },
    { 208, "Multi-Status (WebDAV)" },
    { 226, "IM Used (HTTP Delta encoding" },

    // REDIRECT MESSAGES
    { 300, "Multiple Choice" },
    { 301, "Moved Permanently" },
    { 302, "Found" },
    { 303, "See Other" },
    { 304, "Not Modified" },
    { 307, "Temporary Redirect" },
    { 308, "Permanent Redirect" },

    // CUSTM_ERROR_RESPONSES
    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 402, "Payment Required" },
    { 403, "Forbidden" },
    { 405, "Method Not Allowed" },
    { 406, "Not Acceptable" },
    { 404, "Not Found" },
    { 407, "Proxy Authentication Required" },
    { 408, "Request Timeout" },
    { 410, "Gone" },
    { 409, "Conflict" },
    { 411, "Length Required" },
    { 412, "Precondition Failed" },
    { 413, "Payload Too Large" },
    { 414, "URI Too Long" },
    { 416, "Requested Range Not Satisfiable" },
    { 415, "Unsupported Media Type" },
    { 417, "Expectation Failed" },
    { 418, "I'm a teapot" },
    { 421, "Misdirected Request" },
    { 422, "Unprocessable Entity (WebDAV)" },
    { 423, "Locked (WebDAV)" },
    { 424, "Failed Dependency (WebDAV)" },
    { 425, "Too Early" },
    { 426, "Upgrade Required" },
    { 428, "Precondition Required" },
    { 429, "Too Many Requests" },
    { 431, "Request Header Fields Too Large" },
    { 451, "Unavailable For Legal Reasons" },

    // SERVER_ERROR_RESPONSES
    { 500, "Internal Server Error" },
    { 501, "Not Implemented" },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable" },
    { 504, "Gateway Timeout" },
    { 505, "HTTP Version Not Supported" },
    { 506, "Variant Also Negotiates" },
    { 507, "Insufficient Storage" },
    { 508, "Not Extended" },
    { 511, "Network Authentication Required" }
  };

  // --------------------------------------------------------------------------
  enum class Compare
  {
    EQ = 0x00,
    LT,
    GT,
    LE,
    GE,
    NE,
    BTWAND,
    BTWOR,
    REGEX
  };

  // --------------------------------------------------------------------------
  enum class FormatDB
  {
    UniqueField = 0x00,
    SepFields
  };

  // --------------------------------------------------------------------------
  enum class MethodType
  {
    MTGet = 0x00,
    MTPut,
    MTPost,
    MTConnect,
    MTHead,
    MTDelete,
    MTOptions,
    MTPatch,
    MTTrace,
    MTOthers
  };

  struct Method_t
  {
    const std::string_view sv_;
  } MethodText_t[10] = { { "GET" },     { "PUT" },   { "POST" },
                         { "CONNECT" }, { "HEAD" },  { "DELETE" },
                         { "OPTIONS" }, { "PATCH" }, { "TRACE" },
                         { "OTHERS" } };
  // --------------------------------------------------------------------------

  enum class SLPError
  {
    SLP_SUCCESS = 0x00,
    SLP_ERR_PARSER_FAILED,
    SLP_ERR_INVALID_TIMESTAMP,
    SLP_ERR_INCOMPLETE_NUM_ARGS,
    SLP_ERR_INVALID_DATE,
    SLP_ERR_INVALID_TIME,
    SLP_ERR_INVALID_TS_OR_IP,
    SLP_ERR_XML_FILE_NOT_SAVE,
    SLP_ERR_XML_FILE_NAME_INCONSISTENT,
    SLP_ERR_REGEX_COLLATE,
    SLP_ERR_REGEX_CTYPE,
    SLP_ERR_REGEX_ESCAPE,
    SLP_ERR_REGEX_BACKREF,
    SLP_ERR_REGEX_BRACK,
    SLP_ERR_REGEX_PAREN,
    SLP_ERR_REGEX_BADBRACE,
    SLP_ERR_REGEX_RANGE,
    SLP_ERR_REGEX_SPACE,
    SLP_ERR_REGEX_BADREPEAT,
    SLP_ERR_REGEX_COMPLEXITY,
    SLP_ERR_REGEX_STACK,
    SLP_ERR_UNKNOWN = 0xff,
  };

  const std::unordered_map<SLPError, const std::string_view> mError = {
    { SLPError::SLP_SUCCESS, "Success!" },
    { SLPError::SLP_ERR_PARSER_FAILED,
      "Parser Error: Probable reasons: badly formatted input." },
    { SLPError::SLP_ERR_INVALID_TIMESTAMP, "Invalid Timestamp." },
    { SLPError::SLP_ERR_INCOMPLETE_NUM_ARGS,
      "Incomplete Number of Arguments." },
    { SLPError::SLP_ERR_INVALID_DATE, "Invalid Date: " },
    { SLPError::SLP_ERR_INVALID_TIME, "Invalid Time: " },
    { SLPError::SLP_ERR_INVALID_TS_OR_IP,
      "Timestamp and/or IP address is wrong." },
    { SLPError::SLP_ERR_XML_FILE_NOT_SAVE, "File cannnot be save." },
    { SLPError::SLP_ERR_XML_FILE_NAME_INCONSISTENT,
      "File name is inconsistent." },
    /*! \note Base on: https://en.cppreference.com/w/cpp/regex/error_type */
    { SLPError::SLP_ERR_REGEX_COLLATE,
      "The expression contains an invalid collating element name." },
    { SLPError::SLP_ERR_REGEX_CTYPE,
      "The expression contains an invalid character class name." },
    { SLPError::SLP_ERR_REGEX_ESCAPE,
      "The expression contains an invalid escaped character or a trailing "
      "escape." },
    { SLPError::SLP_ERR_REGEX_BACKREF,
      "The expression contains an invalid back reference." },
    { SLPError::SLP_ERR_REGEX_BRACK,
      "The expression contains mismatched square brackets ('[' and ']')." },
    { SLPError::SLP_ERR_REGEX_PAREN,
      "The expression contains mismatched parentheses ('(' and ')')." },
    { SLPError::SLP_ERR_REGEX_BADBRACE,
      "The expression contains mismatched curly braces ('{' and '}')." },
    { SLPError::SLP_ERR_REGEX_RANGE,
      "The expression contains an invalid character range (e.g. [b-a])." },
    { SLPError::SLP_ERR_REGEX_SPACE,
      "There was not enough memory to convert the expression into a finite "
      "state machine." },
    { SLPError::SLP_ERR_REGEX_BADREPEAT,
      "One of *?+{ was not preceded by a valid regular expression." },
    { SLPError::SLP_ERR_REGEX_COMPLEXITY,
      "The complexity of an attempted match exceeded a predefined level." },
    { SLPError::SLP_ERR_REGEX_STACK,
      "There was not enough memory to perform a match." },

    { SLPError::SLP_ERR_UNKNOWN, "Unknown Error." }
  };
};

/* ------------------------------------------------------------------------- */
/*!
 * \brief Object to interact the composite key of the data map.
 */
class SquidLogParser_EXPORT DataKey
{
private:
  using ipaddr_t = uint32_t;
  using timestamp_t = uint32_t;

  std::pair<timestamp_t, ipaddr_t> key_;

public:
  explicit DataKey(timestamp_t date_, ipaddr_t ip_) { key_ = { date_, ip_ }; };

  const timestamp_t& getTs() const { return key_.first; };
  const ipaddr_t& getIp() const { return key_.second; };

  /*!
   * \internal
   * \brief operator<()
   * \param rhs_
   * \return bool true|false
   * \warning Indexing should be by timestamp only in order to keep the same
   * data entry order. This is very important!
   */
  bool operator<(const DataKey& rhs_) const
  {
    return key_.first < rhs_.key_.first;
  };
};

/* -------------------------------------------------------------------------- */

/*!
 * \brief Class used as data structure for the vector that will store the data
 * about Http Request Codes. It'll allow simpler access to field values.
 */
class HRCData
{
public:
  explicit HRCData(const short c_, const std::string d_, const int s_)
    : code_(std::move(c_))
    , descr_(std::move(d_))
    , score_(std::move(s_))
  {}

  short getCode() const { return code_; };
  std::string getDescription() const { return descr_; };
  int getScore() const { return score_; };

private:
  short code_ = {};
  std::string descr_ = {};
  int score_ = {};
};

/* -------------------------------------------------------------------------- */

/*!
 * \brief The SquidLogParser class
 */
class SquidLogParser_EXPORT SquidLogParser
  : public SquidLogData
  , public Visitor
{
public:
  explicit SquidLogParser(LogFormat log_fmt_ = LogFormat::Squid);

  SquidLogParser& append(const std::string& raw_log_);

  SLPError errorNum() const noexcept;
  std::string getErrorText() const;
  size_t size() const;
  void clear();

  int getPartInt(Fields f_) const;
  uint32_t getPartUInt(Fields f_) const;
  std::string getPartStr(Fields f_) const;

  // Convenience functions
  uint32_t addrToNumeric(const std::string addr_ = std::string()) const;
  std::string numericToAddr(const uint32_t&& ip_ = 0) const;

  uint32_t unixTimestamp(const std::string d_ = std::string()) const;
  std::string unixToSquidDate(std::time_t uts_) const;

  SquidLogParser::SLPError toXML(const std::string&& fn_ = std::string(),
                                 const std::string&& d0_ = std::string(),
                                 const std::string&& ip0_ = std::string(),
                                 const std::string&& d1_ = std::string(),
                                 const std::string&& ip1_ = std::string());

  static constexpr std::string_view invalidText = "@@@"; // Don't change!

  // Test only. Will be removed soon.
  void printuniq()
  {
    for (auto a : HttpCodesUniques_m) {
      std::cout << "-> " << a.first << " : " << a.second << "\n";
    }
  }

protected:
  SLPError slpError_ = SLPError::SLP_SUCCESS;

  std::multimap<DataKey, DataSet_Squid> mEntry;

  template<typename TString = std::string, typename TSize = size_t>
  TString toLower(TString s_, TSize sz_ = 0);

  std::string strRight(const std::string src_, const char sep_) const;

  bool isMonth(const std::string&& s_);
  int monthToNumber(const std::string&& s_) const;
  std::string numberToMonth(const int&& m_) const;
  std::tm mkTime(const std::string d_) const;

  void setError(SLPError e_);
  std::string getErrorRE(std::regex_error& e_) const;

  constexpr int intFields(Fields f_, const DataSet_Squid& d_) const;
  constexpr uint32_t uint32Fields(Fields f_, const DataSet_Squid& d_) const;
  std::string strFields(Fields f_, const DataSet_Squid& d_) const;

  template<typename TVarD, typename TMin, typename TMax, typename TCompare>
  bool decision(TVarD&& data_, TMin&& min_, TMax&& max_, TCompare&& cmp_) const;

  template<typename TVarS, typename TVarD, typename TCompare>
  bool decision(TVarS&& lhs_, TVarD&& rhs_, TCompare&& cmp_) const;

private:
  LogFormat logFmt_;
  std::string rawLog_ = {};
  std::string logFileName_ = {};
  DataSet_Squid ds_squid_ = {};

  static const constexpr char* nmonths_[] = { "Jan", "Feb", "Mar", "Apr",
                                              "May", "Jun", "Jul", "Aug",
                                              "Sep", "Oct", "Nov", "Dec" };

  /*
   * Don't change the regular expressions below, it will make the whole
   * program crash!
   */
  static constexpr char cp_id_fmt_squid_[] =
    "^(\\S+) (\\S+) (\\S+) (\\S+) (\\S+) (\\S+) (\\S+) (\\S+) (\\S+) (.*)$";

  static constexpr char cp_id_fmt_common_[] =
    "^(\\S+) (\\S+) (\\S+) \\[(\\S+ \\S+)\\] \\\"(\\S+) (\\S+) (\\S+)\\\" "
    "(\\S+) (\\S+) (.*)$";

  static constexpr char cp_id_fmt_combined_[] =
    "^(\\S+) (\\S+) (\\S+) \\[(.*?)\\] \\\"(\\S+) (\\S+) (\\S+)\\\" (\\S+) "
    "(\\S+) \\\"(\\S+)\\\" \\\"(.*?)\\\" (.*)$";

  static constexpr char cp_id_fmt_referrer_[] = "^(\\S+) (\\S+) (\\S+) (.*)$";
  static constexpr char cp_id_fmt_useragent_[] =
    "^(\\S+) \\[(\\S+ \\S+)\\] \\\"(.*?)\\\"";

  std::regex re_id_fmt_squid_;
  std::regex re_id_fmt_common_;
  std::regex re_id_fmt_combined_;
  std::regex re_id_fmt_referrer_;
  std::regex re_id_fmt_useragent_;

  SLPError parserSquid();
  SLPError parserCommon();
  SLPError parserCombined();
  SLPError parserReferrer();
  SLPError parserUserAgent();

  void removeExtraWhiteSpaces(const std::string& input_, std::string& output_);
};

/* ------------------------------------------------------------------------- */

/*!
 * \brief The SLPQuery class
 */
class SquidLogParser_EXPORT SLPQuery : public SquidLogParser
{
public:
  explicit SLPQuery(SquidLogParser* obj_);

  SLPQuery& select(const std::string&& d0_ = std::string(),
                   const std::string&& t0_ = std::string(),
                   const std::string&& d1_ = std::string(),
                   const std::string&& t1_ = std::string());

  void field(Fields fld_, Compare cmp_, Visitor::var_t&& t_);

  std::vector<int> getInt(const std::string&& ts_,
                          const std::string&& ip_,
                          Fields fld_) const;
  std::vector<uint32_t> getUInt(const std::string&& ts_,
                                const std::string&& ip_,
                                Fields fld_) const;
  std::vector<std::string> getStr(const std::string&& ts_,
                                  const std::string&& ip_,
                                  Fields fld_) const;

  long sumTotalSizeReply() const;
  long sumResponseTime() const;

  struct accReqMethods_t
  {
    int Get;
    int Put;
    int Post;
    int Connect;
    int Head;
    int Delete;
    int Options;
    int Patch;
    int Trace;
    int Others;
  };
  accReqMethods_t countByReqMethod() const;

  void countByHttpCodes(const short&& code_ = 0);
  std::pair<int, std::string> getHRCScore(const short&& code_ = 0);

  inline std::string MethodText(MethodType mt_) const;

  size_t size() const;
  void clear();

  // This type can be used by the user to simplify the return expression of the
  // getHRCDetails() function
  // HttpRequestCodes_V is a user-defined type that represents the std::vector
  // that contains the fields: Code, Description and Score.
  using HttpRequestCodes_V = std::vector<HRCData>;
  HttpRequestCodes_V getHRCDetails();

protected:
  std::multimap<DataKey, DataSet_Squid> mSubset_;

private:
  LogFormat logFmt_;
  SLPError slpError_;

  using var_t = Visitor::var_t;

  struct Info_t
  {
    int flag_;
    uint32_t begin_date_;
    uint32_t end_date_;
    uint32_t begin_ip_;
    uint32_t end_ip_;
  } info_t;
};

/* ------------------------------------------------------------------------- */

/*!
 * \internal
 * \brief Helper object for XML file creation.
 */
class SquidLogParser_EXPORT SLPRawToXML : public SquidLogParser
{
public:
  explicit SLPRawToXML(LogFormat fmt_, size_t count_);

  SLPRawToXML& append(const DataSet_Squid& ds_);

  SLPError save(const std::string fn_);
  SLPError close();

private:
  XMLDocument doc;
  XMLElement* root;
  XMLElement* root_node;
  XMLDeclaration* decl;

  SLPError slpError_;

  LogFormat logFmt_;
  DataSet_Squid ds_squid_ = {};

  std::string fname_ = {};
  size_t cnt_ = 0;

  std::string localTime() const;
  void writePart();
  SLPError normFn(std::string& fn_) const;
};

/* ------------------------------------------------------------------------- */

} // namespace squidlogparser

#endif // SQUIDLOGPARSER_H
