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

#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <iostream>
#include <string>

#include <squidlogparser.h>     // mandatory
using namespace squidlogparser; // mandatory

/*!
 * Compile:
 * g++ -Wall -O2 -std=c++17 -lsquidlogparser -pthread ex-query.cc -o ex-query
 *
 * Usage stream: cat access.log | ./ex-query
 *
 */

#define STREAM

int
main()
{
  using LogFormat = SquidLogParser::LogFormat;
  using SLPError = SquidLogParser::SLPError;

#ifdef STREAM
  std::string raw_log_ = {};

  // Loads all log entries from the selected file.
  SquidLogParser* p = new SquidLogParser(LogFormat::Squid);
  while (std::getline(std::cin, raw_log_)) {
    p->append(raw_log_);
    if (p->errorNum() != SLPError::SLP_SUCCESS) {
      std::cout << "Error: " << p->getErrorText() << "\n";
      exit(255);
    }
  }
  // Returns the total number of valid entries read.
  std::cout << "Size == " << p->size() << "\n";

  /*!
   * \brief When we inform only the two first arguments to the
   * select(begin_timestamp, begin_ip) function, it will assume that the end
   * date and the end ip are the same, thus selecting only the objects that
   * match.
   *
   * \code
   * SLPQuery *obj = new SLPQuery( SquidLogParser_Pointer );
   * obj->select(begin_timestamp, being_ip_address [,
   *             end_timestamp, end_ip_address]
   *             .field(Fields, Comparision, Value);
   * \endcode
   *
   */

  SLPQuery* qry = new SLPQuery(p);

  /*!
   * \brief In this example, it selects all records between the dates and IP's
   * informed that have the field Total Size Reply > 0.
   *
   * \note Replace the values below with those from your log file.
   */
  qry
    ->select("01/Feb/2022:00:00:00",
             "192.168.100.1",
             "31/Mar/2022:23:59:59",
             "192.168.100.199")
    .field(
      SquidLogParser::Fields::TotalSizeReply, SquidLogParser::Compare::GT, 0);
  if (qry->errorNum() != SquidLogParser::SLPError::SLP_SUCCESS) {
    std::cout << "\n0>>> " << qry->getErrorText() << "\n";
  } else {
    std::cout << "\n0>>> " << qry->getErrorText() << "\n";
  }

  /*!
   * \brief Calculates the score for all Http Request Codes found
   */
  qry->countByHttpCodes();

  /*!
   * \brief Calculates the score for code 201-Created
   */
  qry->countByHttpCodes(201);

  /*!
   * \brief Gets Code and Score
   * The function getHRCScore(code), returns a std::pair<short, std::string>
   * as follows:
   *
   * std::pair<int, std::string> getHRCScore(const short&& code_ = 0);
   *
   * getHRCScore(code).first is the HTTP code
   * getHRCScore(code).second is the calculated score
   *
   */
  std::cout << qry->getHRCScore(201).first << "  "
            << qry->getHRCScore(201).second << "\n";

  /*
   * HttpRequestCodes_V is a user-defined type that represents the vector that
   * contains the fields: Code, Description and Score.
   * The field values can be returned using the functions: getCode(),
   * getDescription() and getScore().
   *
   * The getHRCDetails() function returns a vector identified by the
   * HttpRequestCodes_V type and can be accessed by the
   * same means as a common std::vector(), see the examples below:
   */
  SLPQuery::HttpRequestCodes_V vec_;
  vec_ = qry->getHRCDetails();
  for (const auto& it_ : vec_) {
    std::cout << it_.getCode() << " : " << it_.getDescription() << " : "
              << it_.getScore() << "\n";
  }

  std::cout << "Total Codes = " << vec_.size() << "\n";

  // access individual element by index.
  std::cout << vec_[10].getCode() << " : " << vec_[10].getDescription() << " : "
            << vec_[10].getScore() << "\n";

  for (size_t i = 0; i <= vec_.size(); ++i) {
    std::cout << vec_[i].getCode() << " : " << vec_[i].getDescription() << " : "
              << vec_[i].getScore() << "\n";
  }

#else
  std::string log_squid =
    "1603310517.212    494 192.168.15.28 TCP_MISS/200 5182 GET "
    "https://mirror.init7.net/fedora/epel/8/Everything/x86_64/repodata/"
    "repomd.xml - ORIGINAL_DST/109.202.202.202";

  std::string log_common =
    "192.168.10.11 - - [05/May/2020:08:28:03 -0300] \"GET "
    "http://ctldl.windowsupdate.com/msdownload/update/v3/static/trustedr/en/"
    "authrootstl.cab? HTTP/1.1\" 304 499 TCP_MISS:ORIGINAL_DST";

  std::string log_combined =
    "192.168.15.10 - - [06/Oct/2021:00:33:25 -0300] \"GET "
    "http://yum.mariadb.org/10.4.1/centos0-amd64/repodata/repomd.xml "
    "HTTP/1.1\" 200 3779 \"-\" \"libdnf (Oracle Linux Server 1.0; server; "
    "Linux.x86_64)\" TCP_CLIENT_REFRESH_MISS:ORIGINAL_DST";

  std::string log_referrer =
    "1596608862.295 192.168.19.65 - "
    "http://xx.yyyy.rrr.zz/R/"
    "O1QKIDVjOTFjYTXxOTQ1MzQzYmViMTc3NzUzMDg4MTRiYzhlEgQIBAggGKwCIgEAKgcIBBCjse"
    "x6OLqYlKABQizzzzzzzzzzzzzzzzzzzzzZ1rqmBFGB-l10EdXP6TiASUiAg5gI";

  std::string log_uagent =
    "192.168.3.99 [15/Sep/2020:01:45:35 -0300] "
    "\"1.0,win/6.1.7601,AV/20.6.3135,avl/push/20.5.5410.0,ffl\"";

  SquidLogParser* p = new SquidLogParser(LogFormat::Squid);
  p->append(log_squid)

    // SquidLogParser *p = new SquidLogParser(LogFormat::Common);
    // p->append( log_common )

    // SquidLogParser *p = new SquidLogParser(LogFormat::Combined);
    // p->append( log_combined )

    // SquidLogParser *p = new SquidLogParser(LogFormat::Referrer);
    // p->append( log_referrer )

    // SquidLogParser *p = new SquidLogParser(LogFormat::UserAgent);
    // p->append( log_uagent )

    // Returns the total number of valid entries read.
    std::cout
    << "Size == " << p->size() << "\n";

#endif
}

#endif
