## OpenMP Project

### Crack MD5 hashs using wordlist/dictionary and OpenMP


Installing/compiling:

```
make
```

Command line arguments:
```
-d  -- dictionary to read from (lines separated with a newline "\n" character)
-h  -- hash to search for
-v  -- show threads processing
```



Example:
```
./crackpass -d dictionary.txt -h 098f6bcd4621d373cade4e832627b4f6 -v
```