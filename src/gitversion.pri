APP_VER_SECOND  = 0

# Need to discard STDERR so get path to NULL device
win32 {
    NULL_DEVICE = NUL # Windows doesn't have /dev/null but has NUL
} else {
    NULL_DEVICE = /dev/null
}

# Need to call git with manually specified paths to repository
BASE_GIT_COMMAND = git rev-list HEAD --count

# Trying to get version from git tag / revision
APP_VER_SECOND = $$system($$BASE_GIT_COMMAND 2> $$NULL_DEVICE)

# Adding C preprocessor #DEFINE so we can use it in C++ code
# also here we want full version on every system so using GIT_VERSION
DEFINES += APP_VER_SECOND=\\\"$$APP_VER_SECOND\\\"

#message($$APP_VER_FIRST)
#message($$APP_VER_SECOND)

