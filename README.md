# Features
- adding current path to PATH env variable if it isn't there
- removing current path from PATH env variable if it's there
- adding provided relative or absolute path to PATH env variable if it isn't there
- removing provided relative or absolute path from PATH env variable if it's there

# Example usage
- adding current path to PATH env variable if it isn't there
```console
C:\some\path>.\add2path
Added path: C:\some\path
```

- removing current path from PATH env variable if it's there
```console
C:\some\path>.\add2path
Deleted path: C:\some\path
```

- adding provided relative or absolute path to PATH env variable if it isn't there
```console
C:\some\path>.\add2path D:\other\path
Added path: D:\other\path
```


- removing provided relative or absolute path from PATH env variable if it's there
```console
D:\other>.\add2path path
Deleted path: D:\other\path
```