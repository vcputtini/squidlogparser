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

#include "squidlogparser.h"

namespace squidlogparser {

/* Utilities ---------------------------------------------------------------- */
IPv4Addr::IPv4Addr()
  : str_({})
  , num_(0UL){};

IPv4Addr::IPv4Addr(const std::string addr_)
  : str_(addr_){};

IPv4Addr::IPv4Addr(const char* addr_)
{
  str_ = std::string(addr_, ::strlen(addr_));
};

/*!
 * \brief IPv4Addr::getAddr
 * \return
 */
std::string
IPv4Addr::getAddr() const
{
  return str_;
};

/*!
 * \brief IPv4Addr::getInt
 * \return
 */
uint32_t
IPv4Addr::getInt() const
{
  return num_;
};

IPv4Addr&
IPv4Addr::operator=(const IPv4Addr& rhs_)
{
  if (this == &rhs_) {
    return *this;
  }
  if (isValid(rhs_.str_)) {
    str_ = rhs_.str_;
    num_ = IPv4Addr::iptol(rhs_.str_);
  } else {
    str_ = "Invalid Address";
    num_ = 0UL;
  }
  return *this;
}

bool
IPv4Addr::operator>(const IPv4Addr& rhs_) const
{
  return IPv4Addr::iptol(str_) > IPv4Addr::iptol(rhs_.str_);
}

bool
IPv4Addr::operator<(const IPv4Addr& rhs_) const
{
  return IPv4Addr::iptol(str_) < IPv4Addr::iptol(rhs_.str_);
}

bool
IPv4Addr::operator==(const IPv4Addr& rhs_) const
{
  return IPv4Addr::iptol(str_) == IPv4Addr::iptol(rhs_.str_);
}

bool
IPv4Addr::operator!=(const IPv4Addr& rhs_) const
{
  return IPv4Addr::iptol(str_) != IPv4Addr::iptol(rhs_.str_);
}

/**
 * \internal
 * \brief IPv4Addr::splitP Private member to internal use only.
 * \param arr_ std::array<std::string,4> with tokens
 * \param src String to be splited
 *
 */
void
IPv4Addr::splitP(std::array<std::string, 4>& arr_, const std::string src_)
{
  std::stringstream ss_(src_);
  std::string tok_ = {};
  int8_t i = 0;
  while (std::getline(ss_, tok_, '.')) {
    arr_[i] = std::move(tok_);
    ++i;
  }
}

/**
 * \internal
 * \brief IPv4Addr::isValid
 * \param addr
 * \return true|false
 * \note e.g.: isValid("192.168.1.100");
 */
bool
IPv4Addr::isValid(const std::string addr_)
{
  struct sockaddr_in saddr_in_;
  return inet_pton(AF_INET, addr_.c_str(), &saddr_in_);
}

/**
 * \internal
 * \brief IPv4Addr::iptol Converts an IPv4 address to its decimal equivalent
 * \param addr
 * \return long int Decimal represetation of IpV4 address
 * \note long int il = iptol("192.168.1.110"); (3232235886)
 */
uint32_t
IPv4Addr::iptol(const std::string addr)
{
  if (isValid(addr)) {
    std::array<std::string, 4> a_;
    IPv4Addr::splitP(a_, addr);
    return (std::stol(a_[0]) << 24) | (std::stol(a_[1]) << 16) |
           (std::stol(a_[2]) << 8) | std::stol(a_[3]);
  }
  return 0UL;
}

/**
 * \internal
 * \brief IPv4Addr::ltoip Converts a decimal IPv4 address to its formatted
 * equivalent.
 * \param addr Decimal representation of IpV4 address
 * \return string IpV4 address
 * \note std::string ltoip(3232235886); ("192.168.1.110")
 */
std::string
IPv4Addr::ltoip(uint32_t addr)
{
  std::stringstream ss;
  ss << (addr >> 24) << "." << ((addr >> 16) & 0xFF) << "."
     << ((addr >> 8) & 0xFF) << "." << (addr & 0xFF);
  return ss.str();
}

/* --------------------------------------------------------------------------
 */

/* Visitor ------------------------------------------------------------------
 */
/*!
 * \internal
 * \brief After deducing the type of data stored in the var_t structure, it
 * returns a corresponding integer value.
 * \param t std::variant() structure with data stored for type deduction.
 * \return int Variable type.
 */
Visitor::TypeVar
Visitor::varType(var_t t_) const
{
  TypeVar typevar_ = {};
  std::visit(
    overloadedP{
      [&typevar_]([[maybe_unused]] int arg) { typevar_ = TypeVar::TInt; },
      [&typevar_]([[maybe_unused]] long arg) { typevar_ = TypeVar::TLong; },
      [&typevar_]([[maybe_unused]] uint32_t arg) { typevar_ = TypeVar::TUint; },
      [&typevar_]([[maybe_unused]] const std::string& arg) {
        typevar_ = TypeVar::TString;
      } },
    t_);
  return typevar_;
}

/* SquidLogParser ---------------------------------------------------------- */
SquidLogParser::SquidLogParser(LogFormat log_fmt_)
  : re_id_fmt_squid_(cp_id_fmt_squid_)
  , re_id_fmt_common_(cp_id_fmt_common_)
  , re_id_fmt_combined_(cp_id_fmt_combined_)
  , re_id_fmt_referrer_(cp_id_fmt_referrer_)
  , re_id_fmt_useragent_(cp_id_fmt_useragent_)
{
  logFmt_ = log_fmt_;
};

/*!
 * \brief Appends the log entries to the system.
 * \param raw_log_
 */
SquidLogParser&
SquidLogParser::append(const std::string& raw_log_)
{
  try {
    rawLog_.resize(
      raw_log_.size()); // try to lower the dynamic memory allocation a bit.
    removeExtraWhiteSpaces(raw_log_, rawLog_);
    switch (logFmt_) {
      case LogFormat::Squid: {
        if (parserSquid() == SLPError::SLP_SUCCESS) {
          mEntry.insert({ DataKey(ds_squid_.timeStamp, ds_squid_.cliSrcIpAddr),
                          ds_squid_ });
        }
        break;
      }
      case LogFormat::Common: {
        if (parserCommon() == SLPError::SLP_SUCCESS) {
          mEntry.insert({ DataKey(unixTimestamp(ds_squid_.localTime),
                                  ds_squid_.cliSrcIpAddr),
                          ds_squid_ });
        }
        break;
      }
      case LogFormat::Combined: {
        if (parserCombined() == SLPError::SLP_SUCCESS) {
          mEntry.insert({ DataKey(unixTimestamp(ds_squid_.localTime),
                                  ds_squid_.cliSrcIpAddr),
                          ds_squid_ });
        }
        break;
      }
      case LogFormat::Referrer: {
        if (parserReferrer() == SLPError::SLP_SUCCESS) {
          mEntry.insert({ DataKey(ds_squid_.timeStamp, ds_squid_.cliSrcIpAddr),
                          ds_squid_ });
        }
        break;
      }
      case LogFormat::UserAgent: {
        if (parserUserAgent() == SLPError::SLP_SUCCESS) {
          mEntry.insert({ DataKey(unixTimestamp(ds_squid_.localTime),
                                  ds_squid_.cliSrcIpAddr),
                          ds_squid_ });
        }
      }
    }
  } catch (const std::exception& e) {
    std::cout << "\n"
              << __FUNCTION__ << ": [" << __LINE__ << "] "
              << __FILE__ ": An exception occurred: " << e.what() << "\n\n";
  };

  return *this;
}

/*!
 * \brief Returns the number of log entries read.
 * \return size_t  Is an unsigned integral type.
 */
size_t
SquidLogParser::size() const
{
  return mEntry.size();
}

/*!
 * \brief SquidLogParser::clear
 */
void
SquidLogParser::clear()
{
  mEntry.clear();
}

/*!
 * \brief SquidLogParser::getPartInt
 * \param f_
 * \return int
 */
int
SquidLogParser::getPartInt(Fields f_) const
{
  return intFields(f_, ds_squid_);
}

/*!
 * \brief SquidLogParser::getPartUInt
 * \param f_
 * \return uint32_t
 */
uint32_t
SquidLogParser::getPartUInt(Fields f_) const
{
  return uint32Fields(f_, ds_squid_);
}

/*!
 * \brief SquidLogParser::getPartStr
 * \param f_
 * \return std::string
 */
std::string
SquidLogParser::getPartStr(Fields f_) const
{
  return strFields(f_, ds_squid_);
}

/*!
 * \brief addrToNumeric is a convenience function to be used
 * in simple conversions or together with the the DataKey() object.
 * \param addr_ IPv4 address in dot-decimal notation.
 * \return uint32_t Decimal representation.
 * \note DataKey usage:
 *
 */
uint32_t
SquidLogParser::addrToNumeric(const std::string addr_) const
{
  return (!addr_.empty() ? IPv4Addr::iptol(addr_) : 0UL);
}

/*!
 * \brief numericToAddr, convenience function to be used in conversions.
 * \param ip_ Decimal IPv4 representation.
 * \return string IPv4 address in dot-decimal notation.
 */
std::string
SquidLogParser::numericToAddr(const uint32_t&& ip_) const
{
  return (ip_ != 0UL ? IPv4Addr::ltoip(ip_) : std::string());
}

/*!
 * \brief unixTimestamp, convenience function that convert the human-readable
 * format date to a Unix timestamp.
 * \param d_ string date 'n' time
 * \return uint32_t Epoch
 * \note Date format accepted:dd/Mmm/yyyy:hh:mm:ss [TZ]
 * \note TZ is ignored
 */
uint32_t
SquidLogParser::unixTimestamp(const std::string d_) const
{
  if (!d_.empty()) {
    boost::regex re_(cp_fmt_squid_date);
    boost::match_results<std::string::const_iterator> match;
    boost::regex_match(d_, match, re_);
    if (!match.empty()) {
      auto [dd_, mm_, yy_, hh_, mn_, ss_] = std::tuple(std::stoi(match[1]),
                                                       monthToNumber(match[2]),
                                                       std::stoi(match[3]),
                                                       std::stoi(match[4]),
                                                       std::stoi(match[5]),
                                                       std::stoi(match[6]));

      if ((dd_ >= 1 && dd_ <= 31) && (mm_ >= 1 && mm_ <= 12) && (yy_ >= 1970) &&
          (hh_ >= 0 && hh_ <= 23) && (mn_ >= 0 && mn_ <= 59) &&
          (ss_ >= 0 && ss_ <= 59)) {
        std::tm tm_ = mkTime(d_);
        return std::mktime(&tm_);
      }
    }
  }
  return 0;
}

/*!
 * \brief unixToSquidDate, convenience function that convert the numeric
 * Unix timestamp to a human-readable format date.
 * \param uts_ Unix Timestamp.
 * \return std::string Squid-format date in human-readable.
 */
std::string
SquidLogParser::unixToSquidDate(std::time_t uts_) const
{
  char buf_[27];
  struct std::tm tm_ = { *std::localtime(&uts_) };
  ::strftime(buf_, sizeof(buf_), "%d/%b/%Y:%H:%M:%S %z", &tm_);
  return std::string(buf_);
}

/*!
 * \brief Converts log entries to XML format and writes to file.
 * \param fn File name with path (Mandatory).
 * \param Beginning date
 * \param Beginning time
 * \param End date
 * \param End time
 *
 * \note If the user does not inform the '.xml' extension, the library will
 * provide it. If another extension is informed, it will not be considered
 * valid and the function will return -1.
 *
 * \warning Important: The SLPRawToXML object will always erase the data read
 * after writing the XML file.
 */

SquidLogParser::SLPError
SquidLogParser::toXML(const std::string&& fn_,
                      const std::string&& d0_,
                      const std::string&& ip0_,
                      const std::string&& d1_,
                      const std::string&& ip1_)
{

  if (fn_.empty()) {
    setError(SLPError::SLP_ERR_XML_FILE_NAME_INCONSISTENT);
    return SLPError::SLP_ERR_XML_FILE_NAME_INCONSISTENT;
  }

  if ((slpError_ == SLPError::SLP_SUCCESS) && (size() > 0)) {
    uint32_t min = unixTimestamp(d0_);
    uint32_t ip0 = addrToNumeric(ip0_);

    uint32_t max = unixTimestamp(d1_);
    uint32_t ip1 = addrToNumeric(ip1_);

    SLPRawToXML xml(logFmt_, size());
    if (xml.save(fn_) != SLPError::SLP_SUCCESS) {
      setError(SLPError::SLP_ERR_XML_FILE_NAME_INCONSISTENT);
      return SLPError::SLP_ERR_XML_FILE_NAME_INCONSISTENT;
    }
    if ((!d0_.empty() && !ip0_.empty()) && (!d1_.empty() && !ip1_.empty())) {
      for (auto it_ = mEntry.cbegin(); it_ != mEntry.cend(); ++it_) {
        if ((it_->first.getTs() >= min) && (it_->first.getTs() <= max) &&
            (it_->first.getIp() >= ip0) && (it_->first.getIp() <= ip1)) {
          xml.append(it_->second);
        }
      }
    } else {
      for (const auto& a : mEntry) {
        xml.append(a.second);
      }
    }
    if (xml.close() != SLPError::SLP_SUCCESS) {
      setError(SLPError::SLP_ERR_XML_FILE_NOT_SAVE);
      return SLPError::SLP_ERR_XML_FILE_NOT_SAVE;
    }
  }
  return SLPError::SLP_SUCCESS;
}

/*!
 * \brief Show decoded URL
 * \param raw_ Raw URL
 * \return string URL decoded.
 *
 * \note This function is the result of Internet research and adaptations that
 * we consider convenient.
 */
std::string
SquidLogParser::ShowDecodedUrl(const std::string raw_) const
{
  std::string tmp_(raw_, raw_.size()); // tries to decrease dynamic allocation.

  for (auto it_ = raw_.cbegin(); it_ != raw_.cend(); ++it_) {
    switch (*it_) {
      case '%': {
        ++it_;
        std::string byte_;
        byte_.push_back(std::move(*it_));
        ++it_;
        byte_.push_back(std::move(*it_));
        tmp_.push_back(
          std::move(static_cast<unsigned char>(std::stol(byte_, nullptr, 16))));
        break;
      }
      case '+': {
        tmp_.push_back(std::move(' '));
        break;
      }
      default: {
        tmp_.push_back(std::move(*it_));
      }
    }
  }
  return tmp_;
}

/* protected----------------------------------------------------------------
 */
/*!
 * \internal
 * \brief Simple recursive uppercase to lowercase characters conversion.
 * \param s_ Text with (maybe) uppercase  characteres.
 * \return Text in lowercase.
 */
template<typename TString, typename TSize>
TString
SquidLogParser::toLower(TString s_, TSize sz_)
{
  if (sz_ != s_.size()) {
    s_[sz_] = ::tolower(s_[sz_]);
    s_ = toLower(s_, ++sz_);
  }
  return s_;
}

/*!
 * \brief Returns the right part of string until the end. From position+1 of
 * the informed separator.
 *
 * \param src_
 * \param sep_
 * \return std::string
 *
 * \example std::string s_ = strRight("NONE/200"s,'/'); // s_ = "200";
 *
 */
std::string
SquidLogParser::strRight(const std::string src_, const char sep_) const
{
  if (size_t f = std::string_view{ src_ }.find(sep_); f != std::string::npos) {
    return src_.substr(f + 1, src_.length());
  }
  return std::string();
}
/*!
 * \internal
 * \brief Returns true or false if month name is corret
 * \param s Month name abbreviation
 * \return true|false
 * \code{.cc}
 * bool b = isMonth("Jan");
 * \endcode
 */
bool
SquidLogParser::isMonth(const std::string&& s_)
{
  auto begin_ = std::cbegin(nmonths_);
  auto end_ = std::cend(nmonths_);

  const auto f_ = std::find_if(begin_, end_, [&s_](const char* cptr_) {
    if (std::strcmp(s_.c_str(), cptr_) == 0) {
      return true;
    }
    return false;
  });

  return (f_ != end_);
}

/*!
 * \internal
 * \brief Converts the month's name abbreviation to number.
 * \param s_
 * \return int
 */
int
SquidLogParser::monthToNumber(const std::string&& s_) const
{
  auto begin_ = std::cbegin(nmonths_);
  auto end_ = std::cend(nmonths_);

  const auto f_ = std::find_if(begin_, end_, [&s_](const char* cptr_) {
    if (std::strcmp(s_.c_str(), cptr_) == 0) {
      return true;
    }
    return false;
  });

  if (f_ != end_) {
    return std::distance(begin_, f_) + 1;
  }
  return 0;
}

/*!
 * \brief Converts the month's number to abbreviation name.
 * \param m_ A number between 1 and 12
 * \return Abbrev of month's name
 */
std::string
SquidLogParser::numberToMonth(const int&& m_) const
{
  if ((m_ >= 1) && (m_ <= 12)) {
    return std::string(nmonths_[m_ - 1]);
  }
  return std::string();
}

/*!
 * \internal
 * \brief Parses date and time strings and save the values inside std::tm.
 * \param d_ Date (dd/Mmm/yyyy:hh:mm:ss -0000)
 * \return std::tm
 */
std::tm
SquidLogParser::mkTime(const std::string d_) const
{
  struct std::tm tm_tmp = {};
  tm_tmp.tm_year = std::move(std::stoi(d_.substr(7, 4))) - 1900;
  tm_tmp.tm_mon = std::move(monthToNumber(d_.substr(3, 3))) - 1;
  tm_tmp.tm_mday = std::move(std::stoi(d_.substr(0, 2)));
  tm_tmp.tm_hour = std::move(std::stoi(d_.substr(12, 2)));
  tm_tmp.tm_min = std::move(std::stoi(d_.substr(15, 2)));
  tm_tmp.tm_sec = std::move(std::stoi(d_.substr(18, 2)));

  return tm_tmp;
}

/*!
 * \internal
 * \brief SquidLogParser::setError
 * \param e_ Error code
 */
void
SquidLogParser::setError(SLPError e_)
{
  slpError_ = e_;
}

std::string
SquidLogParser::getErrorRE(boost::regex_error& e_) const
{
  switch (e_.code()) {
    case boost::regex_constants::error_collate: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_COLLATE);
      break;
    }
    case boost::regex_constants::error_ctype: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_CTYPE);
      break;
    }
    case boost::regex_constants::error_escape: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_ESCAPE);
      break;
    }
    case boost::regex_constants::error_backref: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_BACKREF);
      break;
    }
    case boost::regex_constants::error_brack: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_BRACK);
      break;
    }
    case boost::regex_constants::error_paren: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_PAREN);
      break;
    }
    case boost::regex_constants::error_brace: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_BRACE);
      break;
    }
    case boost::regex_constants::error_badbrace: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_BADBRACE);
      break;
    }
    case boost::regex_constants::error_range: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_RANGE);
      break;
    }
    case boost::regex_constants::error_space: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_SPACE);
      break;
    }
    case boost::regex_constants::error_badrepeat: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_BADREPEAT);
      break;
    }
    case boost::regex_constants::error_complexity: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_COMPLEXITY);
      break;
    }
    case boost::regex_constants::error_stack: {
      const_cast<SquidLogParser*>(this)->setError(
        SLPError::SLP_ERR_REGEX_STACK);
      break;
    }
    default: {
      const_cast<SquidLogParser*>(this)->setError(SLPError::SLP_ERR_UNKNOWN);
    }
  }
  return getErrorText();
}

/*!
 * \internal
 * \brief Returns the value of integer fields
 * \param f_ Field Id
 * \param d_ Data
 * \return int
 */
constexpr int
SquidLogParser::intFields(Fields f_, const DataSet_Squid& d_) const
{

  switch (f_) {
    case Fields::ResponseTime: {
      return d_.responseTime;
    }
    case Fields::TotalSizeReply: {
      return d_.totalSizeReply;
    }
    case Fields::HttpStatus: {
      return d_.httpStatus;
    }
    default: {
      return -1;
    }
  }
}

/*!
 * \internal
 * \brief Returns the value of uint32_t fields
 * \param f_ Field Id
 * \param d_ Data
 * \return uint32_t
 */
constexpr uint32_t
SquidLogParser::uint32Fields(Fields f_, const DataSet_Squid& d_) const
{
  switch (f_) {
    case Fields::Timestamp: {
      return d_.timeStamp;
    }
    case Fields::CliSrcIpAddr: {
      return d_.cliSrcIpAddr;
    }
    default: {
      return 0;
    }
  }
}

/*!
 * \internal
 * \brief Returns the value of string fields
 * \param f_ Field Id
 * \param d_ Data
 * \return std::string
 */
std::string
SquidLogParser::strFields(Fields f_, const DataSet_Squid& d_) const
{
  switch (f_) {
    case Fields::Timestamp: {
      return unixToSquidDate(d_.timeStamp);
    }
    case Fields::CliSrcIpAddr: {
      return IPv4Addr::ltoip(d_.cliSrcIpAddr);
    }
    case Fields::LocalTime: {
      return d_.localTime;
    }
    case Fields::UserName: {
      return d_.userName;
    }
    case Fields::UserNameIdent: {
      return d_.userNameIdent;
    }
    case Fields::ReqMethod: {
      return d_.reqMethod;
    }
    case Fields::ReqURL: {
      return d_.reqURL;
    }
    case Fields::ReqProtoVersion: {
      return d_.reqProtoVersion;
    }
    case Fields::ReqStatusHierStatus: {
      return d_.reqStatusHierStatus;
    }
    case Fields::HierStatusIpAddress: {
      return d_.hierStatusIpAddress;
    }
    case Fields::MimeContentType: {
      return d_.mimeTypeContent;
    }
    case Fields::OrigRcvReqHeader: {
      return d_.origRcvReqHeader;
    }
    case Fields::Referrer: {
      return d_.referrer;
    }
    case Fields::UserAgent: {
      return d_.userAgent;
    }
    default: {
      return invalidText.data();
    }
  }
}

/*!
 * \internal
 * \brief This template function implements the logical AND and OR operations
 * for functions like "between A AND B" or "between A OR B".
 *
 * \param data_ The data to be compared with its limits.
 * \param min_ Lower value.
 * \param max_ Highest value.
 * \param cmp_ Type of logical operation: BTWAND | BTWOR
 *
 *
 */
template<typename TVarD, typename TMin, typename TMax, typename TCompare>
bool
SquidLogParser::decision(TVarD&& data_,
                         TMin&& min_,
                         TMax&& max_,
                         TCompare&& cmp_) const
{
  switch (cmp_) {
    case Compare::BTWAND: {
      return ((data_ >= min_) && (data_ <= max_));
    }
    case Compare::BTWOR: {
      return ((data_ >= min_) || (data_ <= max_));
    }
    default: {
      return false;
    }
  }
};

/*!
 * \internal
 * \brief Overloaded: This template function implements the logical
 * operations:
 * == < > <= >= !=
 * \param lhs_ Argument 1
 * \param rhs_ Argument 2
 * \param cmp_ Type of logical operation. EQ, LT, GT, LE, GE, NE
 */
template<typename TVarS, typename TVarD, typename TCompare>
bool
SquidLogParser::decision(TVarS&& lhs_, TVarD&& rhs_, TCompare&& cmp_) const
{
  switch (cmp_) {
    case Compare::EQ: {
      return lhs_ == rhs_;
    }
    case Compare::LT: {
      return lhs_ < rhs_;
    }
    case Compare::GT: {
      return lhs_ > rhs_;
    }
    case Compare::LE: {
      return lhs_ <= rhs_;
    }
    case Compare::GE: {
      return lhs_ >= rhs_;
    }
    case Compare::NE: {
      return lhs_ != rhs_;
    }
    default: {
      return false;
    }
  }
};

/*!
 * \brief Return a error code.
 * \return
 */
SquidLogParser::SLPError
SquidLogParser::errorNum() const noexcept
{
  return slpError_;
}

/*!
 * \brief Return the description of the error.
 * \return std::string
 */
std::string
SquidLogParser::getErrorText() const
{
  if (const auto it_(mError.find(slpError_)); it_ != mError.end()) {
    return it_->second.data();
  }
  return mError.at(SLPError::SLP_ERR_UNKNOWN).data();
}

/* private------------------------------------------------------------------
 */

/*!
 * \internal
 *
 * \note The constants below are defined in CMakeLists.txt
 *
 * DEBUG_PARSER_SQUID
 * DEBUG_PARSER_COMMON
 * DEBUG_PARSER_COMBINED
 * DEBUG_PARSER_REFERRER
 * DEBUG_PARSER_USERAGENT
 *
 */

/*!
 * \internal
 * \brief SquidLogParser::parserSquid
 * \return
 *
 */
SquidLogData::SLPError
SquidLogParser::parserSquid()
{

  if (rawLog_.empty()) {
    setError(SLPError::SLP_SUCCESS);
    return SLPError::SLP_SUCCESS;
  }

#ifdef DEBUG_PARSER_SQUID
  std::cout << "raw : " << rawLog_ << "\n";
#endif

  try {
    boost::match_results<std::string::const_iterator> match;
    boost::regex_match(rawLog_, match, re_id_fmt_squid_);
    if (match.empty()) {
      setError(SLPError::SLP_ERR_PARSER_FAILED);
      return SLPError::SLP_ERR_PARSER_FAILED;
    }
    ds_squid_ = {};
    ds_squid_.timeStamp = std::move(std::stod(match[1]));
    ds_squid_.responseTime = std::move(std::stoi(match[2]));
    ds_squid_.cliSrcIpAddr = std::move(IPv4Addr::iptol(match[3]));
    ds_squid_.reqStatusHierStatus = std::move(match[4]);
    ds_squid_.totalSizeReply = std::move(std::stoi(match[5]));
    ds_squid_.reqMethod = std::move(match[6]);
    ds_squid_.reqURL = std::move(match[7]);
    ds_squid_.userName = std::move(match[8]);
    ds_squid_.hierStatusIpAddress = std::move(match[9]);
    ds_squid_.mimeTypeContent = std::move(match[10]);

    // stores unique http request codes for later score.
    HttpCodesUniques_m.insert(
      { std::move(std::stoi(strRight(ds_squid_.reqStatusHierStatus, '/'))),
        0 });

#ifdef DEBUG_PARSER_SQUID
    std::cout << "ds_squid :\n";
    std::cout << ds_squid_.timeStamp << "\n"
              << ds_squid_.responseTime << "\n"
              << ds_squid_.cliSrcIpAddr << "\n"
              << ds_squid_.reqStatusHierStatus << "\n"
              << ds_squid_.totalSizeReply << "\n"
              << ds_squid_.reqMethod << "\n"
              << ds_squid_.reqURL << "\n"
              << ds_squid_.userName << "\n"
              << ds_squid_.hierStatusIpAddress << "\n"
              << ds_squid_.mimeTypeContent << "\n";
    for (size_t i = 1; i < match.size(); i++) {
      std::cout << "> " << i << " -- " << match[i] << "\n";
    }
#endif
  } catch (boost::regex_error& e_) {
    std::cout << "Parser PFLogentry regex error = " << e_.what() << "\n";
    setError(SLPError::SLP_ERR_PARSER_FAILED);
    return SLPError::SLP_ERR_PARSER_FAILED;
  } catch (const std::exception& e_) {
    std::cout << "[" << __LINE__ << "] "
              << __FILE__ ": An exception occurred: " << e_.what() << "\n";
  }

  setError(SLPError::SLP_SUCCESS);
  return SLPError::SLP_SUCCESS;
}

/*!
 * \internal
 * \brief SquidLogParser::parserCommon
 * \return
 */
SquidLogData::SLPError
SquidLogParser::parserCommon()
{
  if (rawLog_.empty()) {
    setError(SLPError::SLP_SUCCESS);
    return SLPError::SLP_SUCCESS;
  }

#ifdef DEBUG_PARSER_COMMON
  std::cout << "raw : " << rawLog_ << "\n";
#endif

  try {
    boost::match_results<std::string::const_iterator> match;
    boost::regex_match(rawLog_, match, re_id_fmt_common_);
    if (match.empty()) {
      setError(SLPError::SLP_ERR_PARSER_FAILED);
      return SLPError::SLP_ERR_PARSER_FAILED;
    }

    ds_squid_ = {};
    ds_squid_.cliSrcIpAddr = std::move(IPv4Addr::iptol(match[1]));
    ds_squid_.userNameIdent = std::move(match[2]);
    ds_squid_.userName = std::move(match[3]);
    ds_squid_.localTime = std::move(match[4]);
    ds_squid_.reqMethod = std::move(match[5]);
    ds_squid_.reqURL = std::move(match[6]);
    ds_squid_.reqProtoVersion = std::move(match[7]);
    ds_squid_.httpStatus = std::move(std::stoi(match[8]));
    ds_squid_.totalSizeReply = std::move(std::stoi(match[9]));
    ds_squid_.reqStatusHierStatus = std::move(match[10]);

    // stores unique http request codes for later score.
    HttpCodesUniques_m.insert({ std::move(std::stoi(match[8])), 0 });

#ifdef DEBUG_PARSER_COMMON
    std::cout << "ds_common :\n";
    std::cout << ds_squid_.cliSrcIpAddr << "\n"
              << ds_squid_.userNameIdent << "\n"
              << ds_squid_.userName << "\n"
              << ds_squid_.localTime << "\n"
              << ds_squid_.reqMethod << "\n"
              << ds_squid_.reqURL << "\n"
              << ds_squid_.reqProtoVersion << "\n"
              << ds_squid_.httpStatus << "\n"
              << ds_squid_.totalSizeReply << "\n"
              << ds_squid_.reqStatusHierStatus << "\n";

    for (size_t i = 1; i < match.size(); i++) {
      std::cout << "> " << i << " -- " << match[i] << "\n";
    }
#endif
  } catch (boost::regex_error& e_) {
    std::cout << "Parser PFLogentry regex error = " << e_.what() << "\n";
    setError(SLPError::SLP_ERR_PARSER_FAILED);
    return SLPError::SLP_ERR_PARSER_FAILED;
  } catch (const std::exception& e_) {
    std::cout << "[" << __LINE__ << "] "
              << __FILE__ ": An exception occurred: " << e_.what() << "\n";
  }

  setError(SLPError::SLP_SUCCESS);
  return SLPError::SLP_SUCCESS;
}

/*!
 * \internal
 * \brief SquidLogParser::parserCombined
 * \return
 */
SquidLogData::SLPError
SquidLogParser::parserCombined()
{
  if (rawLog_.empty()) {
    setError(SLPError::SLP_SUCCESS);
    return SLPError::SLP_SUCCESS;
  }

#ifdef DEBUG_PARSER_COMBINED
  std::cout << "raw combined : " << rawLog_ << "\n";
#endif

  try {
    boost::match_results<std::string::const_iterator> match;
    boost::regex_match(rawLog_, match, re_id_fmt_combined_);
    if (match.empty()) {
      setError(SLPError::SLP_ERR_PARSER_FAILED);
      return SLPError::SLP_ERR_PARSER_FAILED;
    }

    ds_squid_ = {};
    ds_squid_.cliSrcIpAddr = std::move(IPv4Addr::iptol(match[1]));
    ds_squid_.userNameIdent = std::move(match[2]);
    ds_squid_.userName = std::move(match[3]);
    ds_squid_.localTime = std::move(match[4]);
    ds_squid_.reqMethod = std::move(match[5]);
    ds_squid_.reqURL = std::move(match[6]);
    ds_squid_.reqProtoVersion = std::move(match[7]);
    ds_squid_.httpStatus = std::move(std::stoi(match[8]));
    ds_squid_.totalSizeReply = std::move(std::stoi(match[9]));
    ds_squid_.referrer = std::move(match[10]);
    ds_squid_.userAgent = std::move(match[11]);
    ds_squid_.reqStatusHierStatus = std::move(match[12]);

    // stores unique http request codes for later score.
    HttpCodesUniques_m.insert({ std::move(std::stoi(match[8])), 0 });

#ifdef DEBUG_PARSER_COMBINED
    std::cout << "ds_combined :\n";
    std::cout << ds_squid_.cliSrcIpAddr << "\n"
              << ds_squid_.userNameIdent << "\n"
              << ds_squid_.userName << "\n"
              << ds_squid_.localTime << "\n"
              << ds_squid_.reqMethod << "\n"
              << ds_squid_.reqURL << "\n"
              << ds_squid_.reqProtoVersion << "\n"
              << ds_squid_.httpStatus << "\n"
              << ds_squid_.totalSizeReply << "\n"
              << ds_squid_.referrer << "\n"
              << ds_squid_.userAgent << "\n"
              << ds_squid_.reqStatusHierStatus << "\n";

    for (size_t i = 1; i < match.size(); i++) {
      std::cout << "> " << i << " -- " << match[i] << "\n";
    }
#endif
  } catch (boost::regex_error& e_) {
    std::cout << "Parser PFLogentry regex error = " << e_.what() << "\n";
    setError(SLPError::SLP_ERR_PARSER_FAILED);
    return SLPError::SLP_ERR_PARSER_FAILED;
  } catch (const std::exception& e_) {
    std::cout << "[" << __LINE__ << "] "
              << __FILE__ ": An exception occurred: " << e_.what() << "\n";
  }

  setError(SLPError::SLP_SUCCESS);
  return SLPError::SLP_SUCCESS;
}

/*!
 * \internal
 * \brief SquidLogParser::parserReferrer
 * \return
 */
SquidLogData::SLPError
SquidLogParser::parserReferrer()
{
  if (rawLog_.empty()) {
    setError(SLPError::SLP_SUCCESS);
    return SLPError::SLP_SUCCESS;
  }

#ifdef DEBUG_PARSER_REFERRER
  std::cout << "raw : " << rawLog_ << "\n";
#endif

  try {
    boost::match_results<std::string::const_iterator> match;
    boost::regex_match(rawLog_, match, re_id_fmt_referrer_);
    if (match.empty()) {
      setError(SLPError::SLP_ERR_PARSER_FAILED);
      return SLPError::SLP_ERR_PARSER_FAILED;
    }

    ds_squid_ = {};
    ds_squid_.timeStamp = std::move(std::stoul(match[1]));
    ds_squid_.cliSrcIpAddr = std::move(IPv4Addr::iptol(match[2]));
    ds_squid_.referrer = std::move(match[3]);
    ds_squid_.reqURL = std::move(match[4]);

#ifdef DEBUG_PARSER_REFERRER
    std::cout << "ds_referrer :\n";
    std::cout << ds_squid_.timeStamp << "\n"
              << ds_squid_.cliSrcIpAddr << "\n"
              << ds_squid_.referrer << "\n"
              << ds_squid_.reqURL << "\n";

    for (size_t i = 1; i <= match.size(); i++) {
      std::cout << "> " << i << " -- " << match[i] << "\n";
    }
#endif
  } catch (boost::regex_error& e_) {
    std::cout << "Parser PFLogentry regex error = " << e_.what() << "\n";
    setError(SLPError::SLP_ERR_PARSER_FAILED);
    return SLPError::SLP_ERR_PARSER_FAILED;
  } catch (const std::exception& e_) {
    std::cout << "[" << __LINE__ << "] "
              << __FILE__ ": An exception occurred: " << e_.what() << "\n";
  }

  setError(SLPError::SLP_SUCCESS);
  return SLPError::SLP_SUCCESS;
}

/*!
 * \internal
 * \brief SquidLogParser::parserUserAgent
 * \return
 */
SquidLogData::SLPError
SquidLogParser::parserUserAgent()
{
  if (rawLog_.empty()) {
    setError(SLPError::SLP_SUCCESS);
    return SLPError::SLP_SUCCESS;
  }

#ifdef DEBUG_PARSER_USERAGENT
  std::cout << "raw : " << rawLog_ << "\n";
#endif

  try {
    boost::match_results<std::string::const_iterator> match;
    boost::regex_match(rawLog_, match, re_id_fmt_useragent_);
    if (match.empty()) {
      setError(SLPError::SLP_ERR_PARSER_FAILED);
      return SLPError::SLP_ERR_PARSER_FAILED;
    }

    ds_squid_ = {};
    ds_squid_.cliSrcIpAddr = std::move(IPv4Addr::iptol(match[1]));
    ds_squid_.localTime = std::move(match[2]);
    ds_squid_.userAgent = std::move(match[3]);

#ifdef DEBUG_PARSER_USERAGENT
    std::cout << "ds_useragent :\n";
    std::cout << ds_squid_.cliSrcIpAddr << "\n"
              << ds_squid_.localTime << " "
              << unixTimestamp(ds_squid_.localTime) << "\n"
              << ds_squid_.userAgent << "\n";

    for (size_t i = 1; i <= match.size(); i++) {
      std::cout << "> " << i << " -- " << match[i] << "\n";
    }
#endif
  } catch (boost::regex_error& e_) {
    std::cout << "Parser PFLogentry regex error = " << e_.what() << "\n";
    setError(SLPError::SLP_ERR_PARSER_FAILED);
    return SLPError::SLP_ERR_PARSER_FAILED;
  } catch (const std::exception& e_) {
    std::cout << "[" << __LINE__ << "] "
              << __FILE__ ": An exception occurred: " << e_.what() << "\n";
  }

  setError(SLPError::SLP_SUCCESS);
  return SLPError::SLP_SUCCESS;
}

/*!
 * \internal
 * \brief Normalize a string removing the extra white spaces between words.
 * \param input_
 * \param output_
 * \note Adapted from:
 * https://stackoverflow.com/questions/35301432/remove-extra-white-spaces-in-c/35302029
 */
void
SquidLogParser::removeExtraWhiteSpaces(const std::string& input_,
                                       std::string& output_)
{
  output_.clear(); // unless you want to add at the end of existing string...
  std::unique_copy(
    input_.cbegin(),
    input_.cend(),
    std::back_insert_iterator<std::string>(output_),
    [](char a_, char b_) { return ::isspace(a_) && ::isspace(b_); });
}

/* SLPQuery --------------------------------------------------------------- */
/*!
 * \brief Constructs a SLPQuery object.
 * \param pf_ Object pointer
 */
SLPQuery::SLPQuery(SquidLogParser* obj_)
  : SquidLogParser(*obj_)
  , mSubset_({})
  , slpError_(SLPError::SLP_SUCCESS)
  , info_t({})
{
  logFmt_ = getFormat();
}

/*!
 * \brief Selects the records given the following arguments: Being date, begin
 * IP, end date and end IP.
 * \param d0_  Begin date. Format: dd/Mmm/yyyy:hh:mm:ss
 * \param ip0_ Being IP address.
 * \param d1_  End date. Format: dd/Mmm/yyyy:hh:mm:ss
 * \param ip1_ Ende IP address.
 * \return *this
 */
SLPQuery&
SLPQuery::select(const std::string&& d0_,
                 const std::string&& ip0_,
                 const std::string&& d1_,
                 const std::string&& ip1_)
{
  info_t = {};
  if ((!d0_.empty() && !ip0_.empty()) && (d1_.empty() && ip1_.empty())) { // a,b
    info_t.begin_date_ = unixTimestamp(d0_);
    info_t.begin_ip_ = addrToNumeric(ip0_);
    if (info_t.begin_date_ > 0 && info_t.begin_ip_ > 0) {
      info_t.end_date_ = info_t.begin_date_;
      info_t.end_ip_ = info_t.begin_ip_;
      setError(SLPError::SLP_SUCCESS);
      info_t.flag_ = true;
    } else {
      setError(SLPError::SLP_ERR_INVALID_TS_OR_IP);
      info_t.flag_ = false;
    }
  } else if ((!d0_.empty() && !ip0_.empty()) &&
             (!d1_.empty() && !ip1_.empty())) { // a,b,c,d
    info_t.begin_date_ = unixTimestamp(d0_);
    info_t.end_date_ = unixTimestamp(d1_);
    info_t.begin_ip_ = addrToNumeric(ip0_);
    info_t.end_ip_ = addrToNumeric(ip1_);
    if ((info_t.begin_date_ > 0 && info_t.begin_ip_ > 0) &&
        (info_t.end_date_ > 0 && info_t.end_ip_ > 0)) {
      setError(SLPError::SLP_SUCCESS);
      info_t.flag_ = true;
    } else {
      setError(SLPError::SLP_ERR_INVALID_TS_OR_IP);
      info_t.flag_ = false;
    }
  } else {
    setError(SLPError::SLP_ERR_INCOMPLETE_NUM_ARGS);
    info_t.flag_ = false;
  }

  return *this;
}

/*!
 * \brief Tells the select() function which field should be used as a
 * conditional key for searching.
 * \param fld_ enum Fields.
 * \param cmp_ enum Compare.
 * \param t_ Value to compare.
 */
void
SLPQuery::field(Fields fld_, Compare cmp_, Visitor::var_t&& t_)
{
  if (info_t.flag_) {
    uint32_t min_ = std::move(info_t.begin_date_);
    uint32_t max_ = std::move(info_t.end_date_);
    uint32_t ip0_ = std::move(info_t.begin_ip_);
    uint32_t ip1_ = std::move(info_t.end_ip_);

    auto procRegex_ = [*this, &min_, &max_, &ip0_, &ip1_, &fld_, &t_](
                        const std::pair<DataKey, DataSet_Squid>& it_) {
      if ((it_.first.getTs() >= min_) && (it_.first.getTs() <= max_) &&
          (it_.first.getIp() >= ip0_) && (it_.first.getIp() <= ip1_)) {
        Visitor::TypeVar tv_ = varType(t_);
        if (tv_ == TypeVar::TString) {
          try {
            boost::regex re_(std::get<std::string>(t_).c_str());
            boost::smatch smatch_res_;
            std::string s_ = strFields(fld_, it_.second);
            if (boost::regex_search(s_, smatch_res_, re_)) {
              return true;
            }

          } catch (boost::regex_error& e_) {
            std::cout << "SLPQuery::fields() regex error: " << getErrorRE(e_)
                      << '\n';
          }
        }
      }
      return false;
    };

    auto procTypes_ = [*this, &min_, &max_, &ip0_, &ip1_, &fld_, &cmp_, &t_](
                        const std::pair<DataKey, DataSet_Squid>& it_) {
      if ((it_.first.getTs() >= min_) && (it_.first.getTs() <= max_) &&
          (it_.first.getIp() >= ip0_) && (it_.first.getIp() <= ip1_)) {
        Visitor::TypeVar tv_ = varType(t_);
        switch (tv_) {
          case TypeVar::TInt: {
            return decision(
              intFields(fld_, it_.second), std::get<int>(t_), cmp_);
            break;
          }
          case TypeVar::TUint: {
            return decision(
              uint32Fields(fld_, it_.second), std::get<uint32_t>(t_), cmp_);
            break;
          }
          case TypeVar::TString: {
            return decision(
              strFields(fld_, it_.second), std::get<std::string>(t_), cmp_);
          }
        } // switch
      }
      return false;
    };

    auto cbegin_ = std::make_move_iterator(mEntry.cbegin());
    auto cend_ = std::make_move_iterator(mEntry.cend());

    if (cmp_ == Compare::REGEX) {
      std::copy_if(cbegin_,
                   cend_,
                   std::inserter(mSubset_, std::begin(mSubset_)),
                   procRegex_);
    } else {
      std::copy_if(cbegin_,
                   cend_,
                   std::inserter(mSubset_, std::begin(mSubset_)),
                   procTypes_);
    }
  }
}

/*!
 * \brief SLPQuery::getInt
 * \param ts_ String Timestamp.
 * \param ip_ String IP address.
 * \param fld_ enum Fields value.
 * \return std::vector<int>
 */
std::vector<int>
SLPQuery::getInt(const std::string&& ts_,
                 const std::string&& ip_,
                 Fields fld_) const
{
  std::vector<int> v_ = {};
  DataKey dk_(unixTimestamp(ts_), IPv4Addr::iptol(ip_));

  std::for_each(
    mSubset_.cbegin(),
    mSubset_.cend(),
    [this, &fld_, &dk_, &v_](const std::pair<DataKey, DataSet_Squid>& it_) {
      if ((it_.first.getTs() == dk_.getTs()) &&
          (it_.first.getIp() == dk_.getIp())) {
        v_.push_back(std::move(intFields(fld_, it_.second)));
        return true;
      }
      return false;
    });
  return v_;
}

/*!
 * \brief SLPQuery::getUInt
 * \param ts_ String Timestamp.
 * \param ip_ String IP address.
 * \param fld_ enum Fields value.
 * \return std::vector<int>
 */
std::vector<uint32_t>
SLPQuery::getUInt(const std::string&& ts_,
                  const std::string&& ip_,
                  Fields fld_) const
{
  std::vector<uint32_t> v_ = {};
  DataKey dk_(unixTimestamp(ts_), IPv4Addr::iptol(ip_));

  std::for_each(
    mSubset_.cbegin(),
    mSubset_.cend(),
    [this, &fld_, &dk_, &v_](const std::pair<DataKey, DataSet_Squid>& it_) {
      if ((it_.first.getTs() == dk_.getTs()) &&
          (it_.first.getIp() == dk_.getIp())) {
        v_.push_back(std::move(uint32Fields(fld_, it_.second)));
        return true;
      }
      return false;
    });
  return v_;
}

/*!
 * \brief SLPQuery::getStr
 * \param ts_ String Timestamp.
 * \param ip_ String IP address.
 * \param fld_ enum Fields value.
 * \return std::vector<int>
 */
std::vector<std::string>
SLPQuery::getStr(const std::string&& ts_,
                 const std::string&& ip_,
                 Fields fld_) const
{
  std::vector<std::string> v_ = {};
  DataKey dk_(unixTimestamp(ts_), IPv4Addr::iptol(ip_));

  std::for_each(
    mSubset_.cbegin(),
    mSubset_.cend(),
    [this, &fld_, &dk_, &v_](const std::pair<DataKey, DataSet_Squid>& it_) {
      if ((it_.first.getTs() == dk_.getTs()) &&
          (it_.first.getIp() == dk_.getIp())) {
        v_.push_back(strFields(fld_, it_.second));
        return true;
      }
      return false;
    });
  return v_;
}

/*!
 * \brief Returns the sum of the values of the "Total Size Reply" field.
 * \return long
 */
long
SLPQuery::sumTotalSizeReply() const
{
  return std::accumulate(
    mSubset_.cbegin(),
    mSubset_.cend(),
    0,
    [](long sum_, const std::pair<DataKey, DataSet_Squid>& d_) {
      return sum_ + d_.second.totalSizeReply;
    });
}

/*!
 * \brief Returns the sum of the values of the "Response Time" field in
 * milliseconds.
 * \return long
 */
long
SLPQuery::sumResponseTime() const
{
  return std::accumulate(
    mSubset_.cbegin(),
    mSubset_.cend(),
    0,
    [](long sum_, const std::pair<DataKey, DataSet_Squid>& d_) {
      return sum_ + d_.second.responseTime;
    });
}

/*!
 * \brief Returns a data structure containing the individual count of Request
 * Methods types.
 *
 * \return ReqMethods_t Public structure
 */
SLPQuery::accReqMethods_t
SLPQuery::countByReqMethod() const
{

  accReqMethods_t rm_t_ = {};

  std::for_each(
    mSubset_.cbegin(),
    mSubset_.cend(),
    [this, &rm_t_](const std::pair<DataKey, DataSet_Squid>& d_) {
      if (d_.second.reqMethod == MethodText(MethodType::MTGet)) {
        ++rm_t_.Get;
      } else if (d_.second.reqMethod == MethodText(MethodType::MTPut)) {
        ++rm_t_.Put;
      } else if (d_.second.reqMethod == MethodText(MethodType::MTPost)) {
        ++rm_t_.Post;
      } else if (d_.second.reqMethod == MethodText(MethodType::MTConnect)) {
        ++rm_t_.Connect;
      } else if (d_.second.reqMethod == MethodText(MethodType::MTHead)) {
        ++rm_t_.Head;
      } else if (d_.second.reqMethod == MethodText(MethodType::MTDelete)) {
        ++rm_t_.Delete;
      } else if (d_.second.reqMethod == MethodText(MethodType::MTOptions)) {
        ++rm_t_.Options;
      } else if (d_.second.reqMethod == MethodText(MethodType::MTPatch)) {
        ++rm_t_.Patch;
      } else if (d_.second.reqMethod == MethodText(MethodType::MTTrace)) {
        ++rm_t_.Trace;
      } else {
        ++rm_t_.Others;
      }
    });
  return rm_t_;
}

/*!
 * \brief processes the count by Http Request Code. If no value is informed, it
 * will process all possible codes, otherwise only the informed code.
 *
 * \param code A valid HRC code.
 *
 * \note The result of the process is stored in the map that is already
 * populated with the codes found in the log file named HttpCodesUniques_m.
 *
 * \note UserAgent and Referrer formats don't have the Http Status field, so
 * HttpCodesUniques_m will always be zero.
 */
void
SLPQuery::countByHttpCodes(const short&& code_)
{
  if (HttpCodesUniques_m.size() > 0) {
    // set scores to zero.
    for (const auto& a : HttpCodesUniques_m) {
      HttpCodesUniques_m.insert_or_assign(a.first, 0);
    }

    std::for_each(mSubset_.cbegin(),
                  mSubset_.cend(),
                  [&code_, this](const std::pair<DataKey, DataSet_Squid>& d_) {
                    short c_ = 0;
                    switch (logFmt_) {
                      case LogFormat::Common:
                        [[fallthrough]];
                      case LogFormat::Combined: {
                        c_ = std::move(d_.second.httpStatus);
                        break;
                      }
                      default: {
                        c_ = std::move(std::stoi(
                          strRight(d_.second.reqStatusHierStatus, '/')));
                      }
                    }

                    if (code_ == 0) { // all
                      if (const auto& it_ = HttpCodesUniques_m.find(c_);
                          it_ != HttpCodesUniques_m.end()) {
                        HttpCodesUniques_m[c_] += 1;
                      }
                    } else if (c_ == code_) {
                      if (const auto& it_ = HttpCodesUniques_m.find(code_);
                          it_ != HttpCodesUniques_m.end()) {
                        HttpCodesUniques_m[code_] += 1;
                      }
                    }
                  });
  }
}

/*!
 * \brief Returns the Http Request Code description and the count of
 * occurrences in the log.
 *
 * \param code Http Request Code.
 * \return std::pair<int, std::string> As follows:
 *         'first' is total score and 'second' is code description
 *
 * \code
 *
 * (...)
 * slpquery->countByHttpCodes(); // General count
 *
 * std::cout << "Description: " slpquery->getHRCScore(409).second
 *           << " Score:  " << slpquery->getHRCScore(409).first << "\n";
 *
 * // Possible output
 * // Description: Conflict Score: 150
 * \endcode
 *
 */
std::pair<int, std::string>
SLPQuery::getHRCScore(const short&& code_)
{
  if (HttpCodesUniques_m.size() > 0) {
    if (code_ > 0) {
      std::pair<int, std::string> result_;
      if (const auto& it_ = HttpCodesUniques_m.find(code_);
          it_ != HttpCodesUniques_m.end()) {
        result_.first = it_->second;
      } else {
        return { 0, "Unknown" };
      }

      if (const auto& it_ = HttpCodesText_m.find(code_);
          it_ != HttpCodesText_m.end()) {
        result_.second = it_->second;
      }

      return result_;
    } else {
      return { 0, "Unknown" };
    }
  }
  return {}; // void pair
}

/*!
 * \brief SLPQuery::getHRCDetails
 * \return
 */
SLPQuery::HttpRequestCodes_V
SLPQuery::getHRCDetails()
{
  HttpRequestCodes_V hrc_v_;
  std::for_each(HttpCodesUniques_m.cbegin(),
                HttpCodesUniques_m.cend(),
                [&hrc_v_, this](const std::pair<short, int>& d_) {
                  std::string descr_;
                  if (const auto& it_ = HttpCodesText_m.find(d_.first);
                      it_ != HttpCodesText_m.end()) {
                    descr_ = it_->second;
                  } else {
                    descr_ = "Unknown"; // for unofficial codes or error
                  }
                  hrc_v_.push_back(HRCData(d_.first, descr_, d_.second));
                });

  return hrc_v_;
}

/*!
 * \brief Given an enumerator as an argument, returns the corresponding text.
 * \param mt_ Enumerator
 * \return string Text
 */
inline std::string
SLPQuery::MethodText(MethodType mt_) const
{
  return MethodText_t[static_cast<int>(mt_)].sv_.data();
}

/*!
 * \brief Returns the size of mSubset.
 * \return size_t
 */
size_t
SLPQuery::size() const
{
  return mSubset_.size();
}

/*!
 * \brief Clear all values mSubset.
 */
void
SLPQuery::clear()
{
  mSubset_.clear();
}

/* SLPUrlParts--------------------------------------------------------------
 */
/*!
 * \brief Parses URLs (http[s]) the log line and returns the parts inside the
 * UrlAnatomy_t structure.
 *
 * \param rawUrl_
 */
SLPUrlParts::SLPUrlParts(const std::string rawUrl_)
  : raw_url_(rawUrl_)
{
  parseUrl();
}

/*!
 * \brief SLPUrlParts::getProtocol
 * \return string
 */
std::string
SLPUrlParts::getProtocol() const
{
  return url_t.protocol_;
}

/*!
 * \brief SLPUrlParts::getDomain
 * \return string
 */
std::string
SLPUrlParts::getDomain() const
{
  return url_t.domain_;
}

/*!
 * \brief SLPUrlParts::getPath
 * \return string
 */
std::string
SLPUrlParts::getPath() const
{
  return url_t.path_;
}

/*!
 * \brief SLPUrlParts::getQuery
 * \return string
 */
std::string
SLPUrlParts::getQuery() const
{
  return url_t.query_;
}

/*!
 * \brief SLPUrlParts::getFragment
 * \return string
 */
std::string
SLPUrlParts::getFragment() const
{
  return url_t.fragment_;
}

/*!
 * \private
 * \brief Parses URLs (http[s]) the log line.
 *
 */
void
SLPUrlParts::parseUrl()
{
  if (!raw_url_.empty() ||
      std::string_view{ raw_url_ }.find("://") == std::string::npos) {
    url_t = {};

    std::string work_url_;
    work_url_.reserve(raw_url_.size());
    work_url_ = raw_url_;

    size_t p_frag_ = std::string_view{ work_url_ }.find("#");
    if (p_frag_ != std::string::npos) {
      url_t.fragment_ =
        std::string_view{ work_url_ }.substr(p_frag_, raw_url_.size());
      work_url_ = std::string_view{ work_url_ }.substr(0, p_frag_);
    }

    size_t p_query_ = std::string_view{ work_url_ }.find("?");
    if (p_query_ != std::string::npos) {
      url_t.query_ =
        std::string_view{ work_url_ }.substr(p_query_, work_url_.size());
      work_url_ = std::string_view{ work_url_ }.substr(0, p_query_);
    }

    size_t p_protocol_ = std::string_view{ work_url_ }.find("://");
    size_t p_slash_;
    if (p_protocol_ != std::string::npos) {
      url_t.protocol_ = std::string_view{ work_url_ }.substr(0, p_protocol_);
      work_url_ =
        std::string_view{ work_url_ }.substr(p_protocol_ + 3, work_url_.size());

      // get domain
      p_slash_ = std::string_view{ work_url_ }.find("/");
      if (p_slash_ != std::string::npos) {
        url_t.domain_ = std::string_view{ work_url_ }.substr(0, p_slash_);
      }

      if (p_slash_ < work_url_.size()) {
        url_t.path_ =
          std::string_view{ work_url_ }.substr(p_slash_, work_url_.size());
      }
    }
  }
}

/* SPLRawToXML--------------------------------------------------------------
 */
/*!
 * \internal
 * \brief Constructs a PFRawToXML object.
 * \param log_data_t
 */
SLPRawToXML::SLPRawToXML(LogFormat fmt_, size_t count_)
  : slpError_(SLPError::SLP_SUCCESS)
  , logFmt_(std::move(fmt_))
  , cnt_(std::move(count_))
{}

/*!
 * \internal
 * \brief Appends the raw_ log entry onto the end of XML file.
 * \param log_data_t
 */
SLPRawToXML&
SLPRawToXML::append(const DataSet_Squid& ds_)
{
  ds_squid_ = std::move(ds_);
  writePart();
  return *this;
}

/*!
 * \internal
 * \brief Save the raw log entry to an XML file. It should only be used
 * for a single entry.
 * \param fn_ Filename.
 * \return SLPError code.
 */
SquidLogParser::SLPError
SLPRawToXML::save(const std::string fn_)
{
  constexpr const auto whatFormat = [](LogFormat f_) {
    switch (f_) {
      case LogFormat::Squid:
        return "Squid";
      case LogFormat::Common:
        return "Common";
      case LogFormat::Combined:
        return "Combined";
      case LogFormat::Referrer:
        return "Referrer";
      case LogFormat::UserAgent:
        return "UserAgent";
      default:
        return "None";
    }
  };

  std::string tmp_ = std::move(fn_);
  SLPError err = normFn(tmp_);
  if (err == SLPError::SLP_SUCCESS) {
    fname_ = tmp_;

    decl = doc.NewDeclaration();
    doc.LinkEndChild(decl);

    root = doc.NewElement("SquidLogParser");
    doc.LinkEndChild(root);

    XMLElement* hdr_ = doc.NewElement("generated");
    hdr_->SetText("libsquidlogparser");
    root->InsertFirstChild(hdr_);

    hdr_ = doc.NewElement("created");
    hdr_->SetText(localTime().c_str());
    root->InsertEndChild(hdr_);

    hdr_ = doc.NewElement("filename");
    hdr_->SetText(fname_.c_str());
    root->InsertEndChild(hdr_);

    hdr_ = doc.NewElement("total_entries");
    hdr_->SetText(static_cast<unsigned int>(cnt_));
    root->InsertEndChild(hdr_);

    root_node = doc.NewElement("logformat");
    root_node->SetAttribute("format", whatFormat(logFmt_));
    root->InsertEndChild(root_node);
    return close();
  }
  return slpError_;
}

/*!
 * \internal
 * \brief Save the XML to disk.
 * \return SPLError code.
 */
SquidLogParser::SLPError
SLPRawToXML::close()
{
  return (doc.SaveFile(fname_.c_str()) == XMLError::XML_SUCCESS)
           ? SLPError::SLP_SUCCESS
           : SLPError::SLP_ERR_XML_FILE_NOT_SAVE;
}

/* private------------------------------------------------------------------
 */
/*!
 * \internal
 * \brief Auxiliary member that return a formatted localtime.
 * \return Formatted string with a date and time.
 */
std::string
SLPRawToXML::localTime() const
{
  std::time_t now_t =
    std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char tm_buf[20] = {};
  std::strftime(tm_buf, sizeof(tm_buf), "%F;%T", std::localtime(&now_t));
  return tm_buf;
}

void
SLPRawToXML::writePart()
{
  XMLElement* elem0 = doc.NewElement("entry");
  root_node->InsertEndChild(elem0);

  switch (logFmt_) {
    case LogFormat::Squid: {
      XMLElement* data = doc.NewElement("timestamp");
      data->SetText(ds_squid_.timeStamp);
      elem0->InsertEndChild(data);
      data = doc.NewElement("responsetime");
      data->SetText(ds_squid_.responseTime);
      elem0->InsertEndChild(data);
      data = doc.NewElement("clisrcipaddr");
      data->SetText(IPv4Addr::ltoip(ds_squid_.cliSrcIpAddr).c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("reqstatushierstatus");
      data->SetText(ds_squid_.reqStatusHierStatus.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("totalsizereply");
      data->SetText(ds_squid_.totalSizeReply);
      elem0->InsertEndChild(data);
      data = doc.NewElement("reqmethod");
      data->SetText(ds_squid_.reqMethod.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("requrl");
      data->SetText(ds_squid_.reqURL.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("username");
      data->SetText(ds_squid_.userName.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("hierstatusipaddress");
      data->SetText(ds_squid_.hierStatusIpAddress.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("mimetypecontent");
      data->SetText(ds_squid_.mimeTypeContent.c_str());
      elem0->InsertEndChild(data);
      break;
    }
    case LogFormat::Common:
      [[fallthrough]];
    case LogFormat::Combined: {
      XMLElement* data = doc.NewElement("clisrcpipaddr");
      data->SetText(IPv4Addr::ltoip(ds_squid_.cliSrcIpAddr).c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("usernamefromident");
      data->SetText(ds_squid_.userNameIdent.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("username");
      data->SetText(ds_squid_.userName.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("localtime");
      data->SetText(ds_squid_.localTime.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("reqmethod");
      data->SetText(ds_squid_.reqMethod.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("requrl");
      data->SetText(ds_squid_.reqURL.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("reqprotoversion");
      data->SetText(ds_squid_.reqProtoVersion.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("httpstatus");
      data->SetText(ds_squid_.httpStatus);
      elem0->InsertEndChild(data);
      data = doc.NewElement("totalsizereply");
      data->SetText(ds_squid_.totalSizeReply);
      elem0->InsertEndChild(data);
      if (logFmt_ == LogFormat::Combined) {
        data = doc.NewElement("referrer");
        data->SetText(ds_squid_.referrer.c_str());
        elem0->InsertEndChild(data);
        data = doc.NewElement("useragent");
        data->SetText(ds_squid_.userAgent.c_str());
        elem0->InsertEndChild(data);
      }
      data = doc.NewElement("reqstatushierstatus");
      data->SetText(ds_squid_.reqStatusHierStatus.c_str());
      elem0->InsertEndChild(data);
      break;
    }
    case LogFormat::Referrer: {
      XMLElement* data = doc.NewElement("timestamp");
      data->SetText(ds_squid_.timeStamp);
      elem0->InsertEndChild(data);
      data = doc.NewElement("clisrcpipaddr");
      data->SetText(IPv4Addr::ltoip(ds_squid_.cliSrcIpAddr).c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("referrer");
      data->SetText(ds_squid_.referrer.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("requrl");
      data->SetText(ds_squid_.reqURL.c_str());
      elem0->InsertEndChild(data);
      break;
    }
    case LogFormat::UserAgent: {
      XMLElement* data = doc.NewElement("clisrcpipaddr");
      data->SetText(IPv4Addr::ltoip(ds_squid_.cliSrcIpAddr).c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("localtime");
      data->SetText(ds_squid_.localTime.c_str());
      elem0->InsertEndChild(data);
      data = doc.NewElement("useragent");
      data->SetText(ds_squid_.userAgent.c_str());
      elem0->InsertEndChild(data);
    }
    default: {
      // do nothing
    }
  }
}

/*!
 * \internal
 * \brief Auxiliary member that provides the '.xml' extension to the
 * filename if the user does not inform.
 * \param fn_ File name
 * \return PFLError
 * \return Normalized file name by reference name with .xml extension.
 * --------------------------------------------------------------------------
 * \note 1. Basically for a name to be considered inconsistent, it must have
 * more than one dot '.' in its formation.
 * \note 2. If there're spaces in the filename they will be replaced by '_'.
 * \note 3. The file name informed is converted to lowercase.
 */
SquidLogData::SLPError
SLPRawToXML::normFn(std::string& fn_) const
{
  std::string fn_s = fn_;
  int c = 0;
  for (const auto& a : fn_) {
    if (a == '.') {
      ++c;
    }
  }

  if (c <= 1) {
    std::replace(fn_.begin(), fn_.end(), ' ', '_');
    std::transform(fn_.cbegin(), fn_.cend(), fn_.begin(), ::tolower);
    if (size_t f = std::string_view{ fn_ }.rfind("."); f != std::string::npos) {
      if (std::string s_ = fn_.substr(f + 1, fn_.size()); s_ != "xml") {
        fn_.replace(f + 1, fn_.size(), "xml");
      }
      return SLPError::SLP_SUCCESS;
    } else {
      fn_ += ".xml";
      return SLPError::SLP_SUCCESS;
    }
  }

  return SLPError::SLP_ERR_XML_FILE_NAME_INCONSISTENT;
};

} // namespace squidlogparser
