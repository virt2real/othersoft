#!/usr/bin/php
<?php

	if (!isset($argv[1])) die("need a text parameter");

	include('config.php');

	/* override lang setting */
	if (isset($argv[2])) $LANG = $argv[2];

	/* override engine setting */
	if (isset($argv[3])) $SPEECH_ENGINE = $argv[3];

	include('/etc/virt2real/speech/include.php');

	Speak($argv[1]);

?>