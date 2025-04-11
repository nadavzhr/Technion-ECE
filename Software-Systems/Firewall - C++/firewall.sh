#!/bin/bash

# Save the rules passed by an argument via txt file
rules_file="$1"
# Save the packets passed as stdin via txt file
packets=$(</dev/stdin)

# Use grep to exclude empty lines and lines starting with '#'
filtered_rules=$(grep -Ev '^#|^$' "$rules_file")

# Remove text after '#' using sed
cleaned_rules=$(echo "$filtered_rules" | sed 's/#.*//')

valid_pkts=""

# Split the rules into an array of rules, line by line
# [each slot is a set of rules]
mapfile -t rules_array <<< "$cleaned_rules"

# Iterate over each line in the given relevant rules
for line in "${rules_array[@]}"; do
  	# Use awk to split the line by commas and whitespace
  	IFS=$'\n' read -rd '' -a rules_set <<< "$(echo "$line" | \
  			awk -F '[,[:space:]]+' '{for (i=1; i<=NF; i++) print $i}')"
  	# After each line was splitted, pass the packets through the rules
  	# Results are rules that fulfill all of the 4 rules in the line		
	valid_pkts+="$(echo "$packets" | \
		./firewall.exe "${rules_set[0]}" | \
		./firewall.exe "${rules_set[1]}" | \
		./firewall.exe "${rules_set[2]}" | \
		./firewall.exe "${rules_set[3]}" | sed 's/ //g')"$'\n'
done
# Sort and elliminate duplicates
echo "$(echo "$valid_pkts" | sed '/^[[:space:]]*$/d' | sort | uniq)"