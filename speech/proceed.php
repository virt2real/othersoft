#!/usr/bin/php
<?php

	if (!isset($argv[1])) die("need a FLAC filename parameter");

	include 'config.php';

	include 'include.php';

	$text = GetResultText($argv[1], true);

	switch ($text) {

		case -1:
		case "":
			if ($SPEAKERRORS)  {
				if ($LANG=="ru") 
					Speak("извините, не понимаю"); 
				else 
					Speak("sorry, i'm not understand");
			}
			break;

		default:

			/* check special words for exit */

			/* good bye */
			if (preg_match('|good(.*)bye|sei', $text, $arr)) {
				Speak("ok, i am going down. Bye bye!");
				exit (1);
			}

			/* good bye */
			if (preg_match('|bye(.*)bye|sei', $text, $arr)) {
				Speak("ok, i am going down. Bye bye!");
				exit (1);
			}

			/* заткнись */
			if (strpos($text, "заткнись") !== FALSE) {
				Speak("хорошо, молчу-молчу");
				SaveSettings($LANG, $SPEECH_ENGINE, 0);
				exit (0);
			}

			/* молчи */
			if (strpos($text, "молч") !== FALSE) {
				Speak("хорошо, молчу-молчу");
				SaveSettings($LANG, $SPEECH_ENGINE, 0);
				exit (0);
			}

			/* говори */
			if (strpos($text, "говори") !== FALSE) {
				Speak("окей, я буду сообщать об ошибках");
				SaveSettings($LANG, $SPEECH_ENGINE, 1);
				exit (0);
			}

			if (strpos($text, "свидан") !== FALSE) {
				Speak("И вам досвидания");
				exit (1);
			}

			/* пока пока */
			if (preg_match('|пока(.*)пока|sei', $text, $arr)) {
				Speak("Ага, пока пока!");
				exit (1);
			}


			/* try to make answer */

			include 'parse.php';
			$text = MakeAnswer($text);

			Speak($text);
			
	}


?>