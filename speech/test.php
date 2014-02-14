#!/usr/bin/php

<?php

$str = "fuck you baby";

	if (preg_match('|you(.*)baby|sei', $str, $arr))  {
		echo 'found';
		print_r ($arr);
	}

?>