<?php

chdir('buildtools');

require __DIR__ . '/vendor/autoload.php';

use Dpp\StructGeneratorInterface;

if (count($argv) < 2) {
    die("You must specify a generator type\n");
} else {
    $generatorName = $argv[1];
    $generator = new $generatorName();
}

chdir('..');

/* Get the content of all cluster source files into an array */
exec("cat src/dpp/cluster/*.cpp", $clustercpp);

/* These methods have signatures incompatible with this script */
$blacklist = [
];

/* The script cannot determine the correct return type of these methods,
 * so we specify it by hand here.
 */
$forcedReturn = [
    'direct_message_create' => 'message',
    'guild_get_members' => 'guild_member_map',
    'guild_search_members' => 'guild_member_map',
    'message_create' => 'message',
    'message_edit' => 'message',
    'message_add_reaction' => 'confirmation',
    'message_delete_reaction' => 'confirmation',
    'message_delete_reaction_emoji' => 'confirmation',
    'message_delete_all_reactions' => 'confirmation',
    'message_delete_own_reaction' => 'confirmation',
    'channel_edit_permissions' => 'confirmation',
    'channel_typing' => 'confirmation',
    'message_get_reactions' => 'emoji_map',
    'thread_create_in_forum' => 'thread',
    'threads_get_private_archived' => 'thread_map',
    'threads_get_public_archived' => 'thread_map',
    'threads_get_active' => 'active_threads',
    'user_get_cached' => 'user_identified',
    'application_role_connection_get' => 'application_role_connection',
    'application_role_connection_update' => 'application_role_connection'
];

/* Get the contents of cluster.h into an array */
$header = explode("\n", file_get_contents('include/dpp/cluster.h'));

/* Finite state machine state constants */
const STATE_SEARCH_FOR_FUNCTION = 0;
const STATE_IN_FUNCTION = 1;
const STATE_END_OF_FUNCTION = 2;

$state = STATE_SEARCH_FOR_FUNCTION;
$currentFunction = $parameters = $returnType = '';
$content = $generator->generateHeaderStart();
$cppcontent = $generator->generatecppStart();

if (!$generator->checkForChanges()) {
    exit(0);
}

$lastFunc = '<none>';
$l = 0;
/* Scan every line of the C++ source */
foreach ($clustercpp as $cpp) {
    $l++;
    /* Look for declaration of function body */
    if ($state == STATE_SEARCH_FOR_FUNCTION &&
        preg_match('/^\s*void\s+cluster::([^(]+)\s*\((.*)command_completion_event_t\s*callback\s*\)/', $cpp, $matches)) {
        $currentFunction = $matches[1];
        $parameters = preg_replace('/,\s*$/', '', $matches[2]);
        if (!in_array($currentFunction, $blacklist)) {
            $state = STATE_IN_FUNCTION;
        }
        /* Scan function body */
    } elseif ($state == STATE_IN_FUNCTION) {
        /* End of function */
        if (preg_match('/^\}\s*$/', $cpp)) {
            $state = STATE_END_OF_FUNCTION;
            /* look for the return type of the method */
        } elseif (preg_match('/rest_request<([^>]+)>/', $cpp, $matches)) {
            /* rest_request<T> */
            $returnType = $matches[1];
        } elseif (preg_match('/rest_request_list<([^>]+)>/', $cpp, $matches)) {
            /* rest_request_list<T> */
            $returnType = $matches[1] . '_map';
        } elseif (preg_match('/callback\(confirmation_callback_t\(\w+, ([^(]+)\(.*, \w+\)\)/', $cpp, $matches)) {
            /* confirmation_callback_t */
            $returnType = $matches[1];
        } elseif (!empty($forcedReturn[$currentFunction])) {
            /* Forced return type */
            $returnType = $forcedReturn[$currentFunction];
        }
    }
    /* Completed parsing of function body */
    if ($state == STATE_END_OF_FUNCTION && !empty($currentFunction) && !empty($returnType)) {
        if (!in_array($currentFunction, $blacklist)) {
            $parameterList = explode(',', $parameters);
            $parameterNames = [];
            $parameterTypes = [];
            foreach ($parameterList as $parameter) {
                $parts = explode(' ', trim($parameter));
                $name = trim(preg_replace('/[\s\*\&]+/', '', $parts[count($parts) - 1]));
                $parameterNames[] = $name;
                $parameterTypes[] = trim(substr($parameter, 0, strlen($parameter) - strlen($name)));
            }
            $content .= getComments($generator, $currentFunction, $returnType, $parameterNames) . "\n";
            $fullParameters = getFullParameters($currentFunction, $parameterNames);
            $parameterNames = trim(join(', ', $parameterNames));
            $parameterTypes = trim(join(', ', $parameterTypes));
            if (!empty($parameterNames)) {
                $parameterNames = ', ' . $parameterNames;
            }
            $noDefaults = $parameters;
            $parameters = !empty($fullParameters) ? $fullParameters : $parameters;
            $content .= $generator->generateHeaderDef($returnType, $currentFunction, $parameters, $noDefaults, $parameterTypes, $parameterNames);
            $cppcontent .= $generator->generateCppDef($returnType, $currentFunction, $parameters, $noDefaults, $parameterTypes, $parameterNames);
        }
        $lastFunc = $currentFunction;
        $currentFunction = $parameters = $returnType = '';
        $state = STATE_SEARCH_FOR_FUNCTION;
    }
}
if ($state != STATE_SEARCH_FOR_FUNCTION) {
    die("\n\n\nBuilding headers is broken ($l) - state machine finished in the middle of function $currentFunction (previous $lastFunc) with parameters $parameters rv $returnType state=$state\n\n\n");
}

$content .= <<<EOT

/* End of auto-generated definitions */

EOT;
$cppcontent .= <<<EOT

};

/* End of auto-generated definitions */

EOT;

/**
 * @brief Get parameters of a function with defaults
 * @param string $currentFunction Current function name
 * @param array $parameters Parameter names
 * @return string Parameter list
 */
function getFullParameters(string $currentFunction, array $parameters): string
{
    global $header;
    foreach ($header as $line) {
        if (preg_match('/^\s*void\s+' . $currentFunction . '\s*\((.*' . join('.*', $parameters) . '.*)command_completion_event_t\s*callback\s*/', $line, $matches)) {
            return preg_replace('/,\s*$/', '', $matches[1]);
        }
    }
    return '';
}

/**
 * @brief Get the comment block of a function.
 * Adds see/return doxygen tags
 * @param string $currentFunction function name
 * @param string $returnType Return type of function
 * @param array $parameters Parameter names
 * @return string Comment block
 */
function getComments(StructGeneratorInterface $generator, string $currentFunction, string $returnType, array $parameters): string
{
    global $header;
    /* First find the function */
    foreach ($header as $i => $line) {
        if (preg_match('/^\s*void\s+' . $currentFunction . '\s*\(.*' . join('.*', $parameters) . '.*command_completion_event_t\s*callback\s*/', $line)) {
            /* Backpeddle */
            $lineIndex = 1;
            for ($n = $i; $n != 0; --$n, $lineIndex++) {
                $header[$n] = preg_replace('/^\t+/', '', $header[$n]);
                $header[$n] = preg_replace('/@see (.+?)$/', '@see dpp::cluster::' . $currentFunction . "\n * @see \\1", $header[$n]);
                $header[$n] = preg_replace('/@param callback .*$/', '@return ' . $returnType . ' returned object on completion', $header[$n]);
                if (preg_match('/\s*\* On success /i', $header[$n])) {
                    $header[$n] = "";
                }
                if (preg_match('/\s*\/\*\*\s*$/', $header[$n])) {
                    $part = array_slice($header, $n, $lineIndex - 1);
                    array_splice($part, count($part) - 1, 0, $generator->getCommentArray());
                    return str_replace("\n\n", "\n", join("\n", $part));
                }
            }
            return '';
        }
    }
    return '';
}

/* Finished parsing, output autogenerated files */
$generator->saveHeader($content);
$generator->savecpp($cppcontent);
