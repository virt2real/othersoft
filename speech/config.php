<?php

	$FILE_MIN_SIZE = 2000; // minimal file size to send
	$TMP_PATH = "/tmp"; // path for tmp files
	$CACHE_PATH = "/usr/share/googlevoice"; // cache dir for downloaded Google mp3 files

	/* next variables get from JSON config */
	$LANG = "en"; // language
	$SPEECH_ENGINE = 0; // 0 - espeak, 1 - Google
	$VERBOSE = 1; // 1 - show messages
	$SPEAKERRORS = 1; // 1 - speak errors

	$config = file_get_contents('/etc/virt2real/speech/config.json');

	if ($config) {

		$json = json_decode($config, true);

		if ($json) {

			$LANG = $json['lang'];
			$SPEECH_ENGINE = $json['engine'];
			$SPEAKERRORS = $json['speakerrors'];

		}
		
	}

	function SaveSettings($lang, $engine, $speakerrors) {

		$arr['lang'] = $lang;
		$arr['engine'] = $engine;
		$arr['speakerrors'] = $speakerrors;

		$json = json_encode($arr);
		file_put_contents('/etc/virt2real/speech/config.json', $json);
	}

?>