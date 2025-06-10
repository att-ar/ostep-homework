## Q1

```shell
# cmd
./fork.py -s 10

    a
Action: a forks b
    a
    └──b
Action: a forks c
    a
    └──b
    └──c
Action: c EXITS
    a
    └──b
Action: a forks d
    a
    └──b
    └──d
Action: a forks e
    a
    └──b
    └──d
    └──e
```

## Q2

```shell
./fork.py -s 10 -a 100 -f 0.2 -F
# Low percentage = shallower tree since it is less likely to get a string of descendents to all fork

# Result: Height = 1 (every child just exited)

./fork.py -s 10 -a 100 -f 0.8 -F
# High percentage = deeper tree for the opposite reason

# Result: height = 4
```

## Q3

```shell
./fork.py -t -a 5 -s 3
                           Process Tree:
                               a

Action? a forks b
                               a
                               └── b
Action? b forks c
                               a
                               └── b
                                   └── c
Action? a forks d
                               a
                               ├── b
                               │   └── c
                               └── d
Action? d forks e
                               a
                               ├── b
                               │   └── c
                               └── d
                                   └── e
Action? b forks f
                               a
                               ├── b
                               │   ├── c
                               │   └── f
                               └── d
                                   └── e
```

## Q4

```shell
./fork.py -A a+b,b+c,c+d,c+e,c-
# creating orphaned proceses ^
# unix behaviour is to reparent to the closest 'subpreaper' ancestor
# the 'init' process is the highest subpreaper ancestor,

    a
Action: a forks b
    a
    └── b
Action: b forks c
    a
    └── b
        └── c
Action: c forks d
    a
    └── b
        └── c
            └── d
Action: c forks e
    a
    └── b
        └── c
            └── d
            └── e
Action: c EXITS
# based on no subreapers: (did not use the -R flag)
    a
    └── b
    └── d
    └── e

# With the R flag: goes to grandparent
Action: c EXITS
    a
    └── b
        └── d
        └── e
```

## Q5

```shell
./fork.py -F -s 10 -a 6

Action: a forks b
Action: a forks c
Action: c EXITS
Action: a forks d
Action: a forks e
Action: e forks f

    a
    └── b
    # └── c (exited)
    └── d
    └── e
        └── f
```

## Q6

```shell
./fork.py -F -s 25 -a 10 -t -R

Action?
Action?
Action?
Action?
Action?
Action?
Action?
Action?
Action?
Action?

                        Final Process Tree:
                               a
                               ├── c
                               │   └── g
                               │       └── h
                               └── f
```
