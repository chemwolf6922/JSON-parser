# JSONParse
A command line json parser build upon [cJSON](https://github.com/DaveGamble/cJSON).
## Usage:
```shell
Usage: JSONParse [-htkl] [-j json] [-p path]
   -h Show this help
   -j JSON string to work on. Read from stdin if not provided
   -p JSON key path string, e.g. '["hello"]["world"]'
   -t Output type instead of the value
       types: object array string boolean number null undefined
   -l Output array length instead of the value
   -k Output object key array instead of the value
```
## Examples:
* Parse JSON with keys and print value
```shell
> ./JSONParse -j '{"hello":{"world":true}}' -p '["hello"]["world"]'
true
```
* Parse JSON with keys but the item does not exist
```shell
> ./JSONParse -j '{"hello":123}' -p '["hello"]["world"]'
(outputs nothing)
```
* Show type of an item
```shell
> ./JSONParse -t -j '{"hello":{"world":true}}' -p '["hello"]["world"]'
boolean
```
* Show type of an item without parsing
```shell
> ./JSONParse -t -j '{"hello":{"world":true}}'
object
```
* Show length of an array
```shell
> ./JSONParse -l -j '{"hello":[0,1,2,3,4]}' -p '["hello"]'
5
```
* Show length of an array without parsing
```shell
> ./JSONParse -l -j '[0,1,2,3,4]'
5
```
* List keys of an object
```shell
> ./JSONParse -k -j '{"hello":1,"world":2}'
["hello","world"]
```
* Take input from stdin
```shell
> echo '{"hello":"world"}' | ./JSONParse -p '["hello"]'
"world"
```

