# <b>SLPDatabase</b>

The _SLPDatabase_ is a utility object that expands the original capabilities of the _SquidLogParser_ by allowing the import of raw log data from Squid-cache(tm), allowing the log entries to be field-separated and written to MariaDB(tm) tables natively.

The _SLPDatabase_ has two main functions:
* Create the tables in the database
* Insert the data into these tables

In addition to the numerous possibilities of analyzing data using SQL, we can also take advantage of the UDF's for MariaDB(tm) that are in the https://github.com/vcputtini/udf_mariadb_cpp project.

The _SLPDatabase_ uses the mechanisms provided by MariaDB-connector C++, to execute all tasks related to data manipulation.

## Reference

| Constructor | Parameters | Notes |
| --- | --- | --- |
| SLPDatabase | enum LogFormat,<br>std::string DataBaseName,<br>std::string Hostname,<br>int PortNumber,<br>std::string Username,<br>std::string Password,<br>std::string Tablename | With the exception of the Port Number which<br> can be 0 (zero), as it tells the object to use the<br> default port 3306, all other parameters are required.<br><br>The default method used to connect to the database is <b>tcp://</b>, but the other methods supported by MariaDB-connector are accepted. |


| Member Name | Brief |
| --- | --- |
| _insert_(Raw Log Line) | Parses and insert the log entries into database table.|
| _dataIngest_(Raw Log Line) | Reads log entries directly from the file where Squid-cache(tm) is writing data. Waiting for each new entry and writing it to the appropriate table.<br>Have a behavior similar to the command: tail -n0 -F file.log |
| _createTable_(bool) | Create the table in the database. If _true_(default), close the connection after creating the table. If _false_ don't close. |
| _getRowsInserted_() | Get the total number of log entries written. |
| _resetRowsInserted_() | Reset the counter. |
| _erroNum_() | Get the error code. |
| _getErrorText_() | Get the message corresponding to the generated error code. |


## Tables

Default structure for the tables.

_{TABLE_NAME}_ is the user given name for the table. If a name is not
informed, one of the default names below will be used for each log format.

| Format | Default Table Name |
| --- | --- |
| `Squid` | slp_log_squid |
| `Common` | slp_log_common |
| `Combined` | slp_log_combined |
| `Referrer` | slp_log_referrer |
| `UserAgent` | slp_log_useragent |

<details><summary>Click here to see the structure of the tables</summary>

```
-- Format: Squid
CREATE TABLE {_TABLE_NAME_} (
  ID bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  ID_MONTH tinyint(3) unsigned NOT NULL,
  TIMESTAMP int(10) unsigned DEFAULT NULL,
  CLIENT_SRC_IP varchar(40) DEFAULT NULL,
  REQSTATUS_HIERSTATUS varchar(30) DEFAULT NULL,
  TOTAL_SIZE_REPLY int(11) DEFAULT NULL,
  REQ_METHOD char(10) DEFAULT NULL,
  URL blob DEFAULT NULL,
  USER_NAME varchar(30) DEFAULT NULL,
  HIERSTATUS_IPADDRESS varchar(30) DEFAULT NULL,
  MIMETYPE varchar(100) DEFAULT NULL,
  PRIMARY KEY (ID,ID_MONTH)) PARTITION BY HASH (`ID_MONTH`)
  PARTITIONS 12;
```
```
-- Format: Common
CREATE TABLE {_TABLE_NAME_} (
  ID bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  ID_MONTH tinyint(3) unsigned NOT NULL,
  CLIENT_SRC_IP varchar(40) DEFAULT NULL,
  USER_NAME_FROM_IDENT varchar(30) DEFAULT NULL,
  USER_NAME varchar(30) DEFAULT NULL,
  REQ_METHOD char(10) DEFAULT NULL,
  URL blob DEFAULT NULL,
  REQ_PROTO_VERSION varchar(50) DEFAULT NULL,
  HTTP_STATUS int(11) DEFAULT NULL,
  TOTAL_SIZE_REPLY int(11) DEFAULT NULL,
  REQSTATUS_HIERSTATUS varchar(30) DEFAULT NULL,
  PRIMARY KEY (ID,ID_MONTH)) PARTITION BY HASH (`ID_MONTH`)
  PARTITIONS 12;
```
```
-- Format: Combined
CREATE TABLE {_TABLE_NAME_} (
  ID bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  ID_MONTH tinyint(3) unsigned NOT NULL,
  CLIENT_SRC_IP varchar(40) DEFAULT NULL,
  USER_NAME_FROM_IDENT varchar(30) DEFAULT NULL,
  USER_NAME varchar(30) DEFAULT NULL,
  LOCAL_TIME varchar(50) DEFAULT NULL,
  REQ_METHOD char(10) DEFAULT NULL,
  URL blob DEFAULT NULL,
  REQ_PROTO_VERSION varchar(50) DEFAULT NULL,
  HTTP_STATUS int(11) DEFAULT NULL,
  TOTAL_SIZE_REPLY int(11) DEFAULT NULL,
  REFERRER text DEFAULT NULL,
  USER_AGENT text DEFAULT NULL,
  HIERSTATUS_IPADDRESS varchar(30) DEFAULT NULL,
  PRIMARY KEY (ID,ID_MONTH)) PARTITION BY HASH (`ID_MONTH`)
  PARTITIONS 12;
```
```
-- Format: Referrer
CREATE TABLE {_TABLE_NAME_} (
  ID bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  ID_MONTH tinyint(3) unsigned NOT NULL,
  TIMESTAMP int(10) unsigned DEFAULT NULL,
  CLIENT_SRC_IP varchar(40) DEFAULT NULL,
  REFERRER text DEFAULT NULL,
  URL blob DEFAULT NULL,
  PRIMARY KEY (ID,ID_MONTH)) PARTITION BY HASH (`ID_MONTH`)
  PARTITIONS 12;
```
```
-- Format: UserAgent
CREATE TABLE {_TABLE_NAME_} (
  ID bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  ID_MONTH tinyint(3) unsigned NOT NULL,
  CLIENT_SRC_IP varchar(40) DEFAULT NULL,
  LOCAL_TIME varchar(50) DEFAULT NULL,
  USER_AGENT text DEFAULT NULL,
  PRIMARY KEY (ID,ID_MONTH)) PARTITION BY HASH (`ID_MONTH`)
  PARTITIONS 12;
```
</details>

## Examples

Small programs are provided with examples in the 'examples/' folder of this project.
