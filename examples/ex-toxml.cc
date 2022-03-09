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
#endif

#include <squidlogparser.h>     // mandatory
using namespace squidlogparser; // mandatory

/*
 * Compile:
 * g++ -Wall -O2 -std=c++17 -lsquidlogparser -pthread ex-toxml.cc -o ex-toxml
 *
 * Usage stream: cat access.log | ./ex-toxml
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

  // Warning: all data in memory is erased after this operation.
  std::cout << "Converting ... \n";

  SLPError err;

  // All log entries:
  err = p->toXML("ACCESS.xml",
                 "23/Feb/2022:00:00:00",
                 "192.168.1.214",
                 "23/Feb/2022:23:59:59",
                 "192.168.1.214");
  // err = p->toXML("ACCESS.xml"); // All
  if (err != SquidLogParser::SLPError::SLP_SUCCESS) {
    std::cout << static_cast<int>(p->errorNum()) << " :  " << p->getErrorText()
              << "\n";
    exit(255);
  }

#endif

  return 0;
}
