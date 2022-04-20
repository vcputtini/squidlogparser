## SquidLogParser
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

### My Environment
- Fedora 35<br>
- clang (clang++) version 13.0.0<br>
- g++ (GCC) 11.2.1 20220127 (Red Hat 11.2.1-9)<br>
(Under Oracle Linux 8 install gcc-devtool-10 or 8.5 install gcc-devtool-11)<br>
- cmake version 3.22.0<br>
- QtCreator 6.0.x (Code Style: clang-format -style=Mozilla)

### Examples

Small programs are provided with examples in the 'examples/' folder of this project.

### Basic Operations

- Loads entire log file into memory and check that the log entries are with the correct format;
- Allows export of log entries to a XML file format.<br>
- Allows query of log entries given a condition or regex.<br>

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

- SLPUrlParts
 - Constructor
    - explicit SLPUrlParts(const std::string rawUrl_);
 - Public Members
    - getProtocol()
    - getDomain()
    - getPath()
    - getQuery()
    - getFragment()
