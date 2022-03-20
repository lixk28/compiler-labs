# Readme for Lab1

## Build

If you build kric for the first time, you **have to** run:

```shell
make build
```

You don't have to run this command again, unless you manually delete the `build/` directory or one of its subdirectory that `make build` creates for you.

Each time you want to build kric, just run:

```shell
make
```

And the executable file `kric` goes into `build/bin`.

You can run the following command to clean all the objects and dependencies.

```shell
make clean
```

## Usage

You can use kric by:

```shell
build/bin/kric demo/demo.c
```

kric will do lexical analysis on `demo.c`, and print all the tokens and their types pair by pair on your terminal.

Or, you can use `>>` to redirect the ouput to a specified file like this:

```shell
build/bin/kric demo/demo.c >> tokens.txt
```

One more thing, you probably don't want to type the `build/bin/` prefix each time you run kric, you can export the path in your current shell. 

```shell
export PATH=build/bin:$PATH
```

Thus, you can just type kric to use kric :-) 

```shell
kric demo/demo.c
```