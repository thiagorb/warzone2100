~/dev/peste/vendor/bin/peste '
    filter(fn ($l) => !preg_match("/^(--|==)/", $l)),
    map(fn ($l) =>
        $l === "{"
        ? (
            "["
        )
        : (
            $l === "}"
            ? "null\n]"
            : json_encode($l) . ","
        )
    )
' \
    | ~/dev/peste/vendor/bin/peste -j '
        map(function ($e) {
            array_pop($e);
            return $e;
        }),
        filter(function ($e) {
            foreach ($e as $l) {
                foreach (["/GFX::/", "/gfx_api/", "/glXCreateContext/", "/_ZN[^A-Z0-9]+re2/"] as $needle) {
                    if (preg_match($needle, $l)) {
                        return true;
                    }
                }
            }

            return false;
        })
    ' \
    | uniq | sort | uniq \
    | ~/dev/peste/vendor/bin/peste -r '
        decodeJson(),
        map(function ($a) {
            array_unshift($a, "{");
            array_push($a, "}");
            return implode(PHP_EOL, $a) . PHP_EOL;
        })
    '
