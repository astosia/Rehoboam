# pebble-fctx-compiler

Compiles SVG resources into a binary format for use with the [pebble-fctx](https://www.npmjs.com/package/pebble-fctx) drawing library.

### Release notes

##### v1.2.2
* Fixed crash bug when unicode-range attribute is not present.  Fix c/o stefanheule

##### v1.2.1
* Made output less verbose by default.
* Added error handling for fonts that exceed 64kb in size.
* Added installation and usage notes geared towards CloudPebble users.
* Made the presence of a resources subdirectory optional.

##### v1.2
* Added cap-height to the font metadata.  Compatible with [pebble-fctx](https://www.npmjs.com/package/pebble-fctx) version 1.6.

### Installation and invocation

This is a Node.js package.  To use it you must have node (with npm) installed on your computer.
See [nodejs.org](https://nodejs.org) for downloads and instructions.

#### For CloudPebble

Install `fctx-compiler` globally on your computer with npm:

    npm install -g pebble-fctx-compiler

And then, from the folder that contains your SVG font, invoke the compiler:

    fctx-compiler <your-file.svg> [-r <regex>]

Output will be written to the current directory, or if a `resources` subdirectory is present, output will be written to the `resources` directory.

To update to latest version of the compiler, run:

    npm update -g pebble-fctx-compiler

#### For local SDK

To install as a local `devDependency` within your pebble project:

    npm install pebble-fctx-compiler --save-dev

And then, from your pebble project directory, invoke the locally installed compiler:

    ./node_modules/.bin/fctx-compiler <path-to/your-file.svg> [-r <regex>]

Output will be written directly into the `resources` subdirectory, or if that directory does not exist (though it should), output will be written to the current directory.

If you have installed the compiler as a `devDependency` then the `pebble build` command should keep your packages, including `pebble-fctx-compiler` up to date, but if for some reason you need to update it, then, from your project directory, run:

    npm update pebble-fctx-compiler

### Usage

The compiler only looks for elements defined within the `<defs>` section of the SVG.  It can handle `<font>` elements and `<g d="...">` elements.  Output files are named according to the `id` property of the element.

The `-r <regex>` option is recommended for selecting a subset of glyphs from a font.

For example, an SVG font definition `<defs><font id="OpenSans-Regular"...> ... </font></defs>` will be output to `OpenSans-regular.ffont`.

And an SVG path element defined as `<defs><g id="hour-hand" d="..."> ... </g></defs>` will be output to `hour-hand.fpath`.

### Example workflow for converting a subset of glyphs from a font.

1. Open the font in [FontForge](https://fontforge.github.io/).
2. Select `File -> Generate Fonts...`, then select `SVG Font` as the output format, and finally click `Generate` to convert the font.
3. Open the SVG file in your favorite code or text editor to double check the `id` of the font and to do any formatting cleanup you like.
4. Invoke `fctx-compiler` with the `-r [0-9:AMP]` option to compile glyph data for just the digits 0 through 9, the colon, and the letters A, M and P.
