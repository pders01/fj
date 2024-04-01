fj() {
	# Check if the 'gum' command is available
	if ! command -v gum &>/dev/null; then
		echo "Error: 'gum' command not found. Please install it first." >&2
		return 1
	fi

	local alias=""
	local completions=""

	# Check if the -c option is supplied
	if [[ "$2" == "-c" || "$2" == "--complete" ]]; then
		# Check if the 'gum' command is available
		if ! command -v gum &>/dev/null; then
			echo "Error: 'gum' command not found. Please install it first." >&2
			return 1
		fi

		# Fetch completions based on the provided alias
		completions=$(command fj -complete "$1")

		# Pass completions to gum for selection
		selected=$(printf "%s" "$completions" | gum choose)

		if [[ -n "$selected" ]]; then
			cd "$selected" || return 1
		fi
		return 0
	fi

	# Check if exactly one argument is provided (fj <ALIAS>)
	if [[ "$#" == 1 ]]; then
		alias="$1"
	else
		cat <<EOF >&2
Usage: $(basename "$0") <ALIAS> [-c|--complete]

Reads your aliases under /home/$USER/.alias.d/paths and lets you easily change into them.
EOF
		return 1
	fi

	# Perform the default behavior if no options are provided
	local path=$(command fj "$alias")
	cd "$path" || return 1
}
