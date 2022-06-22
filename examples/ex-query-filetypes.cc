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
 * g++ -Wall -O2 -std=c++17 -lsquidlogparser -pthread ex-query-filetypes.cc -o
 * ex-query-ftype
 *
 * Usage stream: cat access.log | ./ex-query-ftype
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
      exit(EXIT_FAILURE);
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
             "192.168.1.1",
             "31/Mar/2022:23:59:59",
             "192.168.1.107")
    .field(
      SquidLogParser::Fields::TotalSizeReply, SquidLogParser::Compare::GT, 0);

  if (qry->errorNum() != SquidLogParser::SLPError::SLP_SUCCESS) {
    std::cout << "\n0>>> " << qry->getErrorText() << "\n";
  } else {
    std::cout << "\n0>>> " << qry->getErrorText() << "\n";
  }

  /*!
   * \brief Calculates the score for all file types found
   *
   * If the extension_ argument is not informed, all types are calculated,
   * otherwise only the informed type is processed.
   *
   * * \warning Please note that the period must be included together with the
   * extension.
   *
   * int countByFiletype(const std::string&& extension_)
   *
   */
  const int total = qry->countByFiletype();

  /*
   * Filetypes_V is a user-defined type that represents the vector that
   * contains the fields: Description and Score.
   * The field values can be returned using the functions: getDescription()
   * and getScore().
   *
   * The getFTDetails() function returns a vector identified by the
   * Filetypes_V type and can be accessed by the
   * same means as a common std::vector(), see the examples below:
   */
  SLPQuery::Filetypes_V fsvec_;
  std::cout << "Total extensions = " << qry->countByFiletype() << "\n\n";
  std::cout << "Total files  = " << qry->totalFiles() << "\n\n";
  fsvec_ = qry->getFTDetails();
  for (const auto& it_ : fsvec_) {
    std::cout << it_.getDescription() << " : " << it_.getScore() << "\n";
  }

  // Returns the position (index) of the file type within vector.
  std::cout << "Index = " << qry->getIndexByFiletype(".html") << "\n";

  const auto idx_ = qry->getIndexByFiletype(".html");
  std::cout << "\n\nindividual" << fsvec_[idx_].getDescription() << " : "
            << fsvec_[idx_].getScore() << "\n";

            
  // Report
  // Example result in examples/summary.rpt
  std::cout << "Total extensions = " << qry->countByFiletype() << "\n";
  const int total_ = qry->totalFiles();
  std::cout << "Total files  = " << total_ << "\n";
  std::cout << "----------|----------|-----------\n";
  std::cout << "Types     | Total    | % \n";
  std::cout << "----------|----------|-----------\n";
  for (size_t i = 0; i != fsvec_.size(); ++i) {
    std::cout.width(10);
    std::cout << std::left << fsvec_[i].getDescription() << "|";
    std::cout.width(10);
    std::cout << std::right << fsvec_[i].getScore() << "|";
    std::cout.width(10);
    std::cout << std::right << std::setprecision(4) << std::fixed
              << ((static_cast<float>(fsvec_[i].getScore()) /
                   static_cast<float>(total_)) *
                  100)
              << "\n";
  }
  std::cout << "---------------------------------\n";
  
  exit(EXIT_SUCCESS);
  
}

#endif
