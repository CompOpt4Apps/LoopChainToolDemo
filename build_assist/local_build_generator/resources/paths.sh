
is_sourced="yes"
if [ "$0" = "$BASH_SOURCE" ]
then
  is_sourced="no"
fi

# Paths to add
LIB_ADD=$(pwd)/install/lib
INC_ADD=$(pwd)/install/include

# Mode constants
appendmode="append"
overwritemode="overwrite"
prependmode="prepend"

# default mode is append
mode="$appendmode"

while [ $# -ge 1 ]
do
  key="$1"

  case $key in
      -a|--append)
        mode="$appendmode"
        ;;
      -p|--prepend)
        mode="$prependmode"
        ;;
      -o|--overwrite)
        mode="$overwritemode"
        ;;
      -h|--help)
        echo "Valid options are:"
        echo "--append    -a: Append to existing path variables"
        echo "--prepend   -p: Prepend to existing path variables"
        echo "--overwrite -o: Overwrite existing path variables"
        echo "--help      -h: Prints this help message"
        echo "This script needs to be sourced:"
        echo "source ./paths.sh [options]"

        if [[ "$is_sourced" == "yes" ]]
        then
          return 0
        else
          exit
        fi
        ;;
      *)
        # unknown option
        echo "Unknown option: $1"
        echo "Valid options are:"
        echo "--append    -a: Append to existing path variables"
        echo "--prepend   -p: Prepend to existing path variables"
        echo "--overwrite -o: Overwrite existing path variables"
        echo "--help      -h: Prints this help message"
        echo "This script needs to be sourced:"
        echo "source ./paths.sh [options]"

        if [[ "$is_sourced" == "yes" ]]
        then
          return 0
        else
          exit
        fi
        ;;
  esac

  shift # past argument or value
done

if [[ "$is_sourced" != "yes" ]]
then
  echo "This script needs to be sourced:"
  echo "source ./paths.sh [options]"
  exit
fi

case $mode in
  # append paths
  $appendmode)
    export CPATH=$CPATH:$INC_ADD
    export C_INCLUDE_PATH=$C_INCLUDE_PATH:$INC_ADD
    export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$INC_ADD

    export LIBRARY_PATH=$LIBRARY_PATH:$LIB_ADD
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIB_ADD
    export LIBRARY_PATH=$LIBRARY_PATH:$LIB_ADD
    ;;

  # prepend paths
  $prependmode)
    export CPATH=$INC_ADD:$CPATH
    export C_INCLUDE_PATH=$INC_ADD:$C_INCLUDE_PATH
    export CPLUS_INCLUDE_PATH=$INC_ADD:$CPLUS_INCLUDE_PATH

    export LIBRARY_PATH=$LIB_ADD:$LIBRARY_PATH
    export LD_LIBRARY_PATH=$LIB_ADD:$LD_LIBRARY_PATH
    export LIBRARY_PATH=$LIB_ADD:$LIBRARY_PATH
    ;;

  # overwrite paths
  $overwritemode)
    export CPATH=$INC_ADD
    export C_INCLUDE_PATH=$INC_ADD
    export CPLUS_INCLUDE_PATH=$INC_ADD

    export LIBRARY_PATH=$LIB_ADD
    export LD_LIBRARY_PATH=$LIB_ADD
    export LIBRARY_PATH=$LIB_ADD
    ;;

  *)
    echo Unknown mode: $mode
    return -1
esac
