#!/bin/bash
## Save dir/file names for repetitive use - $1 = course number
grades_file="$1.txt"
stat_dir="$1_stat"
## Check for valid number of arguments
if [ "$#" -ne 1 ]; then
    echo "Wrong number of arguments" >&2
    exit 1
fi
## Check for existence of appropriate txt file in current dir only
if [[ ! -e "$grades_file" ]]; then
	echo "Course not found" >&2
	exit 1
fi
## Initialize/Reset the course stats directory
if [[ -d "./$stat_dir" ]]; then
	# Remove dir and all its contents
  	rm -rf "./$stat_dir"
fi
mkdir "./$stat_dir"

## Start operating on the grades files and perform stats calculation
# Build the histogram with appropriate num of bins (set by default value = 10)
./hist.exe "./$grades_file" > "./$stat_dir"/histogram.txt

# Perform statistics calculations and output results to a stats file
(cat $grades_file | ./mean.exe; cat $grades_file | ./median.exe; \
	cat $grades_file | ./min.exe; cat $grades_file | ./max.exe) | \
	tr '\n' '\t' > $stat_dir/statistics.txt

## Calc the percentage of students who passed the exam
# Use hist program and divide the grades to 11 bins using a tmp file
./hist.exe "./$grades_file" -n_bins 11 > "./$stat_dir"/temp_f.txt
# Analyze data in the 2nd column of the file (amount of students in each bin)
tot_students=$(awk '{ tot += $2 } END { print tot }' \
				"./$stat_dir"/temp_f.txt)
# Students who pass (55+) are in the 7th bin and forward
num_pass=$(awk 'FNR >= 7 { pass += $2 } END { print pass }' \
				"./$stat_dir"/temp_f.txt)

percentage=$(( $num_pass * 100 / $tot_students ))
echo -e "${percentage}%" >> "./$stat_dir"/statistics.txt
# Delete the tmp file
rm -rf "./$stat_dir"/temp_f.txt