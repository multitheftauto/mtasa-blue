<?php

ini_set("default_charset", "UTF-8");

echo "-- Autogenrating include/dpp/unicode_emoji.h\n";

$url = "https://raw.githubusercontent.com/ArkinSolomon/discord-emoji-converter/master/emojis.json";

$header = <<<END
#pragma once

namespace dpp {

/**
 * @brief Emoji unicodes.
 *
 * @note The unicode emojis in this namespace are auto-generated from https://raw.githubusercontent.com/ArkinSolomon/discord-emoji-converter/master/emojis.json
 *
 * @warning If you want to use this, you have to pull the header in separately. For example:
 * ```cpp
 * #include <dpp/dpp.h>
 * #include <dpp/unicode_emoji.h>
 * ```
 */
namespace unicode_emoji {

END;

/* This JSON is generated originally via the NPM package maintained by Discord themselves at https://www.npmjs.com/package/discord-emoji */
$emojis = json_decode(file_get_contents($url));
if ($emojis) {
    foreach ($emojis as $name=>$code) {
        if (preg_match("/^\d+/", $name)) {
            $name = "_" . $name;
        }
	$name = str_replace("-", "minus", $name);
	$name = str_replace("+", "plus", $name);
	$name = str_replace("ñ", "n", $name);
	if ($name == "new") {
		$name = "_new";
	}
        $header .= "	inline constexpr const char " .$name . "[] = \"$code\";\n";
    }
    $header .= "}\n};\n";
    file_put_contents("include/dpp/unicode_emoji.h", $header);
}
