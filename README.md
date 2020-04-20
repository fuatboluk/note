# note

    Note - Write, Read, Delete, and List Notes by Group

    Options:
        -h | --help	        program usage
        -g | --group        used with the all option
        -c | --content      used only with the write option

        -r | --read         note read               | name
        -w | --write        note write or update    | name and content
        -d | --delete       note delete             | name
        -l | --list         list notes in group     | key or all

    Usage  :
        note -g <group> -l all
        note -g <group> -r <name>
        note -g <group> -d <name>
        note -g <group> -w <name> -c <content>


## Compilation Dependencies

*  gcc

*  pkg-config

*  libdb-devel



## Compilation on Linux


**Open a Terminal**

`git clone https://gitlab.com/fuatboluk/note.git`

**Change Directory**

`cd note`

**Compile**

`make`

**Install**

`sudo make install`

**Happy Usages For All :)**

## End