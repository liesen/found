# found is the dual of find

`found` is the dual of `find`: `find` searches for files in a directory hierarchy and evaluates an expression on the files it finds while `found` listens for changes in the directory hierarchy and evaluates the expression on files that have changed.
 
A common use case of `found` is to watch for changes in source files and compile the tree. For `found` itself this looks like: `found . –iname \*.c –exec make \;`. (Actually, to keep compiling, use: `while true; do found . -iname \*.c -exec make \; -exit; done` until `-restart` is implemented.)
 
## EXPRESSIONS
 
`found` mimics the options of `find`, and adds a few of its own. However, some options that `find` uses do not make sense in `found`, e.g. the modified since predicates (`-mtime`) and terminating `-exec` with `+`.
 
### TESTS
 
`-event {modify,create,delete}` tests that a file was modified, created or deleted.
 
`-name`, `-iname`, `-ipath`, `-type`, `-true`, `-false` works as in `find`.
 
### ACTIONS
 
`-exec command {} ;`, `-print` works as in `find`.

`-exit` terminates `found`.
