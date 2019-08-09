<?
if (count($argv) < 3) exit("Usage: $argv[0] DB DIR\n");
$DBname = $argv[1];
$dir = $argv[2];
require("lib/db.php");
$db = dbConn("dbname=$DBname");
$dd = dbOneValue($db, "select oid from pg_database where datname='$DBname'");
if (empty($dd)) exit("Can't find database\n");
$uf = dbOneValue($db, "select oid from pg_class where relname='users'");
if (empty($uf)) exit("Can't find Users table\n");
$uz = filesize("$dir/$dd/$uf")/(1024*1024.0);
printf("Size of Users data file = %0.2f MB\n",$uz);
$sf = dbOneValue($db, "select oid from pg_class where relname='sessions'");
if (empty($uf)) exit("Can't find Sessions table\n");
$sz = filesize("$dir/$dd/$sf")/(1024*1024.0);
printf("Size of Sessions data file = %0.2f MB\n",$sz);
?>
