<?php
/*
$Id$

  This code is part of LDAP Account Manager (http://www.sourceforge.net/projects/lam)
  Copyright (C) 2003  Roland Gruber

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

// ldap.inc provides basic functions to connect to the OpenLDAP server.

// PHP 5 compatibility
if( phpversion() >= "5" ) ini_set( "zend.ze1_compatibility_mode", 1 );

include_once("config.inc");
include_once("blowfish.inc");

// converts a HEX string to a binary value
function hex2bin($value) {
	return pack("H*", $value);
}

// returns the hash value of a plain text password
// the hash algorithm depends on the configuration file
// $password: the password string
// $enabled: marks the hash as enabled/disabled (e.g. by prefixing "!")
function pwd_hash($password, $enabled=true) {
	// check for empty password
	if (! $password || ($password == "")) {
		return "";
	}
	// calculate new random number
	$_SESSION['ldap']->new_rand();
	// hash password with algorithm from config file
	$hash = "";
	switch ($_SESSION['config']->get_pwdhash()) {
		case 'CRYPT':
			$hash = "{CRYPT}" . crypt($password);
			break;
		case 'MD5':
			$hash = "{MD5}" . base64_encode(hex2bin(md5($password)));
			break;
		case 'SMD5':
				$salt0 = substr(pack("h*", md5($_SESSION['ldap']->rand)), 0, 8);
				$salt = substr(pack("H*", md5($salt0 . $password)), 0, 4);
				$hash = "{SMD5}" . base64_encode(hex2bin(md5($password . $salt)) . $salt);
			break;
		case 'SHA':
			// PHP 4.3+ can use sha1() function
			if (function_exists(sha1)) {
				$hash = "{SHA}" . base64_encode(hex2bin(sha1($password)));
			}
			// otherwise use MHash
			elseif (function_exists(mHash)) {
				$hash = "{SHA}" . base64_encode(mHash(MHASH_SHA1, $password));
			}
			// if SHA1 is not possible use crypt()
			else {
				$hash = "{CRYPT}" . crypt($password);
			}
			break;
		case 'SSHA':
			// PHP 4.3+ can use sha1() function
			if (function_exists(sha1)) {
				$salt0 = substr(pack("h*", md5($_SESSION['ldap']->rand)), 0, 8);
				$salt = substr(pack("H*", sha1($salt0 . $password)), 0, 4);
				$hash = "{SSHA}" . base64_encode(hex2bin(sha1($password . $salt)) . $salt);
			}
			// otherwise use MHash
			elseif (function_exists(mHash)) {
				$salt = mhash_keygen_s2k(MHASH_SHA1, $password, substr(pack("h*", md5($_SESSION['ldap']->rand)), 0, 8), 4);
				$hash = base64_encode(mHash(MHASH_SHA1, $password . $salt) . $salt);
				$hash = "{SSHA}" . $hash;
			}
			// if SSHA is not possible use crypt()
			else {
				$hash = "{CRYPT}" . crypt($password);
			}
			break;
		case 'PLAIN':
			$hash = $password;
			break;
		// use SSHA if the setting is invalid
		default:
			// PHP 4.3+ can use sha1() function
			if (function_exists(sha1)) {
				$salt0 = substr(pack("h*", md5($_SESSION['ldap']->rand)), 0, 8);
				$salt = substr(pack("H*", sha1($salt0 . $password)), 0, 4);
				$hash = "{SSHA}" . base64_encode(hex2bin(sha1($password . $salt)) . $salt);
			}
			// otherwise use MHash
			elseif (function_exists(mHash)) {
				$salt = mhash_keygen_s2k(MHASH_SHA1, $password, substr(pack("h*", md5($_SESSION['ldap']->rand)), 0, 8), 4);
				$hash = base64_encode(mHash(MHASH_SHA1, $password . $salt) . $salt);
				$hash = "{SSHA}" . $hash;
			}
			// if SSHA is not possible use crypt()
			else {
				$hash = "{CRYPT}" . crypt($password);
			}
		break;
	}
	// enable/disable password
	if (! $enabled) return pwd_disable($hash);
	else return $hash;
}


// marks an password hash as enabled
// and returns the new hash string
// hash: hash value to enable
function pwd_enable($hash) {
	// check if password is disabled (old wrong LAM method)
	if ((substr($hash, 0, 2) == "!{") || ((substr($hash, 0, 2) == "*{"))) {
		return substr($hash, 1, strlen($hash));
	}
	// check for "!" or "*" at beginning of password hash
	else {
		if (substr($hash, 0, 1) == "{") {
			$pos = strpos($hash, "}");
			if ((substr($hash, $pos + 1, 1) == "!") || (substr($hash, $pos + 1, 1) == "*")) {
				// enable hash
				return substr($hash, 0, $pos + 1) . substr($hash, $pos + 2, strlen($hash));
			}
			else return $hash;  // not disabled
		}
		else return $hash;  // password is plain text
	}
}

// marks an password hash as disabled
// and returns the new hash string
// hash: hash value to disable
function pwd_disable($hash) {
	// check if password is disabled (old wrong LAM method)
	if ((substr($hash, 0, 2) == "!{") || ((substr($hash, 0, 2) == "*{"))) {
		return $hash;
	}
	// check for "!" or "*" at beginning of password hash
	else {
		if (substr($hash, 0, 1) == "{") {
			$pos = strpos($hash, "}");
			if ((substr($hash, $pos + 1, 1) == "!") || (substr($hash, $pos + 1, 1) == "*")) {
				// hash already disabled
				return $hash;
			}
			else return substr($hash, 0, $pos + 1) . "!" . substr($hash, $pos + 1, strlen($hash));  // not disabled
		}
		else return $hash;  // password is plain text
	}
}

// checks if a password hash is enabled/disabled
// returns true if the password is marked as enabled
function pwd_is_enabled($hash) {
	// disabled passwords have a "!" or "*" at the beginning (old wrong LAM method)
	if ((substr($hash, 0, 2) == "!{") || ((substr($hash, 0, 2) == "*{"))) return false;
	if (substr($hash, 0, 1) == "{") {
		$pos = strrpos($hash, "}");
		// check if hash starts with "!" or "*"
		if ((substr($hash, $pos + 1, 1) == "!") || (substr($hash, $pos + 1, 1) == "*")) return false;
		else return true;
	}
	else return true;
}


// manages connection to LDAP and several helper functions
class Ldap{

	// object of Config to access preferences
	var $conf;

	// server handle
	var $server;

	// LDAP username and password used for bind
	var $username;
	var $password;

	// Arrays that contain LDAP attributes and their descriptions which are translated
	var $ldapUserAttributes;
	var $ldapGroupAttributes;
	var $ldapHostAttributes;

	// array with all objectClass strings from the LDAP server
	var $objectClasses;

	// capabilities of the LDAP server
	var $supports_unix_hosts=false;  // host attribute in inetOrgPerson
	var $supports_samba2_schema=false;  // objectClass sambaAccount
	var $supports_samba3_schema=false;  // objectClass sambaSamAccount

	// random number (changes on every page request)
	var $rand;

	// constructor
	// $config: an object of Config (../config/config.php)
	function Ldap($config) {
		setlanguage();
		if (is_object($config)) $this->conf = $config;
		else return false;
		// construct arrays with known LDAP attributes
		$this->ldapUserAttributes = array (
			"uid" => _("User ID"),
			"uidnumber" => _("UID number"),
			"gidnumber" => _("GID number"),
			"cn" => _("Username"),
			"host" => _("Allowed hosts"),
			"givenname" => _("First name"),
			"sn" => _("Last name"),
			"homedirectory" => _("Home directory"),
			"loginshell" => _("Login shell"),
			"mail" => _("E-Mail"),
			"gecos" => _("Description")
			);
		$this->ldapGroupAttributes = array (
			"cn" => _("Group name"),
			"gidnumber" => _("GID number"),
			"memberuid" => _("Group members"),
			"member" => _("Group member DNs"),
			"description" => _("Group description")
			);
		$this->ldapHostAttributes = array (
			"uid" => _("Host username"),
			"cn" => _("Host name"),
			"rid" => _("RID (Windows UID)"),
			"description" => _("Host description"),
			"uidnumber" => _("UID number"),
			"gidnumber" => _("GID number")
			);
		mt_srand((double)microtime()*1000000);
		$this->rand = mt_rand();
		return true;
	}

	// connects to the server using the given username and password
	// if connect succeeds the server handle is returned
	// $user: user name
	// $passwd: password
	function connect($user, $passwd) {
		// close any prior connection
		@$this->close();
		// do not allow anonymous bind
		if ((!$user)||($user == "")||(!$passwd)) {
			return false;
		}
		// save password und username encrypted
		$this->encrypt_login($user, $passwd);
		$this->server = @ldap_connect($this->conf->get_ServerURL());
		if ($this->server) {
			// use LDAPv3
			ldap_set_option($this->server, LDAP_OPT_PROTOCOL_VERSION, 3);
			// start TLS if possible
			if (function_exists('ldap_start_tls')) {
				@ldap_start_tls($this->server);
				// connect without TLS if it failed
				if (ldap_errno($this->server) != 0) {
					@ldap_close($this->server);
					$this->server = @ldap_connect($this->conf->get_ServerURL());
					ldap_set_option($this->server, LDAP_OPT_PROTOCOL_VERSION, 3);
				}
			}
			$bind = @ldap_bind($this->server, $user, $passwd);
			if ($bind) {
				// read objectClasses from server and update capabilities if needed
				if (! $this->objectClasses) {
					$this->updateClasses();
					$this->updateCapabilities();
				}
				// return success number
				return ldap_errno($this->server);
			}
			// return error number
			else return ldap_errno($this->server);
		}
		else return false;
	}

	// closes connection to server
	function close() {
		@ldap_close($this->server);
	}

	// searches LDAP for a specific user name
	// and returns its DN entry
	// $name: user name
	function search_username($name) {
		$filter = "(uid=$name)";
		$attrs = array();
		$sr = @ldap_search($this->server, $this->conf->get_UserSuffix(), $filter, $attrs);
		if ($sr) {
			$info = ldap_get_entries($this->server, $sr);
			// return only first DN entry
			$ret = $info[0]["dn"];
			ldap_free_result($sr);
			return $ret;
		}
	}

	// returns an array with all organizational units under the given suffix
	// $suffix: search suffix
	function search_units($suffix) {
		$ret = array();
		$sr = @ldap_search($this->server(), $suffix, "objectClass=organizationalunit", array("DN"));
		if ($sr) {
			$units = ldap_get_entries($this->server, $sr);
			// extract Dns
			for ($i = 0; $i < sizeof($units); $i++) {
				if ($units[$i]['dn']) $ret[] = $units[$i]['dn'];
			}
		}
		// add root suffix if needed
		$found = false;
		for ($i = 0; $i < sizeof($ret); $i++) { // search suffix case-intensitive
			if (strtolower($suffix) == strtolower($ret[$i])) {
				$found = true;
				break;
			}
		}
		if (!$found) {
			$ret[] = $suffix;
		}
		usort($ret, array($this,"cmp_array"));
		return $ret;
	}

	// returns an array with all Samba 3 domain entries under the given suffix
	// $suffix: search suffix
	function search_domains($suffix) {
		$ret = array();
		$attr = array("DN", "sambaDomainName", "sambaSID", "sambaNextRid", "sambaNextGroupRid",
			"sambaNextUserRid", "sambaAlgorithmicRidBase");
		$sr = @ldap_search($this->server(), $suffix, "objectClass=sambaDomain", $attr);
		if ($sr) {
			$units = ldap_get_entries($this->server, $sr);
			// delete count entry
			array_shift($units);
			// extract attributes
			for ($i = 0; $i < sizeof($units); $i++) {
				$ret[$i] = new samba3domain();
				$ret[$i]->dn = $units[$i]['dn'];
				$ret[$i]->name = $units[$i]['sambadomainname'][0];
				$ret[$i]->SID = $units[$i]['sambasid'][0];
				$ret[$i]->nextRID = $units[$i]['sambanextrid'][0];
				$ret[$i]->nextGroupRID = $units[$i]['sambanextgrouprid'][0];
				$ret[$i]->nextUserRID = $units[$i]['sambanextuserrid'][0];
				if (isset($units[$i]['sambaalgorithmicridbase'][0])) $ret[$i]->RIDbase = $units[$i]['sambaalgorithmicridbase'][0];
			}
			// sort array by domain name
			usort($ret, array($this,"cmp_domain"));
		}
		return $ret;
	}

	// reads the array of objectClasses from the LDAP server
	function updateClasses() {
		// read from default cn
		$sr = @ldap_read($this->server, 'cn=subschema', '(objectClass=*)', array('objectclasses'));
		// if default was not correct check different cn
		if (!$sr) $sr = @ldap_read($this->server, 'cn=schema', '(objectClass=*)', array('objectclasses'));
		if ($sr) {
			// get search result and save it
			$info = @ldap_get_entries($this->server,$sr);
			if ($info) {
				$this->objectClasses = $info[0]['objectclasses'];
				array_shift($this->objectClasses);
				return true;
			}
		}
		// if search failed save empty result
		$this->objectClasses = array();
	}

	// updates the capabilities values (var $supports_*)
	function updateCapabilities() {
		for ($i = 0; $i < sizeof($this->objectClasses); $i++) {
			$line = $this->objectClasses[$i];
			// search keywords
			if (strpos($line, "NAME 'inetOrgPerson'") && strpos($line, " host ")) $this->supports_unix_hosts = true;
			if (strpos($line, "NAME 'sambaAccount'")) $this->supports_samba2_schema = true;
			if (strpos($line, "NAME 'sambaSamAccount'")) $this->supports_samba3_schema = true;
		}
	}

	// returns the LDAP connection handle
	function server() {
		return $this->server;
	}

	// closes connection to LDAP server before serialization
	function __sleep() {
		$this->close();
		// define which attributes to save
		return array("conf", "username", "password", "ldapUserAttributes", "ldapGroupAttributes",
			"ldapHostAttributes", "objectClasses", "supports_unix_hosts", "supports_samba2_schema",
			"supports_samba3_schema", "rand");
	}

	// reconnects to LDAP server when deserialized
	function __wakeup() {
		$data = $this->decrypt_login();
		$this->connect($data[0], $data[1]);
		// change random number
		mt_srand($this->rand + (microtime() * 1000000));
		$this->rand = mt_rand();
		// delete PDF files which are older than 10 min
		if (isset($_SESSION['lampath'])) {
			$relpath = $_SESSION['lampath'] . 'tmp/';
			$time = time();
			$dir = @opendir($relpath);
			while ($file = @readdir($dir)) {
					if (substr($file, -4) == '.pdf') {
					$path = $relpath . $file;
					if ($time - filemtime($path) > 600) {
						@unlink($path);
					}
				}
			}
			@closedir($h);
		}
	}

	// calculates a new value for rand
	function new_rand() {
		// change random number
		mt_srand($this->rand + (microtime() * 1000000));
		$this->rand = mt_rand();
	}

	// encrypts a string
	// $data: string to encrypt
	// return: encrypted string
	function encrypt($data) {
		// use MCrypt if available
		if (function_exists(mcrypt_create_iv)) {
			// read key and iv from cookie
			$iv = base64_decode($_COOKIE["IV"]);
			$key = base64_decode($_COOKIE["Key"]);
			// encrypt string
			return mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $key, $data, MCRYPT_MODE_ECB, $iv);
		}
		// use Blowfish if MCrypt is not available
		else {
			// read key and iv from cookie
			$iv = base64_decode($_COOKIE["IV"]);
			$key = base64_decode($_COOKIE["Key"]);
			$b_key = $iv . $key;
			// encrypt string
			$b_fish = new Cipher_blowfish();
			return $b_fish->encrypt($data, $b_key);
		}
	}

	// decrypts a string
	// $data: string to decrypt
	// return: decrypted string
	function decrypt($data) {
		// use MCrypt if available
		if (function_exists(mcrypt_create_iv)) {
			// read key and iv from cookie
			$iv = base64_decode($_COOKIE["IV"]);
			$key = base64_decode($_COOKIE["Key"]);
			// decrypt string
			$ret = mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $key, $data, MCRYPT_MODE_ECB, $iv);
			$ret = str_replace(chr(00), "", $ret);
			return $ret;
		}
		// use Blowfish if MCrypt is not available
		else {
			// read key and iv from cookie
			$iv = base64_decode($_COOKIE["IV"]);
			$key = base64_decode($_COOKIE["Key"]);
			$b_key = $iv . $key;
			// decrypt string
			$b_fish = new Cipher_blowfish();
			return $b_fish->decrypt($data, $b_key);
		}
	}

	// encrypts username and password
	// $username: LDAP user name
	// $password: LDAP password
	function encrypt_login($username, $password) {
		// encrypt username and password
		$this->username = base64_encode($this->encrypt($username));
		$this->password = base64_encode($this->encrypt($password));
	}

	// decrypts username and password
	// returns an array
	// return[0]: user name
	// return[1]: password
	function decrypt_login() {
		// decrypt username and password
		$username = $this->decrypt(base64_decode($this->username));
		$password = $this->decrypt(base64_decode($this->password));
		$ret = array($username, $password);
		return $ret;
	}

	// closes connection to LDAP server and deletes encrypted username/password
	function destroy() {
		$this->close();
		$this->username="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		$this->password="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	}

	// returns an array that contains LDAP attribute names and their description
	function attributeUserArray() {
		return $this->ldapUserAttributes;
	}

	// returns an array that contains LDAP attribute names and their description
	function attributeGroupArray() {
		return $this->ldapGroupAttributes;
	}

	// returns an array that contains LDAP attribute names and their description
	function attributeHostArray() {
		return $this->ldapHostAttributes;
	}


	// helper function to sort the unit DNs
	function cmp_array($a, $b) {
		// split DNs
		$array_a = explode(",", $a);
		$array_b = explode(",", $b);
		$len_a = sizeof($array_a);
		$len_b = sizeof($array_b);
		// check how many parts to compare
		$len = min($len_a, $len_b);
		// compare from last part on
		for ($i = 0; $i < $len; $i++) {
			// get parts to compare
			$part_a = strtolower($array_a[$len_a - $i - 1]);
			$part_b = strtolower($array_b[$len_b - $i - 1]);
			// compare parts
			if ($part_a == $part_b) { // part is identical
				if ($i == ($len - 1)) {
					if ($len_a > $len_b) return 1;
					elseif ($len_a < $len_b) return -1;
					else return 0; // DNs are identical
				}
			}
			elseif ($part_a == max($part_a, $part_b)) return 1;
			else return -1;
		}
	}

	// helper function to sort the domains
	function cmp_domain($a, $b) {
		if ($a->name == $b->name) return 0;
		elseif ($a->name == max($a->name, $b->name)) return 1;
		else return -1;
	}


}

// represents a Samba 3 domain entry
class samba3domain {

	// DN
	var $dn;

	// domain name
	var $name;

	// domain SID
	var $SID;

	// next RID
	var $nextRID;

	// next user RID
	var $nextUserRID;

	// next group RID
	var $nextGroupRID;

	// RID base to calculate RIDs, default 1000
	var $RIDbase=1000;
}

?>