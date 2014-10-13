
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef LB_FILESYSTEM_H
#define LB_FILESYSTEM_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

/* An enumeration of the API-specific errors. */
enum LBFilesystemError
{
    /* No error occured */
    LBFilesystemNoError,
    /* Failed to allocate memory */
    LBFilesystemAllocationFailure,
	/* A path that does not exist was passed to a function */
	LBFilesystemPathNotFound,
	/* A directory with the specified path already exists */
	LBFilesystemDirectoryAlreadyExists,
	/* An error occured but the type of error could not be determined */
	LBFilesystemUnknownError
};
typedef enum LBFilesystemError LBFilesystemError;

typedef void* LBFilesystem;

/** Constructs a new filesystem object.
  * @return a pointer to the newly created filesystem object if successful,
  * else returns a pointer identifying a non-constructed filesystem object.
  * Use lb_filesystem_constructed to find out if an object was
  * constructed or not.
  */
LBFilesystem lb_filesystem_construct( void );

/** Destructs a previously constructed filesystem object.
  * Destructing a filesystem object will deallocate all the resources
  * associated with it.
  * @param filesystem a pointer to the filesystem object to destruct.
  */
void lb_filesystem_destruct( LBFilesystem filesystem );

/** Check for successfully constructed filesystem objects.
  * @param filesystem a pointer returned by lb_filesystem_construct
  * @return 1 if filesystem points to a successfully constructed
  * filesystem object or 0 if it doesn't.
  */
int lb_filesystem_constructed( LBFilesystem filesystem );


/** Check if the specified path refers to a file.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @param path a string specifying the path to check.
  * @return 1 if path refers to a file or 0 if it doesn't.
  */
int lb_filesystem_path_is_file( LBFilesystem filesystem, const char* path );

/** Returns the file size of the file at path in bytes.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @param path a string specifying the path to the file.
  * @return the file size in bytes of the file at path.
  */
unsigned int lb_filesystem_file_size( LBFilesystem filesystem, const char* path );


/** Check if the specified path refers to a directory.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @param path a string specifying the path to check.
  * @return 1 if path refers to a directory or 0 if it doesn't.
  */
int lb_filesystem_path_is_directory( LBFilesystem filesystem, const char* path );

/** Creates a directory defined by path.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @param path a string specifying the path of the new directory to create.
  * @return LBFilesystemNoError if a directory was successfully created.
  * LBFilesystemPathNotFound if the specified path is not valid.
  * LBFilesystemDirectoryAlreadyExists if the directory already exists.
  * LBFilesystemUnknownError otherwise.
  */
LBFilesystemError lb_filesystem_create_directory( LBFilesystem filesystem, const char* path );

/** Removes the directory defined by path.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @param path a string specifying the path of the directory to remove.
  * @return LBFilesystemNoError if a directory was successfully removed.
  * LBFilesystemPathNotFound if the specified path is not valid.
  * LBFilesystemUnknownError otherwise.
  */
LBFilesystemError lb_filesystem_remove_directory( LBFilesystem filesystem, const char* path );

/** Lists the files and sub-directories of the specified directory.
  * The first entry is not read until lb_filesystem_directory_next_entry is called.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @param path a string specifying the path of the directory to list.
  * @return 1 if successfully listed directory, 0 otherwise.
  */
int lb_filesystem_directory_list( LBFilesystem filesystem, const char* path );

/** Reads the next entry in the directory list.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @return 1 if an entry was successfully read, 0 otherwise.
  */
int lb_filesystem_directory_next_entry( LBFilesystem filesystem );

/** Returns the current entry in the list.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @return a string with the name of the current directory entry.
  */
const char* lb_filesystem_directory_get_entry( LBFilesystem filesystem );

/** Returns the current working directory path.
  * @param filesystem a pointer to a successfully constructed filesystem object.
  * @return a string with the current working directory path.
  */
const char* lb_filesystem_get_working_directory( LBFilesystem filesystem );

#ifdef __cplusplus
}
#endif
#endif

