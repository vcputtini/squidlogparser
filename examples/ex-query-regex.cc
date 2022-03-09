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
#include <tuple>
#endif

#include <squidlogparser.h>     // mandatory
using namespace squidlogparser; // mandatory

/*
 * Compile:
 * g++ -Wall -O2 -std=c++17 -lsquidlogparser -pthread ex-query-regex.cc -o
 * ex-query-regex
 *
 * Usage stream: cat access.log | ./ex-query-regex
 *
 */

int
main()
{
  using LogFormat = SquidLogParser::LogFormat;
  using SLPError = SquidLogParser::SLPError;

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
  std::cout << "Load Size == " << p->size() << "\n";

  /*
   * When we inform only the two first arguments to the select(begin_timestamp,
   * begin_ip) function, it will assume that the end date and the end ip are the
   * same, thus selecting only the objects that match.
   *
   * obj->select(begin_timestamp, being_ip_address [,
   *             end_timestamp, end_ip_address]
   *             .field(Fields, Comparition, Value);
   *
   */

  std::cout << "\nREGEX\n";
  SLPQuery* qry = new SLPQuery(p);

  /*
   *
   * The use of regex is only applicable for String-type fields at this point.
   *
   * Regular expressions must follow the C/C++ format of text with escaped
   * characters.
   * e.g.: "(\\S+)"
   * e.g.: "(\\S+) \"(\\d{1})\""
   */
  qry
    ->select("01/Feb/2022:00:00:00",
             "172.50.1.1",
             "28/Feb/2022:23:59:59",
             "172.50.1.100")
    .field(
      SquidLogParser::Fields::ReqURL, SquidLogParser::Compare::REGEX, "cab$");
  if (qry->errorNum() != SquidLogParser::SLPError::SLP_SUCCESS) {
    std::cout << "\n0>>> " << qry->getErrorText() << "\n";
  } else {
    std::cout << "\n0>>> " << qry->getErrorText() << "\n";
  }

  std::cout << "regex size = " << qry->size() << '\n';
  std::tuple<std::vector<int>,
             std::vector<std::string>,
             std::vector<std::string>>
    vt1_ = std::make_tuple(qry->getInt("03/Feb/2022:11:39:34",
                                       "172.50.1.28",
                                       SquidLogParser::Fields::TotalSizeReply),
                           qry->getStr("03/Feb/2022:11:39:34",
                                       "172.50.1.28",
                                       SquidLogParser::Fields::CliSrcIpAddr),
                           qr1->getStr("03/Feb/2022:11:39:34",
                                       "172.50.1.28",
                                       SquidLogParser::Fields::Timestamp));

  for (size_t i = 0; i < std::get<0>(vt1_).size(); ++i) {
    std::cout << i << " > " << std::get<0>(vt1_)[i] << " : "
              << std::get<1>(vt1_)[i] << " : " << std::get<2>(vt1_)[i] << "\n";
  }
  std::cout << "Sum total size reply   = " << qry->sumTotalSizeReply() << "\n";
  std::cout << "Sum Response Time (ms) = " << qry->sumResponseTime() << "\n";

  
  std::cout << "GET\t= " << qry->countByReqMethod().Get << "\n";
  std::cout << "PUT\t= " << qry->countByReqMethod().Put << "\n";
  std::cout << "POST\t= " << qry->countByReqMethod().Post << "\n";
  std::cout << "CONNECT\t= " << qry->countByReqMethod().Connect << "\n";
  std::cout << "HEAD\t= " << qry->countByReqMethod().Head << "\n";
  std::cout << "DELETE\t= " << qry->countByReqMethod().Delete << "\n";
  std::cout << "OPTIONS\t= " << qry->countByReqMethod().Options << "\n";
  std::cout << "PATCH\t= " << qry->countByReqMethod().Patch << "\n";
  std::cout << "TRACE\t= " << qry->countByReqMethod().Trace << "\n";
  std::cout << "OTHERS\t= " << qry->countByReqMethod().Others << "\n";
  
  
  delete qry;
  delete p;

  return 0;
}
