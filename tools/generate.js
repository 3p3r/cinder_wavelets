/*!
 * A utility to generate C++ color palette source for Cinder STFT project.
 * 
 * Usage: generate.js [... color palette URLs (space separated) ...] > out.cpp
 *
 * A sample palette URL can be found at:
 * http://www.ncl.ucar.edu/Document/Graphics/ColorTables/Files/matlab_jet.rgb
 * To get more paletts links take a look at:
 * http://www.ncl.ucar.edu/Document/Graphics/color_table_gallery.shtml
 */

//! Will contain palette URLs to download
var links = [];

//! Let's extract all valid URLs from argv
process.argv.forEach(function (val, index) {
    if (index < 2) return;
    links.push(val);
});

//! Do we have something to process?
if (links.length == 0)
{
    console.log("No user supplied parameters were found. Exiting.");
    process.exit(1);
}

//! #include's
var http = require('http');
var fs = require('fs');
var readline = require('readline');

/*!
 * \name download
 * \brief downloads a supplied URL into a file
 * and fires "cb" on finish.
 * \see http://stackoverflow.com/a/17676794/1055628
 */
var download = function (url, dest, cb) {
    var file = fs.createWriteStream(dest);
    var request = http.get(url, function (response) {
        response.pipe(file);
        file.on('finish', function () {
            file.close(cb);
        });
    });
}

/*!
 * \name readLineByLine
 * \brief reads a filename supplied by "dest", line
 * by line. Fires "cb" on each line, suppling the line
 * string to it. Fires "finish_cb" on finish.
 * \see http://stackoverflow.com/a/15554600/1055628
 */
var readLineByLine = function (dest, cb, finish_cb) {
    var rd = readline.createInterface({
        input: fs.createReadStream(dest),
        output: process.stdout,
        terminal: false
    });

    rd.on('line', function (line) {
        cb(line);
    });

    rd.on('close', finish_cb);
}

/*!
 * \name getFileName
 * \brief extracts a filename out of a URL.
 * \see http://befused.com/javascript/get-filename-url
 */
var getFileName = function (url) {
    url = url.substring(0, (url.indexOf("#") == -1) ? url.length : url.indexOf("#"));
    url = url.substring(0, (url.indexOf("?") == -1) ? url.length : url.indexOf("?"));
    url = url.substring(url.lastIndexOf("/") + 1, url.length);

    return url;
}

/*!
 * \name cleanArray
 * \brief cleans the array from empty elements.
 * \see http://stackoverflow.com/a/281335/1055628
 */
var cleanArray = function (actual) {
    var newArray = new Array();
    for (var i = 0; i < actual.length; i++) {
        if (actual[i]) {
            newArray.push(actual[i]);
        }
    }
    return newArray;
}

/*!
 * \name capitalizeFirstLetter
 * \see http://stackoverflow.com/a/1026087/1055628
 */
var capitalizeFirstLetter = function (string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

// Iterate through supplied links and generate color palettes
links.forEach(function (link) {
    // Get the filename out of the URL
    var _filename = getFileName(link);
    // Download the file
    download(link, _filename, function () {

		var _paletteName = "";
		var _paletteNameArr = (_filename.split("."))[0].split("_");
		_paletteNameArr.forEach(function (piece) {
			_paletteName += capitalizeFirstLetter(piece);
		});
	
        // Generate the first few lines of the source
        console.log(""); //GLSL
        console.log("// Generated from:"); //GLSL
        console.log("// " + link); //GLSL
        console.log("// Palette Name: " + _paletteName); //GLSL
        console.log(""); //GLSL

        // Keep track of where we are
        var _lineCount = 0;
        var _numColor = 0;

        // Read downloaded file line by line
        readLineByLine(_filename, function (line) {
            if (line.indexOf("ncolor") == 0) {

                // First line always contains count of colors
                _numColor = cleanArray(line.split(/\s+/))[2];

                console.log("#define PALETTE_SIZE " + _numColor); //GLSL
                console.log("const vec3 palette[PALETTE_SIZE] = vec3[PALETTE_SIZE] ("); //GLSL

            } else if (line.indexOf("#") == 0) {
                return; /* no op */
            } else {
                _lineCount++;
                var colors = cleanArray(line.split(/\s+/));

                var r = 0;
                var g = 0;
                var b = 0;

                // Some palettes are in [0, 1] range and some are [0, 255]
                if (colors[0] > 1 || colors[1] > 1 || colors[2] > 1) {
                    r = (colors[0] / 255.0).toPrecision(5);
                    g = (colors[1] / 255.0).toPrecision(5);
                    b = (colors[2] / 255.0).toPrecision(5);
                } else {
                    r = colors[0];
                    g = colors[1];
                    b = colors[2];
                }

                console.log("\tvec3(" + r + ", " + g + ", " + b + ")" + ((_lineCount != _numColor) ? "," : "")); //GLSL

                // Done.
                if (_lineCount == _numColor) {
                    console.log(");"); //GLSL
                }
            }
        }, function () {
            // Clean up downloaded file.
            fs.unlink(_filename);
        });
    });
});
