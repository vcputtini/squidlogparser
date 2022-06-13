## <b>S</b>quid<b>L</b>og<b>P</b>arser
This library is a basic one for analyzing and retrieving information from the fields of the Squid-cache(tm) log entries.<br>
Currently only internally predefined formats are supported: squid; common; combined; referrer and useragent.<br>
This is not a generic library and has not been tested in environments other than the one used in its development.

<b>SquidLogParser</b> was coded entirely in C++.<br>
As I consider QtCreator(tm) to be an excellent development environment,<br>
I use it for my projects, even if these don't directly involve using the Qt(tm) tools.

### Dependencies for Compilation:
- At least C++17.<br>
- Boost<br>
- Tinyxml-2<br>
- MariaDB Connector C++ 1.0.1 or greater for <b>SLPDatabase</b> (Optional)<br>

### My Environment
- Fedora 35<br>
- clang (clang++) version 13.0.0<br>
- g++ (GCC) 11.2.1 20220127 (Red Hat 11.2.1-9)<br>
(Under Oracle Linux 8 install gcc-devtool-10 or 8.6 install gcc-devtool-11)<br>
- cmake version 3.22.0<br>
- QtCreator 7.0.x (Code Style: clang-format -style=Mozilla)

### Examples

Small programs are provided with examples in the 'examples/' folder of this project.

### Basic Operations

- Loads entire log file into memory and check that the log entries are with the correct format;
- Allows query of log entries given a condition or regex.<br>
- Allows export of log entries to a XML file format.<br>
- <strong>(NEW)</strong> Allows export log entries to tables in a MariaDB(tm) database.
  The tables are preconfigured by the object. See
  [README_SLPDatabase](./README_SLPDatabase.md) for more details.<br>

### Objects

- SquidLogParser
 - Constructor
    - explicit SquidLogParser(LogFormat log_fmt_ )
 - Public Members
    - append()
    - errNum()
    - getErrorText()
    - size()
    - clear()
    - getPartInt()
    - getPartUint()
    - getPartStr()
    - addrToNumeric()
    - numericToAddr()
    - unixTimestamp()
    - unixToSquidDate()
    - toXML()
    - ShowDecodedUrl(URL)

- SLPQuery
 - Constructor
    - explicit SLPQuery(SquidLogParser* obj_);
 - Public Members
    - select()
    - field()
    - getInt()
    - getUInt()
    - getStr()
    - sumTotalSizeReply()
    - sumResponseTime()
    - countByReqMethod()
    - countByHttpCdodes()
    - HttpRequestCodes_V getHRCDetails()
    - MethodText()
    - size()
    - clear()
    - ShowDecodedUrl(URL)

- SLPDatabase
    - Constructor
        - explicit SLPDatabase(LogFormat format_, const std::string& dbase_, const std::string& host_, const int& port_, const std::string& user_, const std::string& pass_, const std::string& table_);
    - Public Members
        - insert()
        - createTable(bool)
        - getRowsInserted()
        - resetRowsInserted()
        - errorNum()
        - getErrorText()

- SLPUrlParts
 - Constructor
    - explicit SLPUrlParts(const std::string rawUrl_);
 - Public Members
    - getScheme()
    - getDomain()
    - getUsername()
    - getPassword()
    - getPath()
    - getQuery()
    - getFragment()
