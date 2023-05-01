#!/bin/bash

# only works as a non-root user, exits with a ”must not be root” error
# gets a path directory or tar archive (*.tar.gz, *.tgz) as the main argument
# traverses all of it and its subpathectories/archives to find all files containing a certain RegEx in
# them and print their file names. (-n argument must be given for filename printing)
# the RegEx pattern argument is provided as the key-value flag -re "<the RegEx>".
# if tar archives are found, search for files in them as well do not leave any evidence that you
# unarchived anything anywhere on the filesystem. (i.e. remember to perform proper cleanup)
# print the number of found files. (-c argument must be given)
# check all arguments, both -c and -n may or may not have been given.


## Flags ##

set -eu         # exit on errors and unset variables
# set -x          # enable debug mode
set -o pipefail # set the exit status of a pipeline to the last (rightmost) command



## Variables ##

bool_n=""     # filename printing
bool_c=""     # count printing
re_arg=""     # RegEx
path_arg=""   # path to target path or .tar (main argument)
files=()      # array of files which fit the criteria  



## Functions ##

function cleanup_on_exit {

    echo -e "\nScript $0 is exiting. \nCleaning up all temporary files."
    
    if [[ -e "/tmp/temp/" ]]
    then
        rm -rf /tmp/temp/
    fi
}

# register trap
trap cleanup_on_exit EXIT # register trap


# exit the script if it's being executed as root user
function checkUser {
    if [[ $USER == "root" ]]
    then    
        echo -e "Error: root not allowed to run the script. \nExiting . . . " 
        exit 1
    fi
}

# traverse a folder or archive given as argument, search for fitting files
function traverse {

    path=""; re=""; 
    local fitting=()

    path=$1; re="$2"; 

    if [[ -f $path && $(grep -E -q "\.tar" $path) ]]   # check if file
    then
        echo -e "Error: path points to a file other than .tar archive."
        exit 3

    elif [[ -d $path ]]
    then
        :   # do nothing

    else    # then is .tar* archive

        mkdir "/tmp/temp/"

        if file $path | grep -E -q 'tar archive'   # check if .tar archive
        then
    
            tar -xf $path -C /tmp/temp

        elif file $path | grep -E -q 'gzip'        # check if .tar.gz archive
        then

            tar -xzf $path -C /tmp/temp

        fi

        path="/tmp/temp/"
        
    fi

    # find the files which fit the criteria
    local fitting=( $(find $path -type f -exec grep -E -l $re {} \;) )

    if [[ ${#fitting[@]} != 0 ]]   # if there are any fitting files (at least 1)
    then
         # remove "/tmp/temp" prefix from every filepath
        for i in "${!fitting[@]}"
        do
            fitting[i]=${fitting[i]#"/tmp/temp/"}
        done
    fi

    # return the fitting files
    echo "${fitting[@]}"
}





## Script ##

echo

# check the user
checkUser

# parse the script args
while [[ $# > 0 ]]
do
    case "$1" in

        -n)
            bool_n=1 ;;

        -c) 
            bool_c=1 ;;

        -re) 
            shift
            re_arg=$1 
            if [[ ! $re_arg ]]
            then
                echo "Error: no RegEx pattern provided."
                exit 4
            fi
            ;;

        *)
            if [[ ! "$1" ]]
            then
                echo "Error: target path (main argument) not provided."
                exit 2
            else
                path_arg=$1
            fi ;;
    esac

    shift

done


# call traverse & capture fitting files
files=$(traverse $path_arg $re_arg)

if [[ -z "${files[@]}" ]]
then
    echo -e "No fitting files found.\n"
else
    echo -e "Found fitting files.\n"
fi


if [[ $bool_n ]]     # if -n flag is set
then
    # display the filepaths
    for i in "${files[@]}"
    do
        echo "$i"
    done
fi


if [[ $bool_c ]]     # if -c flag is set
then
    # display the count
    count=$(echo $files | wc -w)
    echo -e "\nFile count: $count"
fi
