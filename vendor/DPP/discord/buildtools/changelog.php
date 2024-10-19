<?php

// D++ changelog generator, saves 15 minutes for each release :-)

// Categories, in display order
$catgroup = [
    '💣 Breaking Changes' => [],
    '✨ New Features' => [],
    '🐞 Bug Fixes' => [],
    '🚀 Performance Improvements' => [],
    '♻️ Refactoring' => [],
    '🚨 Testing' => [],
    '📚 Documentation' => [],
    '💎 Style Changes' => [],
    '🔧 Chore' => [],
    '📜 Miscellaneous Changes' => []
];

// Pattern list
$categories = [
    'break' => '💣 Breaking Changes',
    'breaking' => '💣 Breaking Changes',
    'feat' => '✨ New Features',
    'feature' => '✨ New Features',
    'add' => '✨ New Features',
    'added' => '✨ New Features',
    'fix' => '🐞 Bug Fixes',
    'bug' => '🐞 Bug Fixes',
    'bugfix' => '🐞 Bug Fixes',
    'fixed' => '🐞 Bug Fixes',
    'fixes' => '🐞 Bug Fixes',
    'perf' => '🚀 Performance Improvements',
    'performance' => '🚀 Performance Improvements',
    'impro' => '♻️ Refactoring',
    'improved' => '♻️ Refactoring',
    'improvement' => '♻️ Refactoring',
    'refactor' => '♻️ Refactoring',
    'refactored' => '♻️ Refactoring',
    'refactoring' => '♻️ Refactoring',
    'deprecated' => '♻️ Refactoring',
    'deprecate' => '♻️ Refactoring',
    'remove' => '♻️ Refactoring',
    'change' => '♻️ Refactoring',
    'changed' => '♻️ Refactoring',
    'test' => '🚨 Testing',
    'tests' => '🚨 Testing',
    'testing' => '🚨 Testing',
    'ci' => '👷 Build/CI',
    'build' => '👷 Build/CI',
    'docs' => '📚 Documentation',
    'documentation' => '📚 Documentation',
    'style' => '💎 Style Changes',
    'chore' => '🔧 Chore',
    'misc' => '📜 Miscellaneous Changes',
    'update' => '📜 Miscellaneous Changes',
    'updated' => '📜 Miscellaneous Changes',
];

$changelog = [];
$githubstyle = true;
if (count($argv) > 2 && $argv[1] == '--discord') {
    $githubstyle = false;
}
$errors = [];

// Magic sauce
exec("git log --oneline --format=\"%s\" $(git log --no-walk --tags | head -n1 | cut -d ' ' -f 2)..HEAD | grep -v '^Merge '", $changelog);

// Case insensitive removal of duplicates
$changelog = array_intersect_key($changelog, array_unique(array_map("strtolower", $changelog)));

// remove duplicates where two entries are the same but one ends with a GitHub pull request link
foreach ($changelog as $item) {
    $entryWithoutPrLink = preg_replace('/( \(#\d+\))$/', '', $item);
    if ($entryWithoutPrLink === $item) {
        continue;
    }

    // if $item ends with (#123)
    foreach ($changelog as $key => $change) {
        if ($entryWithoutPrLink === $change) {
            unset($changelog[$key]);
            break;
        }
    }
}

function add_change(string $change, string $category, string $scope = null) {
    global $catgroup;

    if (isset($scope) && !empty($scope)) {
        // Group by feature inside the section
        if (!isset($catgroup[$category][$scope])) {
            $catgroup[$category][$scope] = [];
        }
        $catgroup[$category][$scope][] = $change;
        return;
    }
    $catgroup[$category][] = $change;
}

foreach ($changelog as $change) {

    // Wrap anything that looks like a symbol name in backticks
    $change = preg_replace('/([a-zA-Z][\w_\/\-]+\.\w+|\S+\(\)|\w+::\w+|dpp::\w+|utility::\w+|(\w+_\w+)+)/', '`$1`', $change);
    $change = preg_replace("/vs(\d+)/", "Visual Studio $1", $change);
    $change = preg_replace("/\bfaq\b/", "FAQ", $change);
    $change = preg_replace("/\bdiscord\b/", "Discord", $change);
    $change = preg_replace("/\bmicrosoft\b/", "Microsoft", $change);
    $change = preg_replace("/\bwindows\b/", "Windows", $change);
    $change = preg_replace("/\blinux\b/", "Linux", $change);
    $change = preg_replace("/\sarm(\d+)\s/i", ' ARM$1 ', $change);
    $change = preg_replace("/\b(was|is|wo)nt\b/i", '$1n\'t', $change);
    $change = preg_replace("/\bfreebsd\b/", 'FreeBSD', $change);
    $change = preg_replace("/``/", "`", $change);
    $change = trim($change);

    $matched = false;
    $matches = [];
    // Extract leading category section
    if (preg_match("/^((?:(?:[\w_]+(?:\([\w_]+\))?+)(?:[\s]*[,\/][\s]*)?)+):/i", $change, $matches) || preg_match("/^\[((?:(?:[\w_]+(?:\([\w_]+\))?+)(?:[\s]*[,\/][\s]*)?)+)\](?:\s*:)?/i", $change, $matches)) {
        $categorysection = $matches[0];
        $changecategories = $matches[1];
        $matchflags = PREG_SET_ORDER | PREG_UNMATCHED_AS_NULL;
        // Extract each category and scope
        if (preg_match_all("/(?:[\s]*)([\w_]+)(?:\(([\w_]+)\))?(?:[\s]*)(?:[,\/]+)?/i", $changecategories, $matches, $matchflags) !== false) {
            /**
             * Given a commit "foo, bar(foobar): add many foos and bars" :
             * $matches is [
             *     0 => [[0] => 'foo,', [1] => 'foo', [2] => null],
             *     1 => [[0] => ' bar(foobar)', [1] => 'bar', [2] => 'foobar'],
             * ]
             * In other words, for a matched category N, matches[N][1] is the category, matches[N][2] is the scope
             */
            $header = $matches[0][1];
            $scope = $matches[0][2];
            $change = trim(substr($change, strlen($categorysection)));
            // List in breaking if present
            foreach ($matches as $nb => $match) {
                if ($nb == 0) // Skip the first category which will be added anyways
                    continue;
                $category = $match[1];
                if (isset($categories[$category]) && $categories[$category] === '💣 Breaking Changes')
                    add_change($change, $categories[$category], $scope);
            }
            if (!isset($categories[$header])) {
                $errors[] = "could not find category \"" . $header . "\" for commit \"" . $change . "\", adding it to misc";
                $header = $categories['misc'];
            }
            else {
                $header = $categories[$header];
            }
            if (!isset($catgroup[$header])) {
                $catgroup[$header] = [];
            }
            $matched = true;
            // Ignore version bumps
            if (!preg_match("/^(version )?bump/i", $change)) {
                add_change($change, $header, $scope);
            }
        }
    }
    if (!$matched) { // Could not parse category section, try keywords
        // Match keywords against categories
        foreach ($categories as $cat => $header) {
            // Purposefully ignored: comments that are one word, merge commits, and version bumps
            if (strpos($change, ' ') === false || preg_match("/^Merge (branch|pull request|remote-tracking branch) /", $change) || preg_match("/^(version )?bump/i", $change)) {
                $matched = true;
                break;
            }
            if (preg_match("/^" . $cat . " /i", $change)) {
                if (!isset($catgroup[$header])) {
                    $catgroup[$header] = [];
                }
                $matched = true;
                $catgroup[$header][] = $change;
                break;
            }
        }
    }
    if (!$matched) {
        $errors[] = "could not guess category for commit \"" . $change . "\", adding it to misc";
        $header = $categories['misc'];
        if (!isset($catgroup[$header])) {
            $catgroup[$header] = [];
        }
        $matched = true;
        $catgroup[$header][] = $change;
    }
}

// Leadin
if ($githubstyle) {
    echo "The changelog is listed below:\n\nRelease Changelog\n===========\n";
} else {
    echo "The changelog is listed below:\n\n## Release Changelog\n";
}

function print_change(string $change) {
    global $githubstyle;

    // Exclude bad commit messages like 'typo fix', 'test push' etc by pattern
    if (!preg_match("/^(typo|test|fix)\s\w+$/", $change) && strpos($change, ' ') !== false) {
        echo ($githubstyle ? '-' : '•') . ' ' . ucfirst(str_replace('@', '', $change)) . "\n";
    }
}

// Output tidy formatting
foreach ($catgroup as $cat => $list) {
    if (!empty($list)) {
        echo "\n" . ($githubstyle ? '## ' : '### ') . $cat . "\n";
        foreach ($list as $key => $item) {
            if (is_array($item)) {
                foreach ($item as $change) {
                    print_change("$key: $change");
                }
            }
            else {
                print_change($item);
            }
        }
    }
}

// Leadout
echo "\n\n**Thank you for using D++!**\n\n";
if (!$githubstyle) {
    $version = $argv[2];
    echo 'The ' . $version . ' download can be found here: <https://dl.dpp.dev/' . $version . '>';
    echo "\n";
}

/*
 * Disabled as it generates pages and pages of stack traces, making it
 * extremely difficult to copy and paste the error log when running this
 * on the command line for sending discord announcement changelogs.
 *
 * foreach ($errors as $err) {
 *     trigger_error($err, E_USER_WARNING);
 * }
 *
 */
