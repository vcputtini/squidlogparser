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
#include <vector>
#endif

#include <squidlogparser.h>     // mandatory
using namespace squidlogparser; // mandatory

/*
 * Compile:
 * g++ -Wall -O2 -std=c++17 -lsquidlogparser -pthread ex-query-useragent.cc -o
 * ex-query-ua
 *
 * Usage stream: cat access.log | ./ex-query-ua
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

  SquidLogParser::SLPError err;
  SquidLogParser* p = new SquidLogParser(SquidLogParser::LogFormat::UserAgent);
  while (std::getline(std::cin, raw_log_)) {
    p->append(raw_log_);
    if (p->errorNum() != SquidLogParser::SLPError::SLP_SUCCESS) {
      std::cout << ">>> " << p->getErrorText() << "\n";
      exit(255);
    }
  }

  std::cout << "UserAgent load = " << p->size() << "\n";

  /*!
   * When we inform only the two first arguments to the select(begin_timestamp,
   * begin_ip) function, it will assume that the end date and the end ip are the
   * same, thus selecting only the objects that match.
   *
   * obj->select(begin_timestamp, being_ip_address [,
   *             end_timestamp, end_ip_address]
   *             .field(Fields, Comparition, Value);
   *
   */

  SLPQuery* qry = new SLPQuery(p);
  qry
    ->select("01/Feb/2022:00:00:00",
             "192.168.1.1",
             "31/Mar/2022:23:59:59",
             "192.168.1.107")
    .field(SquidLogParser::Fields::CliSrcIpAddr,
           SquidLogParser::Compare::EQ,
           "192.168.1.20");
  if (qry->errorNum() != SquidLogParser::SLPError::SLP_SUCCESS) {
    std::cout << "\n0>>> " << qry->getErrorText() << "\n";
  } else {
    std::cout << "\n0>>> " << qry->getErrorText() << "\n";
  }

  std::cout << "QRY SIZE = " << qry->size() << "\n";

  std::vector<std::string> v_ = qry->getStr(
    "23/Feb/2022:10:22:22", "192.168.1.20", SquidLogParser::Fields::UserAgent);
  std::cout << "getStr = " << v_.size() << "\n";
  for (size_t i = 0; i < v_.size(); ++i) {
    std::cout << "v = " << i << "  " << v_[i] << "\n";
  }

  delete qry;

  // Using RE
  SLPQuery* qry1 = new SLPQuery(p);
  qry1
    ->select("01/Feb/2022:00:00:00",
             "192.168.1.1",
             "31/Mar/2022:23:59:59",
             "192.168.1.107")
    .field(
      SquidLogParser::Fields::UserAgent, SquidLogParser::Compare::REGEX, "-");

  std::cout << "QRY1 SIZE = " << qry1->size() << "\n";

  std::tuple<std::vector<std::string>,
             std::vector<std::string>,
             std::vector<std::string>>
    vt1_ = std::make_tuple(qry1->getStr("23/Feb/2022:10:22:22",
                                        "192.168.1.20",
                                        SquidLogParser::Fields::CliSrcIpAddr),
                           qry1->getStr("23/Feb/2022:10:22:22",
                                        "192.168.1.20",
                                        SquidLogParser::Fields::Timestamp),
                           qry1->getStr("23/Feb/2022:10:22:22",
                                        "192.168.1.20",
                                        SquidLogParser::Fields::UserAgent));

  for (size_t i = 0; i < std::get<0>(vt1_).size(); ++i) {
    std::cout << i << " > " << std::get<0>(vt1_)[i] << " : "
              << std::get<1>(vt1_)[i] << " : " << std::get<2>(vt1_)[i] << "\n";
  }

  delete qry1;

  /*!
   * \brief Possible output
   * 
   * serAgent load = 1007
   * 0>>> Success!
   * QRY SIZE = 891
   * getStr = 10
   * v = 0  -
   * v = 1  -
   * v = 2  Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:97.0) Gecko/20100101 Firefox/97.0
   * v = 3  Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:97.0) Gecko/20100101 Firefox/97.0
   * v = 4  Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:97.0) Gecko/20100101 Firefox/97.0 
   * v = 5  Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:97.0) Gecko/20100101 Firefox/97.0 
   * v = 6  - 
   * v = 7 Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:97.0) Gecko/20100101 Firefox/97.0 
   * v = 8 Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:97.0) Gecko/20100101 Firefox/97.0
   * v = 9  - 
   * 
   * QRY1 SIZE = 304
   * 0 > 192.168.1.20 : 31/Dec/1969:21:00:00 -0300 : - 
   * 1 > 192.168.1.20 : 31/Dec/1969:21:00:00 -0300 : - 
   * 2 > 192.168.1.20 : 31/Dec/1969:21:00:00 -0300 : - 
   * 3 > 192.168.1.20 : 31/Dec/1969:21:00:00 -0300 : -
   */

#endif
}
