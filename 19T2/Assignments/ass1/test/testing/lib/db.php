<?
// WebCMS2 utilities
// Database access library

function dbConn($connector)
{
	$db = pg_connect($connector);
	if (!$db) fatal("Can't connect to database $dbname");
	return $db;
}


function dbQuery($db,$sql)
{
	if (!is_resource($db)) fatal("Invalid database object");
	$res = pg_exec($db,$sql);
	if (!$res) fatal("Query failed\n$sql");
	return $res;
}

function dbNext($res,$style="hash")
{
	if (!is_resource($res)) fatal("dbNext: Invalid result object");
	switch ($style) {
	case "row": return pg_fetch_row($res); break;
	case "obj": return pg_fetch_object($res); break;
	default:    return pg_fetch_array($res); break;
	}
}

function dbCount($res)
{
	if (!is_resource($res)) fatal("dbNext: Invalid result object");
	return pg_num_rows($res);
}

function dbOneValue($db,$sql)
{
	$t = dbOneTuple($db,$sql,"row");
	return $t[0];
}

function dbOneTuple($db,$sql,$style="hash")
{
	$res = dbQuery($db,$sql);
	if (!is_resource($res)) return null;
	return dbNext($res,$style);
}

function dbAllTuples($db,$sql,$style="hash")
{
	$res = dbQuery($db,$sql);
	$tuples = array();
	while ($t = dbNext($res,$style)) $tuples[] = $t;
	return $tuples;
}

# mkSQL:
# - build an SQL query string from a printf-like format statement,
#   ensuring that values are properly quoted for passing to PostgreSQL
# - also converts PHP (null) value into SQL (null)
# - e.g. input:  mkSQL("select * from R where a=%d and b=%s",1,"it's")
#        output: "select * from R where a=1 and b='it''s'
function mkSQL()
{
	$argv = func_get_args();
	$a = 1;
	$q = preg_split('//', $argv[0], -1, PREG_SPLIT_NO_EMPTY);
	$n = count($q);
	$sql = ""; $nerrs = 0;
	for ($i = 0; $i < $n; $i++)
	{
		$c = $q[$i];
		if ($c == "\\")
			$sql .= $q[++$i];
		elseif ($c != "%")
			$sql .= $c;
		else {
			$i++;
			switch ($q[$i]) {
			// String
			case 's':
				$v = trim($argv[$a++]);
				if (empty($v))
					$sql .= "null";
				else {
					$v = pg_escape_string($v);
					if (strchr($v,"\\") !== false)
						$sql .= "E'{$v}'";
					else
						$sql .= "'{$v}'";
				}
				break;
			// Numeric (decimal/float)
			case 'd':
			case 'f':
				$v = $argv[$a++];
				if (empty($v) && $v !== 0 && $v !== "0")
					$sql .= "null";
				else {
					if ($c == 'd')
						$v = intval("$v");
					else
						$v = floatval("$v");
					$sql .= $v;
				}
				break;
			// Boolean
			case 'b':
				$v = $argv[$a++];
				$tf = truth_value($v);
				if (is_null($tf))
					$sql .= "null";
				elseif ($tf == 't')
					$sql .= "TRUE";
				elseif ($tf == 'f')
					$sql .= "FALSE";
				else {
					$sql .= $v;
					$nerrs++;
				}
				break;
			// Patterns
			case 'p':
				$v = $argv[$a++];
				if (empty($v))
					$sql .= "'%'";
				else {
					$v = pg_escape_string($v);
					if (strchr($v,"\\") !== false)
						$sql .= "E'%{$v}%'";
					else
						$sql .= "'%{$v}%'";
				}
				break;
			// Literal
			// Allows us to insert pre-built chunks of SQL
			case 'L':
				$v = $argv[$a++];
				if (empty($v))
					$nerrs++;
				else
					$sql .= $v;
				break;
			// Escape a literal '%'
			case '%':
				$sql .= '%';
				break;
			default:
				$nerrs++;
			}
		}
	}
	if ($nerrs > 0) db_error($sql);
	return $sql;
}

function truth_value($val)
{
	if (!isset($val)) return null;
	if ($val === true)
		return 't';
	elseif ($val === false)
		return 'f';
	$val = strtolower(substr($val,0,1));
	if ($val == 't' || $val == 'y')
		return 't';
	elseif ($val == 'f' || $val == 'n')
		return 'f';
	else
		return $val;
}

function db_error($msg) { fatal($msg); }

function fatal($msg)
{
	$msg .= "\n";
	$trace = debug_backtrace();
	foreach ($trace as $ref) {
		if (array_key_exists("file",$ref))
			$msg .= "in '$ref[file]'";
		if (array_key_exists("file",$ref))
			$msg .= ", line $ref[line]";
		if (array_key_exists("file",$ref))
			$msg .= ", $ref[function](";
		if (array_key_exists("args",$ref)) {
			$args = array();
			foreach ($ref["args"] as $a)
			$arg[] = strval($a);
			$msg .= join(",",$args);
		}
		if (array_key_exists("file",$ref))
			$msg .= ")";
		$msg .= "\n";
	}
	exit ("$msg\n");
}

?>
