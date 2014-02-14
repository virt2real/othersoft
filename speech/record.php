#!/usr/bin/php
<?php

	if (!isset($argv[1])) die("need a filename parameter");

	include 'config.php';
	include 'include.php';

	RecordSpeech($argv[1]);

?>