# fj

## Description
fj is a command-line tool that allows you to quickly navigate to frequently used directories using aliases.

## Usage
To use fj, simply type `fj <ALIAS>` in your terminal to navigate to the directory associated with the specified alias.

## Options
- `-c`, `--complete`: Displays a list of available completions for the provided alias, allowing you to choose from them interactively (requires [gum](https://github.com/charmbracelet/gum) but you could just replace it in the fj shell function).

Example:
`fj projects -c`

## Installation
1. Clone this repository to your local machine:

```sh
git clone https://github.com/pders01/fj.git
```

2. Change into the directory:

```sh
cd fj
```

3. Build the executable:

```sh 
make
```

4. Install fj:

```sh
make install
```

5. Source your shell configuration:

```sh
$ source ~/.{bashrc,zshenv}
```

6. Add some aliases to your `$HOME/.alias.d/paths` file.

```sh
alias projects=$HOME/Projects
```

### NOTE
If you use anything other than bash or zsh just tweak the shell function to your needs.
