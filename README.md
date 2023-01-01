# JSONParse
A command line json parser build upon [cJSON](https://github.com/DaveGamble/cJSON).
## Usage:
```shell
Usage: JSONParse [-htkl] [-j json] [-p path]
   -h Show this help
   -j JSON string to work on. Read from stdin if not provided
   -p JSON key path string, e.g. '[\"hello\"][\"world\"]'
   -t Output type instead of the value
       types: object array string boolean number null undefined
   -l Output array length instead of the value
   -k Output object key array instead of the value
```
