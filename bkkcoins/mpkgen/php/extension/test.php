<?php

// example of how to use GenAddress from the mpkgen extension.

for($n = 0; $n < $argv[2]; $n++)
	print GenAddress($argv[1], $n)."\n";

?>
