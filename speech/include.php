<?php

	function RecordSpeech($filename) {

		global $TMP_PATH;

		file_put_contents("/proc/v2r_gpio/pwctr3", 1);

		//exec('rec -c 1 -r 16000 -b 16 '.$TMP_PATH.'/'.$filename.'.wav --no-show-progress silence 1 0.1 6% 1 1.0 10%');
		exec('rec -c 1 -r 16000 -b 16 '.$TMP_PATH.'/'.$filename.'.wav --no-show-progress silence 1 0.1 6% 1 0.5 8%');

		exec('flac -f -s '.$TMP_PATH.'/'.$filename.'.wav -o '.$TMP_PATH.'/'.$filename.'.flac');

		file_put_contents("/proc/v2r_gpio/pwctr3", 0);

	}


	function GetResultText($filename, $flag) {

		global $FILE_MIN_SIZE;
		global $LANG;
		global $VERBOSE;
		global $TMP_PATH;

		$filename = $TMP_PATH.'/'.$filename.'.flac';

		if (filesize($filename) < $FILE_MIN_SIZE) return -1;

		$lang = ($LANG == "ru") ? 'ru-RU' : 'en-US';

		if ($VERBOSE) echo "sending to server... ";

		$file_to_upload = array('userfile'=>'@' . $filename);

		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, "http://www.google.com/speech-api/v1/recognize?xjerr=1&client=chromium&lang=$lang");
		curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (X11; Linux x86_64; rv:17.0) Gecko/17.0 Firefox/17.0');
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_TIMEOUT, 5);
		curl_setopt($ch, CURLOPT_HTTPHEADER, array("Content-Type: audio/x-flac; rate=16000"));
		curl_setopt($ch, CURLOPT_POSTFIELDS, $file_to_upload);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
    	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
		$json=curl_exec ($ch);
		curl_close($ch);

		if ($VERBOSE) echo "done\n";

		if (!$flag) return $json;

		$result = json_decode($json, true);

		if (!$result) return "";

		//print_r($result);

		if ($result['status']) {

			return "";
		}

		foreach ($result['hypotheses'] as $key => $value)  {

			$text = $value['utterance'];
			$average = floor($value['confidence'] * 100);

			if ($VERBOSE) echo "variants:\n";

			if ($VERBOSE) echo "\t$key) $average%: " . $text . "\n";

		}

		return $text;

	}

	function Speak($text) {

		if (!$text) return -1;

		global $SPEECH_ENGINE;
		global $LANG;

		switch ($SPEECH_ENGINE) {

			case 1:
				SpeakGoogle($text);
				break;

			default:
				$lang = ($LANG == "ru") ? 'ru' : 'en';

				exec('espeak "'.$text.'"  -a 200 -g 3 -k 1 -p 99 -v '.$lang.' -s 120 --stdout | aplay -q');
				break;

		}

	}


	function SpeakGoogle($text) {

		global $LANG;
		global $TMP_PATH;
		global $CACHE_PATH;
		global $VERBOSE;

		if (!$text) return -1;

		$lang = ($LANG == "ru") ? 'ru' : 'en';

		/* check dir */
		if (!file_exists($CACHE_PATH))
			 mkdir ($CACHE_PATH);

		/* make filename for cache */
		$encodedfilename = md5(urlencode($text));
		$filename = $CACHE_PATH. '/' . $encodedfilename . '.' .$LANG . '.mp3';

		$filename = str_replace("'", "_", $filename);

		// $filename = $TMP_PATH.'/answer.mp3';

		if (!file_exists($filename) || !filesize($filename)) {

			if ($VERBOSE) echo "loading from Google... ";
			exec('wget -q -U "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5" "http://translate.google.com/translate_tts?q='.$text.'&tl='.$lang.'" -O ' . $filename);
			if ($VERBOSE) echo " done\n";

		} else 
			if ($VERBOSE) echo "found in voice cache\n";

		exec('play -q -v 3 -V1 "'.$filename.'" >> /dev/null');

		//exec('rm -f ' . $filename);

	}


?>