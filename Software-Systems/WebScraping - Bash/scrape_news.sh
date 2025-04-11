#!/bin/bash

site="https://www.ynetnews.com/category/3082"
# Output the site contents into the data variable
# and redirect errors to junk channel
data=$(wget --no-check-certificate -O - $site 2>/dev/null)

# Get all articles on the front page
articles=$(echo "$data" | \
			grep -oP "https://(www.)?ynetnews.com/article/[0-9a-zA-Z]+" | \
			sort | uniq)

# Print the number of unique articles to stdout (redirection)
echo "$(echo "$articles" | wc -l)" > results.csv

politicians=("Netanyahu" "Gantz" "Bennett" "Peretz")

for article in $articles; do
	# Initialize a mentions array of size 4 for each article
	# Corresponding slots: Netanyahu Gantz Bennet Peretz
	mentions=(0 0 0 0)
	idx=0
	# Fill up the mentions histogram
	for name in "${politicians[@]}"; do
		content=$(wget --no-check-certificate -O - $article 2>/dev/null)
		mentions[$idx]=$(echo "$content" | grep -o "$name" | wc -l)
		(( idx++ ))
	done

	# Check if the current article's mentions array is empty or not
	all_zero=true # Assume emptiness
	for i in $mentions; do
		if [[ $i -ne 0 ]]; then
			all_zero=false # at least one politician was mentioned once
			break
		fi
	done

	# Print results accordingly
	if $all_zero; then
		echo "$article, -" >> results.csv
	else
		echo -n "$article, " >> results.csv
		for (( i=0; i<${#politicians[@]}; i++ )); do
			# Avoid unneccesary final comma and whitepsace
			if [[ i -eq ${#politicians[@]}-1 ]]; then
				echo "${politicians[i]}, ${mentions[i]}" >> results.csv
				break
			fi
			echo -n "${politicians[i]}, ${mentions[i]}, " >> results.csv
		done
	fi
done