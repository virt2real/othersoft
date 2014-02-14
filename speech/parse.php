<?php

/* 
	auto answer function
	if can't answer - return source text
*/

function MakeAnswer($text) {

	require_once 'config.php';

	global $VERBOSE;

	if ($VERBOSE) echo 'Try to make answer...';

	$message = "";
	$answer = $text;

	$arr = Array();

	if (preg_match('|(.*)привет|sei', $text, $arr)) 
		$answer = "Привет пупсик";

	if (preg_match('|тебя(.*)зовут|sei', $text, $arr)) 
		$answer = "меня зовут Виртурилка";
		
	if (preg_match('|меня(.*)зовут|sei', $text, $arr)) 
		$answer = "Вас зовут Пупсик";

	if (preg_match('|(.*)дура|sei', $text, $arr)) 
		$answer = "Сам дурак и уши у тебя холодные";

	if (preg_match('|(.*)тупа|sei', $text, $arr)) 
		$answer = "Сам тупой";

	if (preg_match('|да(.*)что|sei', $text, $arr)) 
		$answer = "Да ващеее";

	if (preg_match('|который(.*)час|sei', $text, $arr)) 
		$answer = "сейчас около " . GetTime();
		
	if (preg_match('|сколько(.*)врем|sei', $text, $arr)) 
		$answer = "сейчас около " . GetTime();

	if (preg_match('|врем(.*)скол|sei', $text, $arr)) 
		$answer = "сейчас около " . GetTime();


	if (preg_match('|(.*)темпер|sei', $text, $arr)) 
		$answer = "В Москве сейчас около " . GetTemperature();

	if (preg_match('|(.*)градус|sei', $text, $arr)) 
		$answer = "В Москве сейчас около " . GetTemperature();

	if (preg_match('|(.*)давлен|sei', $text, $arr)) 
		$answer = "В Москве атмосферное давление около " . GetPressure();

	if (preg_match('|(.*)погода|sei', $text, $arr)) 
		$answer = "В Москве температура около " . GetTemperature() . " , а давление около " . GetPressure();

	if (preg_match('|(.*)статус|sei', $text, $arr)) {
		$answer = 'я подключена к беспроводной сети ' . file_get_contents('/tmp/ssid');
	}

	if (preg_match('|(.*)сеть|sei', $text, $arr)) {
		$answer = 'я подключена к беспроводной сети ' . file_get_contents('/tmp/ssid');
	}

	if (preg_match('|(.*)сети|sei', $text, $arr)) {
		$answer = 'я подключена к беспроводной сети ' . file_get_contents('/tmp/ssid');
	}

	if (preg_match('|(.*)вайфа|sei', $text, $arr)) {
		$answer = 'я подключена к беспроводной сети ' . file_get_contents('/tmp/ssid');
	}

	if (preg_match('|(.*)комментар|sei', $text, $arr)) {
		$tmp = file_get_contents("http://www.g0l.ru/cache/lastcomment.json");
		$json = json_decode($tmp, TRUE);
		$answer = 'Пользователь ' . $json['login'] .' написал : '. $json['text'];
	}


	if (preg_match('|клю(.*)син|sei', $text, $arr)) {
		$answer = 'включаю синий светодиод';
		file_put_contents('/proc/v2r_gpio/pwctr3', 1);
	}

	if (preg_match('|ыклю(.*)син|sei', $text, $arr)) {
		$answer = 'выключаю синий светодиод';
		file_put_contents('/proc/v2r_gpio/pwctr3', 0);
	}

	if (preg_match('|клю(.*)красн|sei', $text, $arr)) {
		$answer = 'включаю красный светодиод';
		file_put_contents('/proc/v2r_gpio/74', 1);
	}

	if (preg_match('|жги(.*)красн|sei', $text, $arr)) {
		$answer = 'включаю красный светодиод';
		file_put_contents('/proc/v2r_gpio/74', 1);
	}

	if (preg_match('|ыклю(.*)красн|sei', $text, $arr)) {
		$answer = 'выключаю красный светодиод';
		file_put_contents('/proc/v2r_gpio/74', 0);
	}

	if (preg_match('|уши(.*)красн|sei', $text, $arr)) {
		$answer = 'выключаю красный светодиод';
		file_put_contents('/proc/v2r_gpio/74', 0);
	}


	if (preg_match('|клю(.*)зел|sei', $text, $arr)) {
		$answer = 'включаю зелёный светодиод';
		file_put_contents('/proc/v2r_gpio/73', 1);
	}

	if (preg_match('|ыклю(.*)зел|sei', $text, $arr)) {
		$answer = 'выключаю зелёный светодиод';
		file_put_contents('/proc/v2r_gpio/73', 0);
	}

	if (preg_match('|онтакт(.*)вкл(.*) |sei', $text, $arr)) {
		$answer = 'включаю контакт ' . $arr[2];
	}

	if (preg_match('|онтакт(.*)выкл(.*) |sei', $text, $arr)) {
		$answer = 'выключаю контакт ' . $arr[2];
	}




	if ($VERBOSE) echo " ".$message."\n";

	return $answer;


}

function GetTime() {
	return @date("G") . " часов " . intval(@date("i")) . ' минут ';
}

function GetTemperature() {

		$json = file_get_contents("http://www.g0l.ru/cache/temperatureJSON");
		$tmp = json_decode($json, true);
		$text = abs(round($tmp['value'])) . ' градусов';

		if ($tmp['value'] > 0) $text.= ' выше нуля';
		if ($tmp['value'] < 0) $text.= ' ниже нуля';

		return $text;

}


function GetPressure() {

		$json = file_get_contents("http://www.g0l.ru/cache/temperatureJSON");
		$tmp = json_decode($json, true);
		$text = abs($tmp['pressure']) . ' миллиметров ртутного столба';

		return $text;

}

?>