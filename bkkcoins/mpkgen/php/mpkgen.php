<?php

// Example using GenAddress as a cmd line utility
// 
// requires phpecc library
// easy way is: git clone git://github.com/mdanter/phpecc.git
// in the directory where this code lives
// and then this loader below will take care of it.

// bcmath module in php seems to be very slow
// apparently the gmp module is much faster
// base2dec needs to be written for gmp as phpecc is missing it

// this loads the phpecc modules and selects best math library
function __autoload($f) {
    //load the interfaces first otherwise contract errors occur
    $interfaceFile = "phpecc/classes/interface/" . $f . "Interface.php";

    if (file_exists($interfaceFile)) {
        require_once $interfaceFile;
    }


    //load class files after interfaces
    $classFile = "phpecc/classes/" . $f . ".php";
    if (file_exists($classFile)) {
        require_once $classFile;
    }

    //if utilities are needed load them last
    $utilFile = "phpecc/classes/util/" . $f . ".php";
    if (file_exists($utilFile)) {
        require_once $utilFile;
    }
}

if(extension_loaded('gmp') && !defined('USE_EXT')){
    define ('USE_EXT', 'GMP');
}else if(extension_loaded('bcmath') && !defined('USE_EXT')){
    define ('USE_EXT', 'BCMATH');
}

// search and replace 'bcmath_Utils::bc' with 'gmp_Utils::gmp_' to use much faster gmp module

function GenAddress($mpk, $seqN)
{
	// create the ecc curve
	$_p = bcmath_Utils::bchexdec('0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F');
    $_r = bcmath_Utils::bchexdec('0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141');
    $_b = bcmath_Utils::bchexdec('0x0000000000000000000000000000000000000000000000000000000000000007');
    $_Gx = bcmath_Utils::bchexdec('0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798');
    $_Gy = bcmath_Utils::bchexdec('0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8');
	$curve = new CurveFp($_p, 0, $_b);
    $gen = new Point( $curve, $_Gx, $_Gy, $_r );
	
	// prepare the input values
	$x = bcmath_Utils::bchexdec(substr($mpk, 0, 64));
	$y = bcmath_Utils::bchexdec(substr($mpk, 64, 64));
	$z = bcmath_Utils::bchexdec(hash('sha256', hash('sha256', $seqN.':0:'.pack('H*',$mpk), TRUE)));

	// generate the new public key based off master and sequence points
	$pt = Point::add(new Point($curve, $x, $y), Point::mul($z, $gen) );
	$keystr = "\x04".str_pad(bcmath_Utils::dec2base($pt->getX(), 256), 32, "\x0", STR_PAD_LEFT) 
		.str_pad(bcmath_Utils::dec2base($pt->getY(), 256), 32, "\x0", STR_PAD_LEFT);
	$vh160 =  "\x0".hash('ripemd160', hash('sha256', $keystr, TRUE), TRUE);
	$addr = $vh160.substr(hash('sha256', hash('sha256', $vh160, TRUE), TRUE), 0, 4);
		
	// base58 conversion
	$alphabet = '123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz';
    $encoded = '';
    $num = bcmath_Utils::base2dec($addr, 256);  // this needs to be rewritten if switching to gmp math

    while (intval($num) >= 58) {
        $div = bcdiv($num, '58');
        $mod = bcmod($num, '58');
        $encoded = $alphabet[intval($mod)] . $encoded;
        $num = $div;
    }
    $encoded = $alphabet[intval($num)] . $encoded;
    $pad = '';
    $n = 0;
    while ($addr[$n++] == "\x0")
		$pad .= '1';
 
    return $pad.$encoded;
}

print GenAddress($argv[1], $argv[2])."\n";

?>
