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
#include <memory>
#include <string>

#include <squidlogparser.h>     // mandatory
using namespace squidlogparser; // mandatory

/*
 * Dependences:
 *
 * MariaDB-connector C++ 1.0.1 or higher.
 *
 * Compile:
 * g++ -Wall -O2 -std=c++17 -DDATABASE_EXTENSION  -lsquidlogparser -lmariadbcpp
 * -pthread ex-insert-01.cc -o ex-insert-01
 *
 * Usage stream: cat access.log | ./ex-insert-01
 *
 */

int
main(int argc, char** argv)
{
  std::string dbname_ = "test";    // Database name
  std::string host_ = "localhost"; // Host name or IP address
  int port_ = 0;                   // 0 to use default port 3306
  std::string user_ = "dbuser";    // Your user name
  std::string passwd_ = "dbpass";   // Your password

  /*
   * Name of the table to be created or used to insert the data.
   * If no name is given a default name based on log format will be used.
   *
   * Format                               Dflt Table name
   * ------------------------------------ ---------------
   * SquidLogParser::LogFormat::Squid     slp_log_squid
   * SquidLogParser::LogFormat::Common    slp_log_common
   * SquidLogParser::LogFormat::Combined  slp_log_combined
   * SquidLogParser::LogFormat::Referrer  slp_log_referrer
   * SquidLogParser::LogFormat::UserAgent slp_log_useragent
   *
   */
  std::string tbname_ = "test_squid";

  // Since we are dealing with database connections, a little more security is
  // good.
  std::unique_ptr<SLPDatabase> dbe_ =
    std::make_unique<SLPDatabase>(SquidLogParser::LogFormat::Squid,
                                  dbname_,
                                  host_,
                                  port_,
                                  user_,
                                  passwd_,
                                  tbname_);

  if (dbe_->errorNum() != SLPDatabase::DBError::DBE_SUCCESS) {
    std::cout << ">>> " << dbe_->getErrorText() << "\n";
    dbe_.reset();
    exit(EXIT_FAILURE);
  }

  std::string raw_log = {};
  while (std::getline(std::cin, raw_log)) {
    dbe_->insert(raw_log);
    if (dbe_->errorNum() != SLPDatabase::DBError::DBE_SUCCESS) {
      std::cout << ">>> " << dbe_->getErrorText() << "\n";
      dbe_.reset();
      exit(EXIT_FAILURE);
    }
  }

  std::cout << "ROWS INSERTED = " << dbe_->getRowsInserted() << "\n";
  dbe_.reset();

  return EXIT_SUCCESS;
}

#endif // EXAMPLE_H
