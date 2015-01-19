## OpenWeather console app

### Building

Requires boost, then build with `make`. Run like:

`./ow "paris, france" london "arendal, norway" stockholm "new york"`

Output will be sorted alphabetically by location, unless the option `--temp` is given, which will
make it sort by temperature (coldest to warmest)